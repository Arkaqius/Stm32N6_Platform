FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y \
        cmake \
        make \
        ninja-build \
        gcc-arm-none-eabi \
        gdb-multiarch \
        git \
    && rm -rf /var/lib/apt/lists/*

# Optional: add other tools if needed

