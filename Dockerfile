# Multi-stage Dockerfile for EventChains cross-platform builds
# Builds for both Linux (native) and Windows (MinGW cross-compile)

FROM ubuntu:22.04 as builder

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and cross-compilation toolchain
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gcc \
    g++ \
    mingw-w64 \
    git \
    file \
    && rm -rf /var/lib/apt/lists/*

# Create build directory
WORKDIR /build

# Copy toolchain file first
COPY toolchain-mingw64.cmake .

# Copy source files
COPY CMakeLists.txt .
COPY eventchains.c .
COPY eventchains.h .
COPY multi_tier_benchmark.c .
COPY dijkstra_benchmark.c .
COPY test_main.c .
COPY README.md .

# Verify all files are present
RUN echo "=== Verifying source files ===" && \
    ls -lh && \
    echo "" && \
    echo "=== Checking CMakeLists.txt ===" && \
    cat CMakeLists.txt && \
    echo "" && \
    echo "=== File sizes ===" && \
    wc -l *.c *.h

# Build for Linux (native)
RUN echo "=== Building for Linux (native) ===" && \
    mkdir -p build-linux && \
    cd build-linux && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-lm" .. && \
    make VERBOSE=1 && \
    ls -lh && \
    file eventchain_test_optimized && \
    strip eventchain_test_optimized && \
    ./eventchain_test_optimized 100

# Build for Windows (cross-compile with MinGW)
RUN echo "=== Building for Windows (MinGW cross-compile) ===" && \
    mkdir -p build-windows && \
    cd build-windows && \
    cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw64.cmake \
          -DCMAKE_BUILD_TYPE=Release .. && \
    make VERBOSE=1 && \
    ls -lh && \
    file eventchain_test_optimized.exe && \
    x86_64-w64-mingw32-strip eventchain_test_optimized.exe

# Create output directory with organized structure
RUN echo "=== Organizing output files ===" && \
    mkdir -p /output/linux /output/windows && \
    cp build-linux/eventchain_test_optimized /output/linux/ && \
    cp build-windows/eventchain_test_optimized.exe /output/windows/ && \
    cp README.md /output/ && \
    cp eventchains.h /output/ && \
    echo "" && \
    echo "=== Final binaries ===" && \
    ls -lh /output/linux/ && \
    ls -lh /output/windows/ && \
    file /output/linux/eventchain_test_optimized && \
    file /output/windows/eventchain_test_optimized.exe

# Create a minimal runtime image for testing
FROM ubuntu:22.04 as runtime

COPY --from=builder /output /binaries

WORKDIR /binaries

CMD ["/binaries/linux/eventchain_test", "1000"]