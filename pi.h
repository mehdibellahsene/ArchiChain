#ifndef PI_H
#define PI_H

#include <stdbool.h>
#include <stdint.h>

// Enhanced Pi calculation functions
void get_pi_digits(char *buffer, int digits);
void get_pi_digits_from_previous(char *buffer, int digits, const void *prev_block);

// High-precision Pi calculation using different algorithms
void calculate_pi_spigot(char *buffer, int digits);
void calculate_pi_chudnovsky(char *buffer, int digits);
void calculate_pi_machin(char *buffer, int digits);

// Pi digit verification
bool verify_pi_digits(const char *digits, int count, int starting_position);
bool is_valid_pi_sequence(const char *digits, int count);

// Proof of work specific Pi functions
uint32_t calculate_pi_proof_hash(const char *pi_digits, int count, uint32_t nonce);
bool verify_pi_proof_of_work(const char *pi_digits, int count, uint32_t difficulty);

#endif
