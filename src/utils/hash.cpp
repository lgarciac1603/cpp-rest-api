#include "hash.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>

using namespace std;

string hash_password(const string& password) {
  // random 16-bytes salt
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> dis(0, 255);
  string salt;
  
  for (int i = 0; i < 16; ++i) {
    salt += static_cast<char>(dis(gen));
  }

  string to_hash = salt + password;

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256(reinterpret_cast<const unsigned char*>(to_hash.c_str()), to_hash.size(), hash);

  stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
  }

  return salt + ":" + ss.str();
}

bool verify_password(const string& password, const string& stored_hash) {
  size_t colon_pos = stored_hash.find(':');

  if (colon_pos == string::npos) {
    return false;
  };

  string salt = stored_hash.substr(0, colon_pos);
  string expected_hash = stored_hash.substr(colon_pos + 1);

  string to_hash = salt + password;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256(reinterpret_cast<const unsigned char*>(to_hash.c_str()), to_hash.size(), hash);

  stringstream ss;
  
  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
  }

  return ss.str() == expected_hash;
}