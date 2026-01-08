#!/bin/bash
set -e

# Sentinel 🛡️: Supply Chain Security
# Verifying checksums prevents the use of compromised binaries if the
# download server or connection is intercepted (MITM).

# Checksums for external dependencies
BUSYBOX_SHA256="6e123e7f3202a8c1e9b1f94d8941580a25135382b99e8d3e34fb858bba311348"
LIMINE_SHA256="392b5cf019f8b829fea17d79b86fc88cbd2809a32a631c035241f61f708df817"

# Helper function to verify checksum
verify_checksum() {
    local file=$1
    local expected_checksum=$2

    if ! echo "$expected_checksum  $file" | sha256sum -c - > /dev/null 2>&1; then
        echo "❌ SECURITY ERROR: Checksum verification failed for $file!"
        echo "Expected: $expected_checksum"
        echo "Computed: $(sha256sum $file | awk '{print $1}')"
        echo "Deleting corrupted file..."
        rm -f "$file"
        exit 1
    else
        echo "✅ Checksum verified for $file"
    fi
}

echo "Setting up build tools..."

# Download Busybox
if [ ! -f busybox ]; then
    echo "Downloading Busybox..."
    curl -L -o busybox https://busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox
    verify_checksum "busybox" "$BUSYBOX_SHA256"
    chmod +x busybox
else
    # Sentinel: Verify existing file too
    verify_checksum "busybox" "$BUSYBOX_SHA256"
fi

# Check for mtools
if ! command -v mcopy &> /dev/null && [ ! -f mcopy ]; then
    echo "mcopy not found. Please install mtools (sudo apt install mtools) or compile it manually."
    echo "WARNING: 'mcopy' (mtools) is required for 'make image'."
fi

# Download Limine
if [ ! -d limine ]; then
    echo "Downloading Limine..."
    curl -L -o limine.tar.gz https://github.com/limine-bootloader/limine/archive/refs/tags/v7.0.0-binary.tar.gz

    verify_checksum "limine.tar.gz" "$LIMINE_SHA256"

    tar -xf limine.tar.gz
    mv limine-7.0.0-binary limine
    rm limine.tar.gz

    # Build limine tool if needed (the binary release usually contains the executable 'limine')
    if [ -f limine/limine ]; then
        chmod +x limine/limine
    else
        echo "Limine binary not found in expected path. Attempting build..."
        cd limine && make && cd ..
    fi
fi

echo "Tools setup complete. Run 'make image' to build."
