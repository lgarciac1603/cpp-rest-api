# Configuration Files

This directory contains the configuration headers for `cpp-rest-api`. There are now three relevant files with distinct purposes:

| File | Purpose |
|---|---|
| `config.h` | **Runtime config** — reads values from environment variables with safe defaults. Used by the application in all modes. |
| `config.local.h` | **Local private file** — your machine-specific values for compiling and running natively without environment variables. Keep it local and do not commit it. |
| `config.example.h` | **Example template** — committed sample showing the shape of a valid local config. Copy this when you need to create your own `config.local.h`. |

---

## `config.h` — Runtime (environment variables)

This is the file `#include`d by the application. It defines macros that resolve at runtime by reading environment variables, falling back to defaults if a variable is not set.

### Variables

| Macro | Env Variable | Default | Description |
|---|---|---|---|
| `DB_HOST` | `DB_HOST` | `localhost` | PostgreSQL host |
| `DB_PORT` | `DB_PORT` | `5432` | PostgreSQL port |
| `DB_NAME` | `DB_NAME` | `apidb` | Database name |
| `DB_USER` | `DB_USER` | `apiuser_test` | Database user |
| `DB_PASS` | `DB_PASS` | `apipass_test` | Database password |
| `APP_PORT` | `APP_PORT` | `8080` | HTTP port the API listens on |
| `JWT_SECRET` | `JWT_SECRET` | `dev-secret-key` | Secret for signing/verifying JWT tokens |
| `CORS_ALLOW_ORIGIN` | `CORS_ALLOW_ORIGIN` | `http://localhost:4200` | Allowed CORS origin |

> ⚠️ The defaults are safe for local development only. **Always override `JWT_SECRET` and `DB_PASS` in any non-local environment.**

---

### Standalone mode

When running `cpp-rest-api` on its own (Docker Compose from this project's root):

```bash
docker compose up --build
```

The `docker-compose.yml` already injects all required variables. No changes to `config.h` are needed.

To customize, edit the `environment` block in `docker-compose.yml`:

```yaml
environment:
  - DB_HOST=postgres
  - DB_PORT=5432
  - DB_NAME=apidb
  - DB_USER=apiuser_test
  - DB_PASS=apipass_test
  - APP_PORT=8080
  - JWT_SECRET=your-secret-here
  - CORS_ALLOW_ORIGIN=http://localhost:4200
```

Or export them in your shell before running the binary directly:

```bash
export DB_HOST=localhost
export DB_PORT=5432
export DB_NAME=apidb
export DB_USER=apiuser_test
export DB_PASS=apipass_test
export APP_PORT=8080
export JWT_SECRET=your-secret-here
export CORS_ALLOW_ORIGIN=http://localhost:4200
./build/api
```

---

### Full stack mode (via crypto-dashboard)

When orchestrated from `crypto-dashboard`, all variables are injected by that project's `docker-compose.yml`. The values for `JWT_SECRET` and `CORS_ALLOW_ORIGIN` can be overridden by placing a `.env` file in the `crypto-dashboard/` root:

```env
# crypto-dashboard/.env
JWT_SECRET=your-production-secret
CORS_ALLOW_ORIGIN=http://localhost:4200
```

`DB_*` values are fixed to the shared internal PostgreSQL service and should not be changed unless you modify the full-stack compose as well.

---

## `config.local.h` — Compile-time local values

This file is intended for **native local development** (compiling and running without Docker). It uses hardcoded `#define` macros evaluated at compile time — no environment variables are read.

Unlike `config.example.h`, this file is meant to contain **your own local values** and should stay out of source control.

### When to use it

Use `config.local.h` when:
- You are building and running the binary natively (e.g. with `g++` or Visual Studio) and do not want to manage environment variables.
- You are connecting to a local PostgreSQL instance that is already running.

### How to use it

1. Start from the committed example:

   ```bash
   cp src/config/config.example.h src/config/config.local.h
   ```

2. Fill in your local values in `config.local.h`:

   ```cpp
   #pragma once

   #define DB_HOST "localhost"
   #define DB_PORT "5432"
   #define DB_NAME "your_db_name"
   #define DB_USER "your_pg_user"
   #define DB_PASS "your_pg_password"
   #define APP_PORT "8080"
   #define JWT_SECRET "your-secret"
   #define CORS_ALLOW_ORIGIN "http://localhost:4200"
   ```

3. Make sure your native build includes `config.local.h` as intended by your local workflow.

4. Rebuild the project.

> ⚠️ `config.local.h` is git-ignored. Never commit credentials to source control.

---

## `config.example.h` — Committed example

This file exists to show the expected format of a valid local config without treating those example values as real credentials for your machine.

### When to use it

Use `config.example.h` when:
- You need a starting point to create `config.local.h`.
- You want to understand which macros must exist for a native local build.

### Typical workflow

1. Copy `config.example.h` to `config.local.h`.
2. Replace the example values with your own local values.
3. Keep `config.local.h` uncommitted.

---

## Summary

| Scenario | File to use | How |
|---|---|---|
| Docker (standalone) | `config.h` | Set env vars in `docker-compose.yml` |
| Docker (full stack) | `config.h` | Managed by `crypto-dashboard/docker-compose.yml` |
| Native build (local) | `config.local.h` | Start from `config.example.h`, then fill your own local values |
