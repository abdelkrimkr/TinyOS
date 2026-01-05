#!/bin/bash
set -e

echo "Setting up build tools..."

# Download Busybox
if [ ! -f busybox ]; then
    echo "Downloading Busybox..."
    curl -L -o busybox https://busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox
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
