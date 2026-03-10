#pragma once

#include <libpq-fe.h>

class DBConnection {
  public:
    DBConnection();
    ~DBConnection(); // constructor

    bool isConnected() const;
    PGconn* get();

  private:
    PGconn* conn;
};
