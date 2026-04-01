# Use Ubuntu as base image with C++ compiler
FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libpq-dev \
    libpq5 \
    postgresql-client \
    libssl-dev \
    wget \
    unzip \
    && rm -rf /var/lib/apt/lists/*

# Download and install jwt-cpp headers
RUN git clone --depth 1 --branch v0.7.0 https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp && \
    cp -r /tmp/jwt-cpp/include/jwt-cpp /usr/local/include/ && \
    rm -rf /tmp/jwt-cpp

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Create build directory and compile
RUN rm -rf build && mkdir build && cd build && \
    cmake .. && \
    make

# Create entrypoint script
RUN mkdir -p /usr/local/bin && \
    echo '#!/bin/sh' > /usr/local/bin/docker-entrypoint.sh && \
    echo 'set -e' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'echo "DB_HOST: $DB_HOST"' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'echo "DB_PORT: $DB_PORT"' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'echo "DB_USER: $DB_USER"' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'echo "DB_PASS: $DB_PASS"' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'echo "DB_NAME: $DB_NAME"' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'if [ -z "$DB_HOST" ] || [ -z "$DB_PORT" ] || [ -z "$DB_USER" ] || [ -z "$DB_NAME" ]; then' >> /usr/local/bin/docker-entrypoint.sh && \
    echo '  echo "Missing DB env vars"' >> /usr/local/bin/docker-entrypoint.sh && \
    echo '  exit 1' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'fi' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'echo "Waiting for PostgreSQL..."' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'until pg_isready -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" > /dev/null 2>&1; do sleep 1; echo "."; done' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'echo "PostgreSQL ready. Running migrations..."' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'for sql in /app/database/migrations/*.sql; do echo "Applying: $sql"; PGPASSWORD="$DB_PASS" psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -f "$sql"; done' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'echo "Migrations done. Starting API."' >> /usr/local/bin/docker-entrypoint.sh && \
    echo 'exec "$@"' >> /usr/local/bin/docker-entrypoint.sh && \
    chmod +x /usr/local/bin/docker-entrypoint.sh

# Expose port
EXPOSE 8080

# Start with entrypoint script
ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
CMD ["/app/build/api"]