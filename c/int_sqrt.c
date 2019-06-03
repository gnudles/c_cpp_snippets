#include <stdint.h>

#define BITSPERLONG 64

#define TOP2BITS(x) ((x & (3ULL << (BITSPERLONG-2))) >> (BITSPERLONG-2))

/* usqrtp64:
    ENTRY x: unsigned long long
    EXIT  returns floor(sqrt(x) * pow(2, BITSPERLONG/2))

    Since the square root never uses more than half the bits
    of the input, we use the other half of the bits to contain
    extra bits of precision after the binary point.

    EXAMPLE
        suppose BITSPERLONG = 64
        then    usqrt(144) = 51539607552 = 12 * (1 << 32)
                usqrt(32) = 24296003999 = 5.66 * (1 << 32)

    NOTES
        (1) change BITSPERLONG to BITSPERLONG/2 if you do not want
            the answer scaled.  Indeed, if you want n bits of
            precision after the binary point, use BITSPERLONG/2+n.
            The code assumes that BITSPERLONG is even.
        (2) This is really better off being written in assembly.
            The line marked below is really a "arithmetic shift left"
            on the double-long value with r in the upper half
            and x in the lower half.  This operation is typically
            expressible in only one or two assembly instructions.
        (3) Unrolling this loop is probably not a bad idea.

    ALGORITHM
        The calculations are the base-two analogue of the square
        root algorithm we all learned in grammar school.  Since we're
        in base 2, there is only one nontrivial trial multiplier.

        Notice that absolutely no multiplications or divisions are performed.
        This means it'll be fast on a wide range of processors.
*/

uint64_t super_usqrtp64(uint64_t x, uint64_t *pbits)
{
      uint64_t a = 0L;    /* accumulator      */
      uint64_t r = 0L;    /* remainder        */
      uint64_t e = 0L;    /* trial product    */

      int i;
      i=__builtin_clzll(x)>>1;
      *pbits=i+32;
      x<<=(i<<1);
#pragma clang loop unroll(full)
      for (i=0; i < BITSPERLONG; i++)   /* NOTE 1 */
      {
            r = (r << 2) + TOP2BITS(x); x <<= 2; /* NOTE 2 */
            a <<= 1;
            e = (a << 1) + 1;
            if (r >= e)
            {
                  r -= e;
                  a++;
            }
      }
      return a;
}
uint64_t usqrtp64(uint64_t x)
{
      uint64_t a = 0L;    /* accumulator      */
      uint64_t r = 0L;    /* remainder        */
      uint64_t e = 0L;    /* trial product    */

      int i;
      i=__builtin_clzll(x)>>1;
      x<<=(i<<1);
      for (; i < BITSPERLONG; i++)   /* NOTE 1 */
      {
            r = (r << 2) + TOP2BITS(x); x <<= 2; /* NOTE 2 */
            a <<= 1;
            e = (a << 1) + 1;
            if (r >= e)
            {
                  r -= e;
                  a++;
            }
      }
      return a;
}

uint64_t usqrt64(uint64_t x)
{
      uint64_t a = 0L;    /* accumulator      */
      uint64_t r = 0L;    /* remainder        */
      uint64_t e = 0L;    /* trial product    */

      int i;
      i=__builtin_clzll(x)>>1;
      x<<=(i<<1);
      for (; i < BITSPERLONG/2 ; i++)   /* NOTE 1 */
      {
            r = (r << 2) + TOP2BITS(x); x <<= 2; /* NOTE 2 */
            a <<= 1;
            e = (a << 1) + 1;
            if (r >= e)
            {
                  r -= e;
                  a++;
            }
      }
      return a;
}
const char* part2string(uint64_t p, uint64_t bits, char* buf, int buflen)
{
	p=(p<<(64-bits));
	uint64_t msb,lsb;
	msb=p>>32;
	lsb=p&0xffffffff;
	int i=0;
	while((lsb!=0 || msb!=0 || i==0) && (i+1)<buflen)
	{
	lsb+=lsb<<2;
	lsb<<=1;
	msb+=msb<<2;
	msb<<=1;
	msb+=lsb>>32;
	lsb&=0xffffffff;
	buf[i]='0'+(msb>>32);
	++i;
	msb&=0xffffffff;
	}
	buf[i]=0;
	return buf;
}

#ifdef TEST
#include <stdio.h>
int main()
{
	char buf[128];
	uint64_t pbits;
	uint64_t res;
	printf("sqrt(144)=%llu\n",usqrt64(144));
	printf("precise sqrt(144)=%llu\n",usqrtp64(144));
	printf("sqrt(35)=%llu\n",usqrt64(35));
	res=usqrtp64(35);
	printf("precise sqrt(35)=%llu (32 bits after dot) value = %llu + %llu/%llu\n",res,res>>32,res&0xffffffff,1ULL<<32);
	res = super_usqrtp64(35,&pbits);
	printf("super precise sqrt(35)=%016llx bits after point:%llu, value= %llu + %llu/%llu %llu.%s\n",res,pbits,res>>pbits, res & (~((-1ULL)<<pbits)),1ULL<<pbits,res>>pbits,part2string(res,pbits,buf,128));
	res = super_usqrtp64(2,&pbits);
	printf("super precise sqrt(2)=%016llx bits after point:%llu, value= %llu + %llu/%llu %llu.%s\n",res,pbits,res>>pbits, res & (~((-1ULL)<<pbits)),1ULL<<pbits,res>>pbits,part2string(res,pbits,buf,128));
	return 0;
}
#endif /* TEST */
