#!/bin/bash
set -e

# Defined checksums for supply chain security
BUSYBOX_SHA256="6e123e7f3202a8c1e9b1f94d8941580a25135382b99e8d3e34fb858bba311348"
LIMINE_SHA256="392b5cf019f8b829fea17d79b86fc88cbd2809a32a631c035241f61f708df817"

echo "Setting up build tools..."

if ! command -v sha256sum &> /dev/null; then
    echo "Error: 'sha256sum' command not found. Please install it to verify tool integrity."
    exit 1
fi

# Function to verify checksum
verify_checksum() {
    local file=$1
    local checksum=$2
    echo "$checksum  $file" | sha256sum -c - >/dev/null 2>&1
}

# Download Busybox
if [ -f busybox ]; then
    if ! verify_checksum busybox "$BUSYBOX_SHA256"; then
        echo "Busybox checksum mismatch. Re-downloading..."
        rm busybox
    else
        echo "Busybox checksum verified."
    fi
fi

if [ ! -f busybox ]; then
    echo "Downloading Busybox..."
    curl -L -o busybox https://busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox
    if ! verify_checksum busybox "$BUSYBOX_SHA256"; then
        echo "Error: Busybox checksum verification failed!"
        rm busybox
        exit 1
    fi
    chmod +x busybox
fi

# Check for mtools
if ! command -v mcopy &> /dev/null && [ ! -f mcopy ]; then
    echo "mcopy not found. Please install mtools (sudo apt install mtools) or compile it manually."
    echo "Downloading mtools source for manual compilation if needed..."
    # We don't auto-compile because it's heavy, but we can provide the tarball.
    # Actually, let's just warn.
    echo "WARNING: 'mcopy' (mtools) is required for 'make image'."
fi

# Download Limine
if [ ! -d limine ]; then
    echo "Downloading Limine..."
    curl -L -o limine.tar.gz https://github.com/limine-bootloader/limine/archive/refs/tags/v7.0.0-binary.tar.gz

    if ! verify_checksum limine.tar.gz "$LIMINE_SHA256"; then
        echo "Error: Limine checksum verification failed!"
        rm limine.tar.gz
        exit 1
    fi

    tar -xf limine.tar.gz
    mv limine-7.0.0-binary limine
    rm limine.tar.gz

    # Build limine tool if needed (the binary release usually contains the executable 'limine')
    # But in v7.0.0 binary release, the executable is 'limine' (Linux) or 'limine.exe' (Windows).
    # We might need to chmod it.
    if [ -f limine/limine ]; then
        chmod +x limine/limine
    else
        echo "Limine binary not found in expected path. Attempting build..."
        cd limine && make && cd ..
    fi
fi

echo "Tools setup complete. Run 'make image' to build."
