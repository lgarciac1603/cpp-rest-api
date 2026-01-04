#include "session_manager.h"
#include <random>
#include <sstream>
#include <iomanip>

using namespace std;

string generate_random_token() {
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> dis(0, 255);
  string bytes;

  for (int i = 0; i < 16; ++i) {
    bytes += static_cast<char>(dis(gen));
  }

  stringstream ss;

  for (char c : bytes) {
    ss << hex << setw(2) << setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
  }

  return ss.str();
}

string SessionManager::create_session(const User& user) {
  string token = generate_random_token();
  auto expiry = chrono::system_clock::now() + chrono::minutes(15);

  sessions[token] = {user, expiry};
  return token;
}

optional<User> SessionManager::validate_session(const string& token) {
  auto it = sessions.find(token);

  if (it == sessions.end()) return nullopt;

  if (chrono::system_clock::now() > it->second.second) {
    sessions.erase(it);
    return std::nullopt;
  }

  return it->second.first;
}
