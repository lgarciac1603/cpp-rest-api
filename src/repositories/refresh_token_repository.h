#pragma once

#include <vector>
#include <string>
#include <libpq-fe.h>
#include "../models/refresh_token.h"

using namespace std;

class RefreshTokenRepository {
private:
  PGconn* conn;

public:
  RefreshTokenRepository(PGconn* connection) : conn(connection) {}

  bool create(int user_id, const string& token_hash, time_t expires_at);
  bool removeByTokenHash(const string& token_hash);
  bool isValid(const string& token_hash);
};