# Multi-stage Dockerfile for Browser Backtracking Simulator
# Stage 1: Build the C++ Backend with g++
FROM gcc:latest AS builder

WORKDIR /app

# Copy C++ source files
COPY browser.h browser.cpp server.cpp ./

# Statically link libstdc++ and libgcc to avoid glibcxx version mismatches in minimal runtime image
RUN g++ -O3 -Wall -Wextra -static-libgcc -static-libstdc++ server.cpp browser.cpp -o server

# Stage 2: Minimal runtime image
FROM debian:bookworm-slim

WORKDIR /app

# Copy compiled binary from builder
COPY --from=builder /app/server /app/server

# Copy static frontend assets
COPY public /app/public

# Default port 8080 (Cloud platforms like Render/Railway override via $PORT)
ENV PORT=8080
EXPOSE 8080

# Run the C++ HTTP server
CMD ["/app/server"]
