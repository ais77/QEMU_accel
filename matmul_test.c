/* matmul_test.c - bare-metal test for the matmul-accel device */

#define UART   ((volatile unsigned char *)0x10000000UL)
#define ACCEL  0x10200000UL
#define REG(off) (*(volatile unsigned int *)(ACCEL + (off)))

/* Register offsets (must match matmul-accel.c) */
#define A_ADDR 0x00
#define B_ADDR 0x04
#define C_ADDR 0x08
#define DIM    0x0c
#define CTRL   0x10
#define STATUS 0x14
#define CYCLES 0x18

#define ST_DONE  0x2
#define ST_ERROR 0x4

#define N 16   /* must be 1..64 */

static int A[N][N], B[N][N], C_hw[N][N], C_sw[N][N];

static void putc_(char c) { *UART = c; }

static void puts_(const char *s) { while (*s) putc_(*s++); }

static void putint(int v)
{
    char buf[12];
    int i = 0;
    unsigned u;

    if (v < 0) { putc_('-'); u = -(unsigned)v; } else { u = v; }
    if (u == 0) { putc_('0'); return; }
    while (u) { buf[i++] = '0' + u % 10; u /= 10; }
    while (i) putc_(buf[--i]);
}

static void print_corner(const char *name, int M[N][N])
{
    puts_(name);
    puts_(" (top-left 4x4):\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            putint(M[i][j]);
            putc_('\t');
        }
        putc_('\n');
    }
}

int main(void)
{
    /* A[i][j] = i + j,  B[i][j] = i - j */
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }

    /* Software reference */
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++)
                sum += A[i][k] * B[k][j];
            C_sw[i][j] = sum;
        }

    /* Program the accelerator */
    REG(A_ADDR) = (unsigned)(unsigned long)A;
    REG(B_ADDR) = (unsigned)(unsigned long)B;
    REG(C_ADDR) = (unsigned)(unsigned long)C_hw;
    REG(DIM)    = N;
    REG(CTRL)   = 1;                          /* start */

    while (!(REG(STATUS) & (ST_DONE | ST_ERROR)))
        ;                                      /* poll */

    if (REG(STATUS) & ST_ERROR) {
        puts_("accelerator error\n");
        for (;;) ;
    }

    /* Compare */
    int mismatches = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (C_hw[i][j] != C_sw[i][j])
                mismatches++;

    print_corner("C_sw", C_sw);
    print_corner("C_hw", C_hw);

    puts_("modeled accelerator cycles = ");
    putint((int)REG(CYCLES));
    putc_('\n');

    if (mismatches == 0) {
        puts_("PASS\n");
    } else {
        puts_("FAIL, mismatches = ");
        putint(mismatches);
        putc_('\n');
    }

    for (;;) ;
}