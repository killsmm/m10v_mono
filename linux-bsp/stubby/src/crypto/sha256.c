/*
 * MySQL SHA256() User Defined Function
 * $Id: sha256.c,v 1.5 2007/02/11 18:56:54 stuge Exp $
 *
 * This code is free for all purposes without any express guarantee it works.
 *
 * Peter Stuge <peter@stuge.se>
 *
 * Uses public domain SHA256 implementation by Tom St Denis
 * http://www.mirrors.wiretapped.net/security/cryptography/hashes/sha2/sha256-stdenis.c
 * With some updates from LibTomCrypt also by Tom St Denis
 * http://libtom.org/cvs/cvsweb.pl/libtom/libtomcrypt/src/hashes/sha2/sha256.c?rev=1.10
 */

#ifdef CONFIG_RSA_SIGNATURE
#ifdef CONFIG_HASH_TYPE_SHA256

#include "asm/utils.h"
#include "string.h"
#include "crypto/crypto.h"

#include "crypto.h"

static const unsigned long K[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
    0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
    0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
    0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
    0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
    0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
    0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
    0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
    0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
    0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

#define Ch(x,y,z)	(z ^ (x & (y ^ z)))
#define Maj(x,y,z)	(((x | y) & z) | (x & y))
#define S(x, n)		(((x)>>((n)&31))|((x)<<(32-((n)&31))))
#define R(x, n)		(((x)&0xffffffffUL)>>(n))
#define Sigma0(x)	(S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)	(S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)	(S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)	(S(x, 17) ^ S(x, 19) ^ R(x, 10))

static void
sha_compress(struct sha256_ctxt *md)
{
	unsigned long S[8];
	unsigned long W[64];
	unsigned long t0;
	unsigned long t1;
	int i;

	/* copy state into S */
	for (i = 0; i < 8; i++)
		S[i] = md->state[i];

	/* copy the state into 512-bits into W[0..15] */
	for (i = 0; i < 16; i++)
		W[i] =	(((unsigned long) md->buf[(4 * i) + 0]) << 24) |
			(((unsigned long) md->buf[(4 * i) + 1]) << 16) |
			(((unsigned long) md->buf[(4 * i) + 2]) << 8) |
			(((unsigned long) md->buf[(4 * i) + 3]));

	/* fill W[16..63] */
	for (i = 16; i < 64; i++)
		W[i] = Gamma1(W[i - 2]) + W[i - 7] +
					Gamma0(W[i - 15]) + W[i - 16];

	/* Compress */
	for (i = 0; i < 64; i++) {
		t0 = S[7] + Sigma1(S[4]) + Ch(S[4], S[5], S[6]) + K[i] + W[i];
		t1 = Sigma0(S[0]) + Maj(S[0], S[1], S[2]);
		S[7] = S[6];
		S[6] = S[5];
		S[5] = S[4];
		S[4] = S[3] + t0;
		S[3] = S[2];
		S[2] = S[1];
		S[1] = S[0];
		S[0] = t0 + t1;
	}

	/* feedback */
	for (i = 0; i < 8; i++)
		md->state[i] += S[i];
}

void
sha256_init(struct sha256_ctxt *md)
{
	md->curlen = 0;
	md->length = 0;
	md->state[0] = 0x6A09E667UL;
	md->state[1] = 0xBB67AE85UL;
	md->state[2] = 0x3C6EF372UL;
	md->state[3] = 0xA54FF53AUL;
	md->state[4] = 0x510E527FUL;
	md->state[5] = 0x9B05688CUL;
	md->state[6] = 0x1F83D9ABUL;
	md->state[7] = 0x5BE0CD19UL;
}

void
sha256_loop(struct sha256_ctxt *md, const unsigned char *buf, size_t len)
{
	while (len--) {
		md->buf[md->curlen++] = *buf++;
		if (md->curlen != 64)
			continue;

		sha_compress(md);
		md->length += 512;
		md->curlen = 0;
	}
}

void
sha256_result(struct sha256_ctxt *md, void *digest0)
{
	int i;
	unsigned char *p = digest0;

	/* increase the length of the message */
	md->length += md->curlen * 8;

	/* append the '1' bit */
	md->buf[md->curlen++] = 0x80;

	/*
	 * if the length is currenlly above 56 bytes we append zeros
	 * then compress.  Then we can fall back to padding zeros and length
	 * encoding like normal.
	 */
	if (md->curlen >= 56) {
		for (; md->curlen < 64;)
			md->buf[md->curlen++] = 0;
		sha_compress(md);
		md->curlen = 0;
	}

	/* pad upto 56 bytes of zeroes */
	for (; md->curlen < 56;)
		md->buf[md->curlen++] = 0;

	/* since all messages are under 2^32 bits we mark the top bits zero */
	for (i = 56; i < 60; i++)
		md->buf[i] = 0;

	/* append length */
	for (i = 60; i < 64; i++)
		md->buf[i] = (md->length >> ((63 - i) * 8)) & 255;
	sha_compress(md);

	for (i = 0; i < 32; i++)
		p[i] = (md->state[i >> 2] >> (((3 - i) & 3) << 3)) & 255;
}

#endif
#endif
