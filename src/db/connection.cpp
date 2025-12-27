#include "connection.h"
#include "../config/config.h"

#include <iostream>
#include <sstream>

using namespace std;

DBConnection::DBConnection() :conn(nullptr) {
  stringstream ss;

  ss  << "host=" << DB_HOST
      << " port=" << DB_PORT
      << " dbname=" << DB_NAME
      << " user=" << DB_USER
      << " password=" << DB_PASS;

  conn = PQconnectdb(ss.str().c_str());

  if (PQstatus(conn) != CONNECTION_OK) {
    cerr << "[DB] Connection failed" << PQerrorMessage(conn);
    PQfinish(conn);
    conn = nullptr;
  };
};

DBConnection::~DBConnection() {
  if(conn) {
    PQfinish(conn);
    conn = nullptr;
  };
};

bool DBConnection::isConnected() const {
  return conn != nullptr;
};

PGconn* DBConnection::get() {
  return conn;
};
