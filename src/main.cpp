#include <iostream>
#include "db/connection.h"
#include "utils/console-colors.h"

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
  } else {
    cout << COLOR_GREEN
    << "Test query OK"
    << COLOR_RESET << "\n";
  }

  PQclear(res);
  return 0;
}
