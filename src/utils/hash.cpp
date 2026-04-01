#include "hash.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>

using namespace std;

static string sha256_hex(const string& value) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256(reinterpret_cast<const unsigned char*>(value.c_str()), value.size(), hash);

  stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
  }

  return ss.str();
}

string hash_password(const string& password) {
  // random 16-bytes salt
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> dis(0, 255);
  string salt_hex;
  
  for (int i = 0; i < 16; ++i) {
    int byte_value = dis(gen);
    stringstream hex_byte;
    hex_byte << hex << setw(2) << setfill('0') << byte_value;
    salt_hex += hex_byte.str();
  }

  return salt_hex + ":" + sha256_hex(salt_hex + password);
}

string hash_refresh_token(const string& refresh_token) {
  return sha256_hex(refresh_token);
}

bool verify_password(const string& password, const string& stored_hash) {
  size_t colon_pos = stored_hash.find(':');

  if (colon_pos == string::npos) {
    return false;
  };

  string salt_hex = stored_hash.substr(0, colon_pos);
  string expected_hash = stored_hash.substr(colon_pos + 1);

  return sha256_hex(salt_hex + password) == expected_hash;
}