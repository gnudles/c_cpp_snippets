#include <stdint.h>
//finds the modular multiplicative inverse of x over modulus of pow(2,bits)
uint64_t mod_mul_inverse_b2(uint64_t x, uint64_t bits, uint64_t *w)
{
        if (bits > 64 || bits == 0 || ((x & 1) == 0))
        {
                if (w != 0)
                        *w = 0;
                return 0;
        }
        uint64_t mask = ((uint64_t)-1) >> (64 - bits);
        x &= mask;
        uint64_t result = 1, state = x, ctz = 0;
        while (state != 1ULL)
        {
                ctz = __builtin_ctzll(state ^ 1);
                result |= 1ULL << ctz;
                state += x << ctz;
                state &= mask;
        }
        if (w != 0)
        {
                if (bits > 32)
                {
                        uint64_t qmul[4];
                        uint64_t carryin, carryout;
                        qmul[0] = (result & 0xFFFFFFFF) * (x & 0xFFFFFFFF);
                        qmul[1] = (result >> 32) * (x >> 32);
                        qmul[2] = (result & 0xFFFFFFFF) * (x >> 32);
                        qmul[3] = (result >> 32) * (x & 0xFFFFFFFF);
                        carryin = 0;
                        qmul[0] += __builtin_addcll(qmul[0], qmul[2] << 32, carryin, (unsigned long long *)&carryout);
                        carryin = carryout;
                        qmul[1] += __builtin_addcll(qmul[1], qmul[2] >> 32, carryin, (unsigned long long *)&carryout);
                        carryin = 0;
                        qmul[0] += __builtin_addcll(qmul[0], qmul[3] << 32, carryin, (unsigned long long *)&carryout);
                        carryin = carryout;
                        qmul[1] += __builtin_addcll(qmul[1], qmul[3] >> 32, carryin, (unsigned long long *)&carryout);
                        if (bits == 64)
                        {
                                *w = qmul[1];
                        }
                        else
                        {
                                *w = (qmul[0] >> bits) | (qmul[1] << (64 - bits));
                        }
                }
                else
                {
                        *w = (x * result) >> bits;
                }
        }
        return result;
}
