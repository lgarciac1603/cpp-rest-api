# Configuration Files

This directory contains configuration files for database connection settings.

## config.h

The main configuration file that uses environment variables with fallback defaults.

It defines macros that read from environment variables:

- `DB_HOST`: Database host (default: "localhost")
- `DB_PORT`: Database port (default: "5432")
- `DB_NAME`: Database name (default: "apidb")
- `DB_USER`: Database user (default: "apiuser_test")
- `DB_PASS`: Database password (default: "apipass_test")

### Usage with Environment Variables (Recommended)

Set environment variables before running the app:

```bash
export DB_HOST="your-host"
export DB_PORT="5432"
export DB_NAME="your-db"
export DB_USER="your-user"
export DB_PASS="your-password"
./api.exe
```

Or in Docker Compose (as in the root docker-compose.yml):

```yaml
environment:
  - DB_HOST=postgres
  - DB_PORT=5432
  - DB_USER=apiuser_test
  - DB_PASS=apipass_test
  - DB_NAME=apidb
```

### Hardcoded Values (Alternative)

If you prefer hardcoded values, edit `config.h` directly:

```cpp
#define DB_HOST "your-host"
#define DB_PORT "5432"
#define DB_NAME "your-db"
#define DB_USER "your-user"
#define DB_PASS "your-password"
```

## config.local.h

An example file with hardcoded defaults (DB_USER and DB_PASS are empty).

You can copy this to `config.h` and modify as needed, but `config.h` already supports env vars.

## Docker Compose

The root `docker-compose.yml` sets up a test PostgreSQL instance with these defaults, so no changes needed for local development.
