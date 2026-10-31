# RISC-V matmul accelerator for QEMU

Apply to the QEMU version listed in QEMU_VERSION.txt:

    git clone https://gitlab.com/qemu-project/qemu.git
    cd qemu && git checkout <tag from QEMU_VERSION.txt>
    git apply /path/to/qemu-matmul-accel.patch
    mkdir build && cd build
    ../configure --target-list=riscv64-softmmu
    make -j2

Test: build matmul_test.c with start.S and link.ld, run with
qemu-system-riscv64 -M virt -bios none -nographic -kernel matmul_test.elf
