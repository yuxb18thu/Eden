#ifndef ED_H
#define ED_H

#include "butterknife.h"
#include <stdint.h>
#include <string.h>

// #define BUTTERKNIFE_ROOT_ROUNDS 7
// #define BUTTERKNIFE_BRANCH_ROUNDS 8
// #define BUTTERKNIFE_ROUNDS (BUTTERKNIFE_ROOT_ROUNDS + BUTTERKNIFE_BRANCH_ROUNDS)
// #define BUTTERKNIFE_MAX_BRANCHES 8

/* Tweakey Schedule */
void eden_256_precompute_rtk(const uint8_t tweakey[32], uint32_t *rtk, const uint8_t num_branches_root, const uint8_t num_branches_level1, const uint8_t num_branches_level2, const uint8_t num_branches_level3);

/* Encryption */

void eden_256_encrypt_w_rtk(const uint32_t rtk[4 * 16], uint8_t *output, const uint8_t message[16], const uint8_t num_branches_root, const uint8_t num_branches_level1, const uint8_t num_branches_level2, const uint8_t num_branches_level3);
void eden_256_encrypt(const uint8_t tweakey[32], uint8_t *output, const uint8_t message[16], const uint8_t num_branches_root, const uint8_t num_branches_level1, const uint8_t num_branches_level2, const uint8_t num_branches_level3);

#endif /* ED_H */
