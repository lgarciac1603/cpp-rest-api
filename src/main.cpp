#define _WIN32_WINNT 0x0A00

#include <iostream>
#include <cstdlib>
#include "db/connection.h"
#include "config/config.h"
#include "third_party/httplib.h"
#include "routes/user_routes.h"
#include "routes/auth_routes.h"
#include "utils/console-colors.h"
#include "utils/session_manager.h"
#include "utils/cors.h"

using namespace std;

int main() {
  DBConnection db;

  if (!db.isConnected()) {
    cerr << COLOR_RED
      << "Database connection failed"
      << COLOR_RESET << "\n";
      return 1;
  }

  cout << COLOR_GREEN
    << "Database connection successfully"
    << COLOR_RESET << "\n";

  PGresult* res = PQexec(db.get(), "SELECT 1");

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    cerr << COLOR_RED
      << "Test query failed"
      << COLOR_RESET << "\n";
    
    PQclear(res);
    return 1;
  }
  
  PQclear(res);

  cout << COLOR_GREEN
    << "Test query OK"
    << COLOR_RESET << "\n";

  // --- HTTP Server ---
  httplib::Server api;
  SessionManager session_mgr;
  register_user_routes(api, db, session_mgr);
  register_auth_routes(api, db, session_mgr);
  
  // Setup CORS
  CORS::handle_preflight(api);

  int app_port = std::atoi(APP_PORT);
  if (app_port <= 0) {
    app_port = 8080;
  }

  cout << COLOR_GREEN
    << "API listening on http://localhost:" << app_port
    << COLOR_RESET << "\n";

  api.listen("0.0.0.0", app_port);

  return 0;
}
