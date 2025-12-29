#include "user_repository.h"
#include <iostream>

UserRepository::UserRepository(PGconn* c) : conn(c) {}

std::vector<User> UserRepository::findAll() {
    std::vector<User> users;
    PGresult* r = PQexec(conn, "SELECT id, username, email FROM users ORDER BY id");
    if (PQresultStatus(r) != PGRES_TUPLES_OK) { PQclear(r); return users; }

    int rows = PQntuples(r);
    for (int i = 0; i < rows; ++i) {
        users.push_back({
            std::stoi(PQgetvalue(r, i, 0)),
            PQgetvalue(r, i, 1),
            PQgetvalue(r, i, 2)
        });
    }
    PQclear(r);
    return users;
}

std::optional<User> UserRepository::findById(int id) {
    const char* params[1] = { std::to_string(id).c_str() };
    PGresult* r = PQexecParams(conn,
        "SELECT id, username, email FROM users WHERE id=$1",
        1, nullptr, params, nullptr, nullptr, 0);

    if (PQresultStatus(r) != PGRES_TUPLES_OK || PQntuples(r) == 0) {
        PQclear(r); return std::nullopt;
    }

    User u {
        std::stoi(PQgetvalue(r, 0, 0)),
        PQgetvalue(r, 0, 1),
        PQgetvalue(r, 0, 2)
    };
    PQclear(r);
    return u;
}

bool UserRepository::create(const std::string& username, const std::string& email, const std::string& password_hash) {
    const char* params[3] = { username.c_str(), email.c_str(), password_hash.c_str() };
    PGresult* r = PQexecParams(conn,
        "INSERT INTO users (username, email, password_hash) VALUES ($1,$2,$3)",
        3, nullptr, params, nullptr, nullptr, 0);
    bool ok = PQresultStatus(r) == PGRES_COMMAND_OK;
    PQclear(r);
    return ok;
}

bool UserRepository::update(int id, const std::string& email) {
    const char* params[2] = { email.c_str(), std::to_string(id).c_str() };
    PGresult* r = PQexecParams(conn,
        "UPDATE users SET email=$1 WHERE id=$2",
        2, nullptr, params, nullptr, nullptr, 0);
    bool ok = PQresultStatus(r) == PGRES_COMMAND_OK;
    PQclear(r);
    return ok;
}

bool UserRepository::remove(int id) {
    const char* params[1] = { std::to_string(id).c_str() };
    PGresult* r = PQexecParams(conn,
        "DELETE FROM users WHERE id=$1",
        1, nullptr, params, nullptr, nullptr, 0);
    bool ok = PQresultStatus(r) == PGRES_COMMAND_OK;
    PQclear(r);
    return ok;
}
