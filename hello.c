void _start(void) {
    volatile char *uart = (volatile char *)0x10000000;
    const char *s = "hello riscv\n";
    while (*s) *uart = *s++;
    while (1);
}
