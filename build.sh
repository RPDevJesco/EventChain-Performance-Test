docker create --name eventchains-temp eventchains-builder#!/bin/bash
# EventChains Cross-Platform Build Script
# Builds for Linux and Windows using Docker, then copies binaries back to project

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
IMAGE_NAME="eventchains-builder"
CONTAINER_NAME="eventchains-build-temp"
OUTPUT_DIR="./build-output"

echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                                                            ║${NC}"
echo -e "${BLUE}║        EventChains Cross-Platform Build System             ║${NC}"
echo -e "${BLUE}║           Linux (native) + Windows (MinGW)                 ║${NC}"
echo -e "${BLUE}║                                                            ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Check if Docker is available
if ! command -v docker &> /dev/null; then
    echo -e "${RED}✗ Error: Docker is not installed or not in PATH${NC}"
    echo "  Please install Docker from https://www.docker.com/"
    exit 1
fi

echo -e "${GREEN}✓${NC} Docker is available"

# Clean previous build artifacts
echo ""
echo -e "${YELLOW}→${NC} Cleaning previous build artifacts..."
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"
echo -e "${GREEN}✓${NC} Clean complete"

# Build Docker image
echo ""
echo -e "${YELLOW}→${NC} Building Docker image: ${IMAGE_NAME}"
echo "  This includes:"
echo "    • Ubuntu 22.04 base"
echo "    • GCC for Linux builds"
echo "    • MinGW-w64 for Windows cross-compilation"
echo "    • CMake build system"
echo ""

if docker build -t "$IMAGE_NAME" .; then
    echo -e "${GREEN}✓${NC} Docker image built successfully"
else
    echo -e "${RED}✗ Error: Docker build failed${NC}"
    exit 1
fi

# Create container to extract binaries
echo ""
echo -e "${YELLOW}→${NC} Creating temporary container to extract binaries..."

# Remove old container if it exists
docker rm "$CONTAINER_NAME" 2>/dev/null || true

# Create container (don't run it, just create)
docker create --name "$CONTAINER_NAME" "$IMAGE_NAME" > /dev/null

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} Container created"
else
    echo -e "${RED}✗ Error: Failed to create container${NC}"
    exit 1
fi

# Copy binaries from container to host
echo ""
echo -e "${YELLOW}→${NC} Extracting compiled binaries..."

if docker cp "$CONTAINER_NAME:/binaries/." "$OUTPUT_DIR/"; then
    echo -e "${GREEN}✓${NC} Binaries extracted to: $OUTPUT_DIR"
else
    echo -e "${RED}✗ Error: Failed to extract binaries${NC}"
    docker rm "$CONTAINER_NAME"
    exit 1
fi

# Clean up container
docker rm "$CONTAINER_NAME" > /dev/null
echo -e "${GREEN}✓${NC} Temporary container removed"

# Display build artifacts
echo ""
echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                    Build Complete!                         ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo "Build artifacts are available in: $OUTPUT_DIR"
echo ""

if [ -f "$OUTPUT_DIR/linux/eventchain_test" ]; then
    SIZE=$(du -h "$OUTPUT_DIR/linux/eventchain_test" | cut -f1)
    echo -e "  ${GREEN}✓${NC} Linux binary:   $OUTPUT_DIR/linux/eventchain_test ($SIZE)"
else
    echo -e "  ${RED}✗${NC} Linux binary:   NOT FOUND"
fi

if [ -f "$OUTPUT_DIR/windows/eventchain_test.exe" ]; then
    SIZE=$(du -h "$OUTPUT_DIR/windows/eventchain_test.exe" | cut -f1)
    echo -e "  ${GREEN}✓${NC} Windows binary: $OUTPUT_DIR/windows/eventchain_test.exe ($SIZE)"
else
    echo -e "  ${RED}✗${NC} Windows binary: NOT FOUND"
fi

if [ -f "$OUTPUT_DIR/README.md" ]; then
    echo -e "  ${GREEN}✓${NC} Documentation:  $OUTPUT_DIR/README.md"
fi

if [ -f "$OUTPUT_DIR/eventchains.h" ]; then
    echo -e "  ${GREEN}✓${NC} Header file:    $OUTPUT_DIR/eventchains.h"
fi

# Test Linux binary (if on Linux)
if [ "$(uname)" = "Linux" ] && [ -f "$OUTPUT_DIR/linux/eventchain_test" ]; then
    echo ""
    echo -e "${YELLOW}→${NC} Testing Linux binary..."
    
    chmod +x "$OUTPUT_DIR/linux/eventchain_test"
    
    if "$OUTPUT_DIR/linux/eventchain_test" 1000 > /dev/null 2>&1; then
        echo -e "${GREEN}✓${NC} Linux binary test: PASSED"
    else
        echo -e "${YELLOW}⚠${NC} Linux binary test: WARNING (may need dependencies)"
    fi
fi

echo ""
echo -e "${BLUE}════════════════════════════════════════════════════════════${NC}"
echo ""
echo "Usage:"
echo "  Linux:   ./$OUTPUT_DIR/linux/eventchain_test [iterations]"
echo "  Windows: .\\$OUTPUT_DIR\\windows\\eventchain_test.exe [iterations]"
echo ""
echo "Options:"
echo "  [iterations]  Number of benchmark iterations (default: 10000)"
echo ""
echo -e "${GREEN}Build process complete!${NC}"
