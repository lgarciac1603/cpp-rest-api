#define _WIN32_WINNT 0x0A00

#pragma once

#include "httplib.h"
#include "../db/connection.h"
#include "../utils/session_manager.h"
#include "../repositories/refresh_token_repository.h"

void register_auth_routes(httplib::Server& api, DBConnection& db, SessionManager& session_mgr);
