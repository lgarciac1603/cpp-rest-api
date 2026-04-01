#define _WIN32_WINNT 0x0A00

#include "auth_routes.h"
#include "../repositories/user_repository.h"
#include "../repositories/refresh_token_repository.h"
#include "../utils/hash.h"
#include "../utils/cors.h"
#define JWT_DISABLE_PICOJSON
#include <nlohmann/json.hpp>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include <chrono>
#include <sstream>

using namespace std;
using json = nlohmann::json;

// Helper para generar JWT
string generate_access_token(int user_id, const string& secret) {
  auto now = chrono::system_clock::now();
  auto exp = now + chrono::hours(1);  // 1 hora

  return jwt::create<jwt::traits::nlohmann_json>()
    .set_issuer("cpp-api")
    .set_subject(to_string(user_id))
    .set_issued_at(now)
    .set_expires_at(exp)
    .sign(jwt::algorithm::hs256{secret});
}

string generate_refresh_token() {
  // Simple random string, en producción usar algo mejor
  return "refresh_" + to_string(rand()) + "_" + to_string(time(nullptr));
}

int get_user_id_from_token(const string& token, const string& secret) {
  try {
    auto decoded = jwt::decode<jwt::traits::nlohmann_json>(token);
    auto verifier = jwt::verify<jwt::traits::nlohmann_json>()
      .with_issuer("cpp-api")
      .allow_algorithm(jwt::algorithm::hs256{secret});

    verifier.verify(decoded);
    return stoi(decoded.get_subject());
  } catch (...) {
    return -1;
  }
}

void register_auth_routes(httplib::Server& api, DBConnection& db, SessionManager& session_mgr) {
  string jwt_secret = "your-secret-key";  // Mover a config

  // POST /sessions (login)
  api.Post("/sessions", [&](const httplib::Request& req, httplib::Response& res) {
    CORS::set_headers(res);
    auto email = req.get_param_value("email");
    auto password = req.get_param_value("password");

    if (email.empty() || password.empty()) {
      res.status = 400;
      res.set_content("{\"error\":\"missing_fields\"}", "application/json");
      return;
    }

    UserRepository user_repo(db.get());
    auto user = user_repo.findByEmail(email);
    if (!user || !verify_password(password, user->password_hash)) {
      res.status = 401;
      res.set_content("{\"error\":\"invalid_credentials\"}", "application/json");
      return;
    }

    string access_token = generate_access_token(user->id, jwt_secret);
    string refresh_token_str = generate_refresh_token();
    string refresh_hash = hash_password(refresh_token_str);  // Usar hash para almacenar

    time_t expires_at = time(nullptr) + 7 * 24 * 3600;  // 7 días
    RefreshTokenRepository token_repo(db.get());
    if (!token_repo.create(user->id, refresh_hash, expires_at)) {
      res.status = 500;
      res.set_content("{\"error\":\"token_creation_failed\"}", "application/json");
      return;
    }

    ostringstream response;
    response << "{\"access_token\":\"" << access_token
      << "\",\"refresh_token\":\"" << refresh_token_str
      << "\",\"expires_in\":3600}";
    res.set_content(response.str(), "application/json");
  });

  // GET /me
  api.Get("/me", [&](const httplib::Request& req, httplib::Response& res) {
    CORS::set_headers(res);
    auto auth_header = req.get_header_value("Authorization");
    if (auth_header.find("Bearer ") != 0) {
      res.status = 401;
      res.set_content("{\"error\":\"missing_token\"}", "application/json");
      return;
    }

    string token = auth_header.substr(7);
    int user_id = get_user_id_from_token(token, jwt_secret);
    if (user_id == -1) {
      res.status = 401;
      res.set_content("{\"error\":\"invalid_token\"}", "application/json");
      return;
    }

    UserRepository user_repo(db.get());
    auto user = user_repo.findById(user_id);
    if (!user) {
      res.status = 404;
      res.set_content("{\"error\":\"user_not_found\"}", "application/json");
      return;
    }

    ostringstream response;
    response << "{\"id\":" << user->id
      << ",\"username\":\"" << user->username
      << "\",\"email\":\"" << user->email << "\"}";
    res.set_content(response.str(), "application/json");
  });

  // DELETE /sessions (logout)
  api.Delete("/sessions", [&](const httplib::Request& req, httplib::Response& res) {
    CORS::set_headers(res);
    auto refresh_token = req.get_param_value("refresh_token");
    if (refresh_token.empty()) {
      res.status = 400;
      res.set_content("{\"error\":\"missing_refresh_token\"}", "application/json");
      return;
    }

    string refresh_hash = hash_password(refresh_token);
    RefreshTokenRepository token_repo(db.get());
    if (!token_repo.removeByTokenHash(refresh_hash)) {
      res.status = 400;
      res.set_content("{\"error\":\"invalid_token\"}", "application/json");
      return;
    }

    res.status = 204;
  });

  // POST /sessions/refresh
  api.Post("/sessions/refresh", [&](const httplib::Request& req, httplib::Response& res) {
    CORS::set_headers(res);
    auto refresh_token = req.get_param_value("refresh_token");
    if (refresh_token.empty()) {
      res.status = 400;
      res.set_content("{\"error\":\"missing_refresh_token\"}", "application/json");
      return;
    }

    string refresh_hash = hash_password(refresh_token);
    RefreshTokenRepository token_repo(db.get());
    if (!token_repo.isValid(refresh_hash)) {
      res.status = 401;
      res.set_content("{\"error\":\"invalid_refresh_token\"}", "application/json");
      return;
    }

    // Obtener user_id de la DB
    ostringstream query;
    query << "SELECT user_id FROM refresh_tokens WHERE token_hash = '" << refresh_hash << "'";
    PGresult* pg_res = PQexec(db.get(), query.str().c_str());
    if (PQresultStatus(pg_res) != PGRES_TUPLES_OK || PQntuples(pg_res) == 0) {
      PQclear(pg_res);
      res.status = 401;
      res.set_content("{\"error\":\"invalid_refresh_token\"}", "application/json");
      return;
    }

    int user_id = atoi(PQgetvalue(pg_res, 0, 0));
    PQclear(pg_res);

    string new_access_token = generate_access_token(user_id, jwt_secret);
    ostringstream response;
    response << "{\"access_token\":\"" << new_access_token
      << "\",\"expires_in\":3600}";
    res.set_content(response.str(), "application/json");
  });
}
