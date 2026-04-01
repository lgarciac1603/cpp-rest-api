#include "refresh_token_repository.h"

using namespace std;

bool RefreshTokenRepository::create(int user_id, const string& token_hash, time_t expires_at) {
  string user_id_str = to_string(user_id);
  string expires_at_str = to_string(expires_at);
  const char* params[3] = { user_id_str.c_str(), token_hash.c_str(), expires_at_str.c_str() };

  PGresult* res = PQexecParams(
    conn,
    "INSERT INTO refresh_tokens (user_id, token_hash, expires_at) VALUES ($1, $2, to_timestamp($3::bigint))",
    3, nullptr, params, nullptr, nullptr, 0
  );
  bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
  PQclear(res);
  return success;
}

bool RefreshTokenRepository::removeByTokenHash(const string& token_hash) {
  const char* params[1] = { token_hash.c_str() };

  PGresult* res = PQexecParams(
    conn,
    "DELETE FROM refresh_tokens WHERE token_hash = $1",
    1, nullptr, params, nullptr, nullptr, 0
  );
  bool success = PQresultStatus(res) == PGRES_COMMAND_OK && atoi(PQcmdTuples(res)) > 0;
  PQclear(res);
  return success;
}

bool RefreshTokenRepository::isValid(const string& token_hash) {
  const char* params[1] = { token_hash.c_str() };

  PGresult* res = PQexecParams(
    conn,
    "SELECT 1 FROM refresh_tokens WHERE token_hash = $1 AND expires_at > now() LIMIT 1",
    1, nullptr, params, nullptr, nullptr, 0
  );
  bool valid = PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0;
  PQclear(res);
  return valid;
}

optional<int> RefreshTokenRepository::findUserIdByTokenHash(const string& token_hash) {
  const char* params[1] = { token_hash.c_str() };

  PGresult* res = PQexecParams(
    conn,
    "SELECT user_id FROM refresh_tokens WHERE token_hash = $1 AND expires_at > now() LIMIT 1",
    1, nullptr, params, nullptr, nullptr, 0
  );

  if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
    PQclear(res);
    return nullopt;
  }

  int user_id = atoi(PQgetvalue(res, 0, 0));
  PQclear(res);
  return user_id;
}