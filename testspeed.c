// #include "include/butterknife.h"
#include "include/eden.h"
#include <stdio.h>
#include <x86intrin.h>

#define N 100000

static inline unsigned long long rdtsc()
{
    unsigned int aux;
    return __rdtscp(&aux);
}

int main(int argc, char *argv[])
{
    srand(1);
    unsigned long long start, end;
    uint8_t tweakey[32] = {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb,
        0xcc, 0xdd, 0xee, 0xff,
        0x10, 0x21, 0x32, 0x43,
        0x54, 0x65, 0x76, 0x87,
        0x98, 0xa9, 0xba, 0xcb,
        0xdc, 0xed, 0xfe, 0x0f};
    for (int i = 0; i < 32; i++)
    {
        tweakey[i] = rand() & 0xFF;
    }

    uint8_t output[16 * 8];
    uint8_t message[16] = {
        0x11, 0x22, 0x33, 0x10,
        0x55, 0x66, 0x77, 0x44,
        0x88, 0x99, 0xaa, 0xbb,
        0xcc, 0xdd, 0xee, 0xff};
    for (int i = 0; i < 16; i++)
    {
        message[i] = rand() & 0xFF;
    }
    const uint8_t num_branches;

    // warm up
    for (int i = 0; i < N; i++)
    {
        butterknife_256_encrypt(tweakey, output, message, 8);
    }

    double time = 0;
    // use the minimum cycles among 20 times
    for (int j = 0; j < 20; j++)
    {
        start = rdtsc();
        for (int i = 0; i < N; i++)
        {
            butterknife_256_encrypt(tweakey, output, message, 8);
        }
        end = rdtsc();
        if (j == 0 || end - start < time)
            time = end - start;
    }
    printf("%lf,", time / N);
    // for (int i = 0; i < 16 * 8; i++)
    //     printf("%d ", output[i]);
    // printf("\n");

    // warm up
    for (int i = 0; i < N; i++)
    {
        eden_256_encrypt(tweakey, output, message, (uint8_t)atoi(argv[1]), (uint8_t)atoi(argv[2]), (uint8_t)atoi(argv[3]), (uint8_t)atoi(argv[4]));
    }

    time = 0;
    // use the minimum cycles among 20 times
    for (int j = 0; j < 20; j++)
    {
        start = rdtsc();
        for (int i = 0; i < N; i++)
        {
            eden_256_encrypt(tweakey, output, message, (uint8_t)atoi(argv[1]), (uint8_t)atoi(argv[2]), (uint8_t)atoi(argv[3]), (uint8_t)atoi(argv[4]));
        }
        end = rdtsc();
        if (j == 0 || end - start < time)
            time = end - start;
    }
    printf("%lf\n", time / N);
    // for (int i = 0; i < 16 * 8; i++)
    //     printf("%d ", output[i]);
    // printf("\n");
    return 0;
}