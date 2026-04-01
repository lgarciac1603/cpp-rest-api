#pragma once

#include "../third_party/httplib.h"

namespace CORS {
  inline void set_headers(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
  }

  inline void handle_preflight(httplib::Server& api) {
    api.Options("/users", [](const httplib::Request&, httplib::Response& res) {
      CORS::set_headers(res);
    });
    api.Options("/users/:id", [](const httplib::Request&, httplib::Response& res) {
      CORS::set_headers(res);
    });
    api.Options("/sessions", [](const httplib::Request&, httplib::Response& res) {
      CORS::set_headers(res);
    });
    api.Options("/sessions/refresh", [](const httplib::Request&, httplib::Response& res) {
      CORS::set_headers(res);
    });
    api.Options("/me", [](const httplib::Request&, httplib::Response& res) {
      CORS::set_headers(res);
    });
  }
}
