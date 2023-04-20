#include <iostream>
#include <string>
#include <cstring>

#define N 256 // 2 ^ 8

#ifndef RC4_T
#define RC4_T

void swap(unsigned char *a, unsigned char *b) {
    int tmp = *a;
    *b = *a;
    *a = tmp;
}

void KSA(const char *key, unsigned char *S) {
    int j = 0, len = strlen(key);

    for (int i = 0; i < N; i++)
        S[i] = i;

    for (int i = 0; i < N; i++) {
        j = (j + S[i] + key[i % len]) % N;

        swap(&S[i], &S[j]);
    }
}

void PRGA(unsigned char *S, const char *plaintext, unsigned char *ciphertext) {
    int i = 0;
    int j = 0;

    for (size_t n = 0, len = strlen(plaintext); n < len; n++) {
        i = (i+1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        ciphertext[n] = rnd ^ plaintext[n];
    }
}

unsigned char *RC4(const char *key, const char *plaintext) {
    unsigned char *ciphertext = new unsigned char[4096];
    unsigned char S[N];

    KSA(key, S);
    PRGA(S, plaintext, ciphertext);

    return ciphertext;
}

#endif
