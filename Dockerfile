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
    libssl-dev \
    libcrypto++-dev \
    wget \
    unzip \
    && rm -rf /var/lib/apt/lists/*

# Download and install jwt-cpp headers
RUN mkdir -p /usr/local/include/jwt-cpp && \
    wget -q https://github.com/Thalhammer/jwt-cpp/releases/latest/download/jwt-cpp-0.7.2.zip -O /tmp/jwt-cpp.zip && \
    unzip /tmp/jwt-cpp.zip -d /tmp && \
    cp -r /tmp/jwt-cpp-0.7.2/include/jwt-cpp/* /usr/local/include/jwt-cpp/ && \
    rm -rf /tmp/jwt-cpp*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Create build directory
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Expose port
EXPOSE 8080

# Run the app (assuming DB is connected via docker-compose)
CMD ["./build/api"]