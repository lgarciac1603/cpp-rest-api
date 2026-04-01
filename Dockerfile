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

# Copy entrypoint script
COPY docker-entrypoint.sh /usr/local/bin/docker-entrypoint.sh
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

# Expose port
EXPOSE 8080

# Start with entrypoint script
ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
CMD ["/app/build/api"]