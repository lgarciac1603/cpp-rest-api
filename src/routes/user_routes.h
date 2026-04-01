#define _WIN32_WINNT 0x0A00  // Windows 10

#pragma once

#include "httplib.h"
#include "../db/connection.h"
#include "../utils/session_manager.h"

void register_user_routes(httplib::Server& api, DBConnection& db, SessionManager& session_mgr);
