FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git build-essential ninja-build pkg-config python3 python3-venv \
    libglib2.0-dev libpixman-1-dev libslirp-dev flex bison \
    gcc-riscv64-linux-gnu \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
RUN git clone --depth 1 --branch v9.2.0 https://gitlab.com/qemu-project/qemu.git

WORKDIR /src/qemu
RUN ./configure --target-list=riscv64-softmmu \
    && make -j$(nproc) \
    && make install

WORKDIR /work