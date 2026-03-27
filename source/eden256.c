#include "../include/butterknife.h"
#include "../include/eden.h"

static const uint8_t jb[15] = {0, 100, 200, 110, 120, 210, 220, 111, 112, 121, 122, 211, 212, 221, 222};

static const uint8_t perm[16] = {1, 6, 11, 12, 5, 10, 15, 0, 9, 14, 3, 4, 13, 2, 7, 8};

static const uint8_t rcon[40] = {
    0x2f,
    0x5e,
    0xbc,
    0x63,
    0xc6,
    0x97,
    0x35,
    0x6a,
    0xd4,
    0xb3,
    0x7d,
    0xfa,
    0xef,
    0xc5,
    0x91,
    0x39,
    0x72,
    0xe4,
    0xd3,
    0xbd,
    0x61,
    0xc2,
    0x9f,
    0x25,
    0x4a,
    0x94,
    0x33,
    0x66,
    0xcc,
    0x83,
    0x1d,
    0x3a,
    0x74,
    0xe8,
    0xcb,
    0x8d,
    0x01,
    0x02,
    0x04,
    0x08,
};

void eden_256_precompute_rtk(const uint8_t tweakey[32], uint32_t *rtk, const uint8_t num_branches_root, const uint8_t num_branches_level1, const uint8_t num_branches_level2, const uint8_t num_branches_level3)
{
    uint8_t r;
    uint8_t tk[2][16];
    uint8_t Nr = num_branches_root + num_branches_level1 + num_branches_level2 + num_branches_level3 - 1;
    uint32_t rcon_row1 = 0x01020408;

    memcpy(tk[0], tweakey + 0, 16);
    memcpy(tk[1], tweakey + 16, 16);

    for (r = 0; r <= Nr; ++r)
    {
        /* Produce the round tk */
        rtk[4 * r + 0] = GETU32(tk[0] + 0) ^ GETU32(tk[1] + 0) ^ rcon_row1;
        rtk[4 * r + 1] = GETU32(tk[0] + 4) ^ GETU32(tk[1] + 4) ^ GETRCON(r);
        rtk[4 * r + 2] = GETU32(tk[0] + 8) ^ GETU32(tk[1] + 8);
        rtk[4 * r + 3] = GETU32(tk[0] + 12) ^ GETU32(tk[1] + 12);

        /* Apply H and G functions */
        H(tk[0]);
        G(tk[0], 2);

        H(tk[1]);
        G(tk[1], 1);
    }
}

void eden_256_encrypt_w_rtk(const uint32_t rtk[4 * 16], uint8_t *output, const uint8_t message[16], const uint8_t num_branches_root, const uint8_t num_branches_level1, const uint8_t num_branches_level2, const uint8_t num_branches_level3)
{
    int jb_cnt = 0;
    uint8_t branch, round;
    uint32_t state[8][4], forkstate[4];

    state[0][0] = GETU32(message);
    state[0][1] = GETU32(message + 4);
    state[0][2] = GETU32(message + 8);
    state[0][3] = GETU32(message + 12);

    for (round = 0; round < num_branches_root; ++round)
    {
        AddRoundKey(state[0], rtk + round * 4, jb[jb_cnt]);
        SubBytes(state[0]);
        ShiftRows(state[0]);
        MixColumns(state[0]);
    }

    memcpy(state[4], state[0], sizeof(state[0]));
    for (branch = 0; branch < 2; ++branch)
    {
        jb_cnt++;
        for (round = 0; round < num_branches_level1; ++round)
        {
            AddRoundKey(state[4 * branch], rtk + (round + num_branches_root) * 4, jb[jb_cnt]);
            SubBytes(state[4 * branch]);
            ShiftRows(state[4 * branch]);
            MixColumns(state[4 * branch]);
        }
    }

    memcpy(state[2], state[0], sizeof(state[0]));
    memcpy(state[6], state[4], sizeof(state[0]));
    for (branch = 0; branch < 4; ++branch)
    {
        jb_cnt++;
        for (round = 0; round < num_branches_level2; ++round)
        {
            AddRoundKey(state[2 * branch], rtk + (round + num_branches_root + num_branches_level1) * 4, jb[jb_cnt]);
            SubBytes(state[2 * branch]);
            ShiftRows(state[2 * branch]);
            MixColumns(state[2 * branch]);
        }
    }

    memcpy(state[1], state[0], sizeof(state[0]));
    memcpy(state[3], state[2], sizeof(state[0]));
    memcpy(state[5], state[4], sizeof(state[0]));
    memcpy(state[7], state[6], sizeof(state[0]));
    for (branch = 0; branch < 8; ++branch)
    {
        jb_cnt++;
        for (round = 0; round < num_branches_level3; ++round)
        {
            AddRoundKey(state[branch], rtk + (round + num_branches_root + num_branches_level1 + num_branches_level2) * 4, jb[jb_cnt]);
            SubBytes(state[branch]);
            ShiftRows(state[branch]);
            MixColumns(state[branch]);
        }
        AddRoundKey(state[branch], rtk + (num_branches_root + num_branches_level1 + num_branches_level2 + num_branches_level3) * 4, jb[jb_cnt]);

        /* Put the state into the ciphertext */
        PUTU32(output + 16 * branch, state[branch][0]);
        PUTU32(output + 16 * branch + 4, state[branch][1]);
        PUTU32(output + 16 * branch + 8, state[branch][2]);
        PUTU32(output + 16 * branch + 12, state[branch][3]);
    }
}

void eden_256_encrypt(const uint8_t tweakey[32], uint8_t *output, const uint8_t message[16], const uint8_t num_branches_root, const uint8_t num_branches_level1, const uint8_t num_branches_level2, const uint8_t num_branches_level3)
{
    uint32_t rtk[4 * (num_branches_root + num_branches_level1 + num_branches_level2 + num_branches_level3 + 1)];

    eden_256_precompute_rtk(tweakey, rtk, num_branches_root, num_branches_level1, num_branches_level2, num_branches_level3);
    eden_256_encrypt_w_rtk(rtk, output, message, num_branches_root, num_branches_level1, num_branches_level2, num_branches_level3);
}
