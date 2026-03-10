#define _WIN32_WINNT 0x0A00

#include "user_routes.h"
#include "../repositories/user_repository.h"
#include "../utils/hash.h"

using namespace std;

static string to_json(const User& u) {
  ostringstream s;

  s << "{\"id\":" << u.id
    << ",\"username\":\"" << u.username
    << "\",\"email\":\"" << u.email << "\"}";
  return s.str();
}

void register_user_routes(httplib::Server& api, DBConnection& db, SessionManager& session_mgr) {
  // get all users
  api.Get("/users", [&](const httplib::Request&, httplib::Response& res) {
    cout << "GET /users\n";
    UserRepository repo(db.get());
    auto users = repo.findAll();

    ostringstream s;
    s << "[";

    for (size_t i = 0; i < users.size(); ++i) {
      s << to_json(users[i]);
      if (i + 1 < users.size()) s << ",";
    }

    s << "]";
    res.set_content(s.str(), "application/json");
  });

  // get one user by id
  api.Get(R"(/users/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
    cout << "GET /users/id\n";
    UserRepository repo(db.get());

    int id = stoi(req.matches[1]);
    auto u = repo.findById(id);
    if (!u) { res.status = 404; return; }
    res.set_content(to_json(*u), "application/json");
  });

  // post user
  api.Post("/users", [&](const httplib::Request& req, httplib::Response& res) {
    cout << "POST /users\n";
    auto username = req.get_param_value("username");
    auto email = req.get_param_value("email");
    auto pass = req.get_param_value("password");

    if (username.empty() || email.empty() || pass.empty()) {
      res.status = 400;
      res.set_content("{\"error\":\"Missing fields\"}", "application/json");
      return;
    }

    std::string hashed = hash_password(pass);

    UserRepository repo(db.get());

    if (!repo.create(username, email, hashed)) {
      res.status = 400;
      res.set_content("{\"error\":\"User creation failed\"}", "application/json");
      return;
    }

    res.status = 201;
    res.set_content("{\"message\":\"User created\"}", "application/json");
  });

  // edit user with id
  api.Put(R"(/users/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
    cout << "put /users/id\n";
    int id = stoi(req.matches[1]);
    auto email = req.get_param_value("email");

    UserRepository repo(db.get());

    if (!repo.update(id, email)) { res.status = 400; return; }

    res.status = 204;
  });

  api.Delete(R"(/users/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
    cout << "DELETE /users/id\n";
    int id = stoi(req.matches[1]);

    UserRepository repo(db.get());

    if (!repo.remove(id)) {
      res.status = 400;
      return;
    }

    res.status = 204;
  });

  // login user (return token)
  api.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
    cout << "POST /login\n";
    auto username = req.get_param_value("username");
    auto password = req.get_param_value("password");

    if (username.empty() || password.empty()) {
      res.status = 400;
      res.set_content("{\"error\":\"Missing credentials\"}", "application/json");
      return;
    }

    UserRepository repo(db.get());
    auto user = repo.authenticate(username, password);
    if (!user) {
      res.status = 401;
      res.set_content("{\"error\":\"Invalid credentials\"}", "application/json");
      return;
    }

    std::string token = session_mgr.create_session(*user);
    res.set_content("{\"token\":\"" + token + "\"}", "application/json");
  });

  // test token validation
  api.Get("/validate-session", [&](const httplib::Request& req, httplib::Response& res) {
    cout << "GET /validate-session\n";
    auto token = req.get_header_value("Authorization");
    if (token.empty()) {
      res.status = 401;
      res.set_content("{\"error\":\"No token provided\"}", "application/json");
      return;
    }

    auto user = session_mgr.validate_session(token);
    if (!user) {
      res.status = 401;
      res.set_content("{\"error\":\"Invalid or expired token\"}", "application/json");
      return;
    }

    res.set_content("{\"valid\":true,\"user\":" + to_json(*user) + "}", "application/json");
  });
}
