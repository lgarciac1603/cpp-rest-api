#include <iostream>
#include "../utils/hash.h"
#include "user_repository.h"

using namespace std;

UserRepository::UserRepository(PGconn* c) : conn(c) {}

vector<User> UserRepository::findAll() {
  vector<User> users;
  PGresult* r = PQexec(conn, "SELECT id, username, email FROM users ORDER BY id");

  if (PQresultStatus(r) != PGRES_TUPLES_OK) {
    PQclear(r);
    return users;
  }

  int rows = PQntuples(r);
  for (int i = 0; i < rows; ++i) {
    users.push_back({
      stoi(PQgetvalue(r, i, 0)),
      PQgetvalue(r, i, 1),
      PQgetvalue(r, i, 2)
    });
  }
  
  PQclear(r);
  return users;
}

optional<User> UserRepository::findById(int id) {
  const char* params[1] = { std::to_string(id).c_str() };

  PGresult* r = PQexecParams(
    conn,
    "SELECT id, username, email FROM users WHERE id=$1",
    1, nullptr, params, nullptr, nullptr, 0
  );

  if (PQresultStatus(r) != PGRES_TUPLES_OK || PQntuples(r) == 0) {
    PQclear(r);
    return nullopt;
  }

  User u {
    stoi(PQgetvalue(r, 0, 0)),
      PQgetvalue(r, 0, 1),
      PQgetvalue(r, 0, 2)
    };

  PQclear(r);
  return u;
}

optional<User> UserRepository::findByUsername(const string& username) {
  const char* params[1] = { username.c_str() };

  PGresult* r = PQexecParams(
    conn,
    "SELECT id, username, email FROM users WHERE username=$1",
    1, nullptr, params, nullptr, nullptr, 0
  );

  if (PQresultStatus(r) != PGRES_TUPLES_OK || PQntuples(r) == 0) {
    PQclear(r);
    return nullopt;
  }

  User u {
    stoi(PQgetvalue(r, 0, 0)),
    PQgetvalue(r, 0, 1),
    PQgetvalue(r, 0, 2)
  };

  PQclear(r);
  return u;
}

optional<User> UserRepository::authenticate(const string& username, const string& password) {
  const char* params[1] = { username.c_str() };

  PGresult* r = PQexecParams(
    conn,
    "SELECT id, username, email, password_hash FROM users WHERE username=$1",
    1, nullptr, params, nullptr, nullptr, 0
  );

  if (PQresultStatus(r) != PGRES_TUPLES_OK || PQntuples(r) == 0) {
    PQclear(r);
    return nullopt;
  }

  std::string stored_hash = PQgetvalue(r, 0, 3);

  if (!verify_password(password, stored_hash)) {
    PQclear(r);
    return nullopt;
  }

  User u {
    stoi(PQgetvalue(r, 0, 0)),
    PQgetvalue(r, 0, 1),
    PQgetvalue(r, 0, 2)
  };

  PQclear(r);
  return u;
}

bool UserRepository::create(const string& username, const string& email, const string& password_hash) {
  const char* params[3] = { username.c_str(), email.c_str(), password_hash.c_str() };

  PGresult* r = PQexecParams(conn,
    "INSERT INTO users (username, email, password_hash) VALUES ($1,$2,$3)",
    3, nullptr, params, nullptr, nullptr, 0
  );

  bool ok = PQresultStatus(r) == PGRES_COMMAND_OK;
  PQclear(r);
  return ok;
}

bool UserRepository::update(int id, const string& email) {
  const char* params[2] = { email.c_str(), to_string(id).c_str() };

  PGresult* r = PQexecParams(conn,
    "UPDATE users SET email=$1 WHERE id=$2",
    2, nullptr, params, nullptr, nullptr, 0
  );
  
  bool ok = PQresultStatus(r) == PGRES_COMMAND_OK;
  PQclear(r);
  return ok;
}

bool UserRepository::remove(int id) {
  const char* params[1] = { std::to_string(id).c_str() };

  PGresult* r = PQexecParams(conn,
    "DELETE FROM users WHERE id=$1",
    1, nullptr, params, nullptr, nullptr, 0
  );

  bool ok = PQresultStatus(r) == PGRES_COMMAND_OK;

  PQclear(r);
  return ok;
}
