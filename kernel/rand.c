#include <types.h>
#include <mem.h>
#include <rand.h>

uint64_t srand_insecure(uint64_t x) /* xorshift* */
{
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    return x * 0x2545F4914F6CDD1DULL;
}

/* ChaCha20 */
static inline uint32_t rotl(uint32_t a, uint32_t b)
{
    return ((a) << (b)) | ((a) >> (32 - (b)));
}

static inline void qround(uint32_t *a, uint32_t * b, uint32_t * c, uint32_t * d)
{
    *a += *b; *d ^= *a; *d = rotl(*d, 16);
    *c += *d; *b ^= *c; *b = rotl(*b, 12);
    *a += *b; *d ^= *a; *d = rotl(*d, 8);
    *c += *d; *b ^= *c; *b = rotl(*b, 7);
}
 
static void chacha20(uint32_t out[16], uint32_t const in[16])
{
    int i;
    uint32_t x[16];

    memcpy(x, in, 16*sizeof(in[0]));

    for (i = 0; i < 10; ++i)
    {
        qround(&x[0], &x[4], &x[ 8], &x[12]); 
        qround(&x[1], &x[5], &x[ 9], &x[13]); 
        qround(&x[2], &x[6], &x[10], &x[14]); 
        qround(&x[3], &x[7], &x[11], &x[15]); 

        qround(&x[0], &x[5], &x[10], &x[15]);
        qround(&x[1], &x[6], &x[11], &x[12]); 
        qround(&x[2], &x[7], &x[ 8], &x[13]); 
        qround(&x[3], &x[4], &x[ 9], &x[14]); 
    }

    for (i = 0; i < 16; ++i) out[i] = x[i] + in[i];
}

static uint32_t state[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};

void init_csprng(void)
{
    int i;
    for (i = 0; i < 4; ++i) ((uint64_t *)state)[i] = rand64();
    for (i = 0; i < 3; ++i) state[i + 13] = rand64();
}

uint64_t csprng(void)
{
    uint64_t res;
    uint32_t out[16];

    chacha20(out, state);
    ++state[12];

    res = ((uint64_t) out[0] << 32) | out[1];
    return res;
}
