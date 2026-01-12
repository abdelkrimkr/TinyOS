#!/bin/bash
set -e

# Sentinel: Check for sha256sum availability
if ! command -v sha256sum &> /dev/null; then
    echo "Error: sha256sum command not found. Please install it to proceed."
    exit 1
fi

echo "Setting up build tools..."

# Checksums
BUSYBOX_SHA256="6e123e7f3202a8c1e9b1f94d8941580a25135382b99e8d3e34fb858bba311348"
LIMINE_SHA256="392b5cf019f8b829fea17d79b86fc88cbd2809a32a631c035241f61f708df817"

verify_checksum() {
    local file=$1
    local expected=$2
    local actual=$(sha256sum "$file" | awk '{print $1}')

    if [ "$actual" != "$expected" ]; then
        echo "ERROR: Checksum verification failed for $file!"
        echo "Expected: $expected"
        echo "Actual:   $actual"
        rm -f "$file"
        exit 1
    else
        echo "Checksum verified for $file"
    fi
}

# Download Busybox
if [ ! -f busybox ]; then
    echo "Downloading Busybox..."
    curl -L -o busybox https://busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox
    verify_checksum busybox "$BUSYBOX_SHA256"
    chmod +x busybox
fi

# Check for mtools
if ! command -v mcopy &> /dev/null && [ ! -f mcopy ]; then
    echo "mcopy not found. Please install mtools (sudo apt install mtools) or compile it manually."
    echo "Downloading mtools source for manual compilation if needed..."
    echo "WARNING: 'mcopy' (mtools) is required for 'make image'."
fi

# Download Limine
if [ ! -d limine ]; then
    echo "Downloading Limine..."
    curl -L -o limine.tar.gz https://github.com/limine-bootloader/limine/archive/refs/tags/v7.0.0-binary.tar.gz
    verify_checksum limine.tar.gz "$LIMINE_SHA256"
    tar -xf limine.tar.gz
    mv limine-7.0.0-binary limine
    rm limine.tar.gz

    if [ -f limine/limine ]; then
        chmod +x limine/limine
    else
        echo "Limine binary not found in expected path. Attempting build..."
        cd limine && make && cd ..
    fi
fi

echo "Tools setup complete. Run 'make image' to build."
