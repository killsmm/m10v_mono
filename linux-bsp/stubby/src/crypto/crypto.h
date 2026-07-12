/*
 * Copyright (c) 2007, Cameron Rich
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors 
 *   may be used to endorse or promote products derived from this software 
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file crypto.h
 */

#ifndef HEADER_CRYPTO_H
#define HEADER_CRYPTO_H

#include "config.h"
#include "bigint_impl.h"
#include "bigint.h"

/* libwebsockets SHA1 */
#define u_int64_t unsigned long long

#define SHA1_BINARY_DIGEST_BYTES (20)

struct sha1_ctxt {
	union {
		unsigned char		b8[20];
		unsigned int		b32[5];
	} h;
	union {
		unsigned char		b8[8];
		u_int64_t		b64[1];
	} c;
	union {
		unsigned char		b8[64];
		unsigned int		b32[16];
	} m;
	unsigned char			count;
};
void sha1_init(struct sha1_ctxt *ctxt);
void sha1_result(struct sha1_ctxt *ctxt, void *digest0);
void sha1_loop(struct sha1_ctxt *ctxt, const unsigned char *input, size_t len);


#define SHA256_BINARY_DIGEST_BYTES (32)

struct sha256_ctxt {
	unsigned long state[8];
	unsigned long length;
	unsigned long curlen;
	unsigned char buf[64];
};

void sha256_init(struct sha256_ctxt *ctxt);
void sha256_result(struct sha256_ctxt *ctxt, void *digest0);
void sha256_loop(struct sha256_ctxt *ctxt,
			const unsigned char *input, size_t len);


#define MAX_BINARY_DIGEST_BYTES SHA256_BINARY_DIGEST_BYTES

#ifdef CONFIG_HASH_TYPE_SHA1
#define hash_init sha1_init
#define hash_loop sha1_loop
#define hash_result sha1_result
#define hash_type struct sha1_ctxt
#define BINARY_DIGEST_BYTES SHA1_BINARY_DIGEST_BYTES
#endif
#ifdef CONFIG_HASH_TYPE_SHA256
#define hash_init sha256_init
#define hash_loop sha256_loop
#define hash_result sha256_result
#define hash_type struct sha256_ctxt
#define BINARY_DIGEST_BYTES SHA256_BINARY_DIGEST_BYTES
#endif


typedef struct 
{
    bigint *m;              /* modulus */
    bigint *e;              /* public exponent */
    bigint *d;              /* private exponent */

    bigint *p;              /* p as in m = pq */
    bigint *q;              /* q as in m = pq */
    bigint *dP;             /* d mod (p-1) */
    bigint *dQ;             /* d mod (q-1) */
    bigint *qInv;           /* q^-1 mod p */

    int num_octets;
    BI_CTX *bi_ctx;
} RSA_CTX;

void RSA_pub_key_new(RSA_CTX *rsa_ctx, 
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len);
int RSA_decrypt(const RSA_CTX *ctx, const uint8_t *in_data, uint8_t *out_data);

int verify_signature(unsigned char *hash, void *signature);
void print_pubkey_fingerprint(void);

#endif 
