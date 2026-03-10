#include "refresh_token_repository.h"
#include <sstream>
#include <iomanip>

using namespace std;

bool RefreshTokenRepository::create(int user_id, const string& token_hash, time_t expires_at) {
    ostringstream query;
    query << "INSERT INTO refresh_tokens (user_id, token_hash, expires_at) VALUES ("
          << user_id << ", '" << token_hash << "', to_timestamp(" << expires_at << "))";

    PGresult* res = PQexec(conn, query.str().c_str());
    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    PQclear(res);
    return success;
}

bool RefreshTokenRepository::removeByTokenHash(const string& token_hash) {
    ostringstream query;
    query << "DELETE FROM refresh_tokens WHERE token_hash = '" << token_hash << "'";

    PGresult* res = PQexec(conn, query.str().c_str());
    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    PQclear(res);
    return success;
}

bool RefreshTokenRepository::isValid(const string& token_hash) {
    ostringstream query;
    query << "SELECT 1 FROM refresh_tokens WHERE token_hash = '" << token_hash
          << "' AND expires_at > now() LIMIT 1";

    PGresult* res = PQexec(conn, query.str().c_str());
    bool valid = PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0;
    PQclear(res);
    return valid;
}