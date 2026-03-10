#pragma once

#include <string>

using namespace std;

string hash_password(const string& password);
bool verify_password(const string& password, const string& stored_hash);