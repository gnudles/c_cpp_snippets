#include <stdint.h>
//finds the modular multiplicative inverse of x over modulus of pow(2,bits)
uint64_t find_mod_mul_inverse(uint64_t x, uint64_t bits)
{
        if (bits > 64 || ((x&1)==0))
                return 0;
        uint64_t mask;
        if (bits == 64)
                mask = -1;
        else
        {                
                mask = 1;
                mask<<=bits;
                mask--;
        }
        x&=mask;
        uint64_t result=1, state=x, ctz=0;
        while(state!=1ULL)
        {
                ctz=__builtin_ctzll(state^1);
                result|=1ULL<<ctz;
                state+=x<<ctz;
                state&=mask;
        }
        return result;
}
