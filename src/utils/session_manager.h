#pragma once

#include <map>
#include <string>
#include <chrono>
#include <optional>
#include "../models/user.h"

using namespace std;

class SessionManager {
  public:
    string create_session(const User& user);
    optional<User> validate_session(const string& token);
  private:
    map<string, pair<User, chrono::time_point<chrono::system_clock>>> sessions;
};