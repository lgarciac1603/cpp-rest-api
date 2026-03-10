#pragma once

#include <string>
#include <ctime>

using namespace std;

struct RefreshToken {
    int id;
    int user_id;
    string token_hash;
    time_t expires_at;
    time_t created_at;
};