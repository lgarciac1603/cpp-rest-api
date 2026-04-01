#!/bin/sh
set -e

# Wait for Postgres to be ready
if [ -z "$DB_HOST" ] || [ -z "$DB_PORT" ] || [ -z "$DB_USER" ] || [ -z "$DB_NAME" ]; then
  echo "Missing DB env vars, ensure DB_HOST, DB_PORT, DB_USER, DB_NAME are set."
  exit 1
fi

echo "Waiting for PostgreSQL to be available..."
until pg_isready -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" > /dev/null 2>&1; do
  sleep 1
  echo "."
done

echo "PostgreSQL is ready. Running migrations..."
for sql in /app/database/migrations/*.sql; do
  echo "Applying migration: ${sql}"
  PGPASSWORD="$DB_PASS" psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -f "$sql"
done

echo "Migrations applied. Starting API."
exec "$@"