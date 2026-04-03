#pragma once

#include "../third_party/httplib.h"
#include "../config/config.h"

namespace CORS {
  inline void set_headers(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", CORS_ALLOW_ORIGIN);
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
  }

  inline void handle_preflight(httplib::Server& api) {
    api.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
      CORS::set_headers(res);
    });
  }
}
