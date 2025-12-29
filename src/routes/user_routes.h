#define _WIN32_WINNT 0x0A00  // Windows 10

#pragma once

#include "third_party/httplib.h"
#include "../db/connection.h"

void register_user_routes(httplib::Server& api, DBConnection& db);
