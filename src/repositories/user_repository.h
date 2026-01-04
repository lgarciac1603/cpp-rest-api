#pragma once // include only once on the compiler

#include <vector>
#include <optional>
#include <libpq-fe.h>
#include "../models/user.h"

class UserRepository {
  public:
    explicit UserRepository(PGconn* conn);

    vector<User> findAll();
    optional<User> findById(int id);
    optional<User> findByUsername(const string& username);
    optional<User> authenticate(const string& username, const string& password);

    bool create(const string& username, const string& email, const string& password_hash);
    bool update(int id, const string& email);
    bool remove(int id);
  private:
    PGconn* conn;
};
