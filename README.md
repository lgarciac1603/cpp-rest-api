# CPP REST API

A simple and minimalist REST API built in C++ for managing users and sessions. It uses PostgreSQL as the database, httplib for the HTTP server, and JWT for authentication. Includes full authentication with login, logout, refresh tokens, and user sessions.

## Features

- **User CRUD**: Create, read, update, and delete users.
- **JWT Authentication**: Login, logout, refresh tokens, and `/me` endpoint with secure token management.
- **Database**: PostgreSQL with SQL migrations, including tables for users and refresh tokens.
- **Containerization**: Docker Compose for PostgreSQL.

## Prerequisites

- **Operating System**: Windows 10 or higher (code is configured for Windows).
- **C++ Compiler**: g++ (MinGW) or Visual Studio with C++17 support.
- **PostgreSQL**: Development client (libpq). Install PostgreSQL or use vcpkg.
- **Dependencies**:
  - httplib (included in `third_party/httplib.h`).
  - jwt-cpp: For JWT. Download from [GitHub](https://github.com/Thalhammer/jwt-cpp) and place headers in `third_party/jwt-cpp/` or adjust include paths.
  - OpenSSL: For hashing. Install via vcpkg: `vcpkg install openssl`.
  - Docker and Docker Compose: For the database.
- **Tools**: Git, Docker (optional, for containerized builds).

## Installation

1. **Clone the repository**:

   ```bash
   git clone https://github.com/lgarciac1603/cpp-rest-api.git
   cd cpp-rest-api
   ```

2. **Install dependencies with vcpkg** (recommended):
   - Install vcpkg if you don't have it: `git clone https://github.com/Microsoft/vcpkg.git && cd vcpkg && bootstrap-vcpkg.bat`.
   - Install packages: `.\vcpkg install libpq openssl`.
   - Integrate vcpkg: `.\vcpkg integrate install`.

3. **Set up Docker**:
   - Ensure Docker Desktop is installed and running.
   - The `docker-compose.yml` configures PostgreSQL automatically.

## Configuration

1. **Database**:
   - Start PostgreSQL: `docker-compose up -d`.
   - Connect to the DB (user: `apiuser_test`, password: `apipass_test`, DB: `apidb`, port: 5432).
   - Run migrations in order:

     ```sql
     -- Connect via psql or tool like pgAdmin
     \i database/migrations/001-init.sql
     \i database/migrations/002-triggers.sql
     \i database/migrations/003-users-population.sql
     \i database/migrations/004-refresh-tokens.sql
     ```

     - The new migration `004-refresh-tokens.sql` adds the table for secure token storage.

   - If you get `Missing DB env vars...`, do:
     1. `docker-compose down -v` to clean old containers/volumes.
     2. `docker-compose up --build`.
     3. Verify with `docker-compose logs api --tail=50`; should show:
        - `DB_HOST: postgres`
        - `DB_PORT: 5432`
        - `DB_USER: apiuser_test`
        - `DB_PASS: apipass_test`
        - `DB_NAME: apidb`

   - Note: `docker-entrypoint.sh` checks for these vars before running migrations.

2. **App Configuration**:
   - Edit `src/config/config.h` to adjust settings (e.g., server port, JWT secret).
   - The JWT secret is hardcoded in `auth_routes.cpp` as `"your-secret-key"` – change it in production.

## Compilation

1. **Using Docker** (recommended for production-like deployment):
   - Make sure `Docker` and `docker-compose` are installed.
   - Run:
     ```bash
     docker-compose up --build
     ```
   - This creates 2 containers:
     - `postgres_minimal`: PostgreSQL 16 with persistent volume (`pgdata`).
     - `cpp-rest-api`: app built from `Dockerfile`.
   - `docker-entrypoint.sh` waits for PostgreSQL, applies migrations from `database/migrations/*.sql`, and starts `./build/api`.
   - API is available at `http://localhost:8080`.

2. **Direct Docker use (without `docker-compose`)**:
   - Build image:
     ```bash
     docker build -t cpp-rest-api .
     ```
   - Start PostgreSQL:
     ```bash
     docker run -d --name postgres_minimal -e POSTGRES_USER=apiuser_test -e POSTGRES_PASSWORD=apipass_test -e POSTGRES_DB=apidb -p 5432:5432 -v pgdata:/var/lib/postgresql/data postgres:16
     ```
   - Run the app:
     ```bash
     docker run --rm --name cpp-rest-api \
       --link postgres_minimal:postgres \
       -e DB_HOST=postgres -e DB_PORT=5432 -e DB_NAME=apidb -e DB_USER=apiuser_test -e DB_PASS=apipass_test \
       -p 8080:8080 cpp-rest-api
     ```

3. **Using g++ (Windows with MinGW/MSYS2)**:
   - Create build folder: `mkdir build`.
   - Compile:
     ```bash
     g++ -std=c++17 -o build/api.exe src/main.cpp src/db/connection.cpp src/repositories/user_repository.cpp src/repositories/refresh_token_repository.cpp src/routes/user_routes.cpp src/routes/auth_routes.cpp src/utils/hash.cpp src/utils/session_manager.cpp -Isrc/third_party -Isrc/third_party/jwt-cpp -Isrc/config -lpq -lssl -lcrypto -lws2_32
     ```
   - Adjust include/lib paths according to your `vcpkg` installation.
   - Ensure dependency: `third_party/nlohmann/json.hpp` and `jwt-cpp` exist in `third_party/jwt-cpp`.

- **Note**: for manual mode, ensure in `auth_routes.cpp`:
  - `#include <nlohmann/json.hpp>`
  - `#define JWT_DISABLE_PICOJSON` (already applied in this repo).

## Execution

1. **With Docker**: `docker-compose up --build` (builds and runs everything).
2. **Manual**:
   - Start the DB: `docker-compose up -d postgres` (if not using full compose).
   - Run the app: `./api.exe` (or `api.exe` on Windows).
3. **Verify**: The server listens on `http://localhost:8080`. Logs will appear in the console.

## Usage

### User Endpoints

- `GET /users`: List all users (returns JSON).
- `GET /users/{id}`: Get a user by ID.
- `POST /users`: Create user (body: `username`, `email`, `password`).
- `PUT /users/{id}`: Update email (body: `email`).
- `DELETE /users/{id}`: Delete user.

### Authentication Endpoints

- `POST /sessions`: Login (body: `email`, `password`) → Returns `access_token` and `refresh_token`.
- `GET /me`: Current user (header: `Authorization: Bearer <access_token>`).
- `DELETE /sessions`: Logout (body: `refresh_token`).
- `POST /sessions/refresh`: Refresh token (body: `refresh_token`) → New `access_token`.

### Usage Example

```bash
# Create user
curl -X POST http://localhost:8080/users -d "username=test&email=test@example.com&password=123456"

# Login
curl -X POST http://localhost:8080/sessions -d "email=test@example.com&password=123456"

# Get user (use token from login)
curl -H "Authorization: Bearer <token>" http://localhost:8080/me
```

## Development

- **Project Structure**:
  - `src/`: Source code (now includes `auth_routes.cpp`, `refresh_token_repository.cpp`, and updated models).
  - `database/`: Migrations and seeds (added `004-refresh-tokens.sql`).
  - `build/`: Build files.
- **Adding New Routes**: Edit `routes/` and register in `main.cpp`.
- **Tests**: Not included; add unit tests with Google Test.

## Contributing

1. Fork the repo.
2. Create a branch: `git checkout -b feature/new-feature`.
3. Commit: `git commit -m "Add new feature"`.
4. Push: `git push origin feature/new-feature`.
5. Open a PR.

## License

MIT License. See `LICENSE`.

## Notes

- This project is educational and minimalist. For production, add rate limiting, robust validations, and use HTTPS.
- If you encounter issues, check console logs or DB connections.
- jwt-cpp is installed manually in `third_party/jwt-cpp/`; ensure the headers are present before compiling.
- The Docker container for the app is named `cpp-rest-api` and connects to the `postgres` container for the database.
