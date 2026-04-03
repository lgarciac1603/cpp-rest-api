#pragma once

#include <string>

using namespace std;

string hash_password(const string& password);
string hash_refresh_token(const string& refresh_token);
bool verify_password(const string& password, const string& stored_hash);