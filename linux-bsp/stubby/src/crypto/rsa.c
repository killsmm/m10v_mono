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
#ifdef CONFIG_RSA_SIGNATURE

#include "asm/utils.h"
#include "string.h"
#include "crypto/crypto.h"

#include "crypto.h"

void RSA_pub_key_new(RSA_CTX *rsa_ctx, 
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len)
{
    BI_CTX *bi_ctx;
    bi_ctx = bi_initialize();
    rsa_ctx->bi_ctx = bi_ctx;
    rsa_ctx->num_octets = mod_len;
    rsa_ctx->m = bi_import(bi_ctx, modulus, mod_len);
    bi_set_mod(bi_ctx, rsa_ctx->m, BIGINT_M_OFFSET);
    rsa_ctx->e = bi_import(bi_ctx, pub_exp, pub_len);
}

int RSA_decrypt(const RSA_CTX *ctx, const uint8_t *in_data, 
                            uint8_t *out_data)
{
	const int byte_size = ctx->num_octets;
	int i = 10, size;
	bigint *decrypted_bi, *dat_bi;
	uint8_t block[4096 / 8];

	memset(out_data, 0, byte_size); /* initialise */
	memset(block, 0, byte_size);

	/* decrypt */
	dat_bi = bi_import(ctx->bi_ctx, in_data, byte_size);
	ctx->bi_ctx->mod_offset = BIGINT_M_OFFSET;
	decrypted_bi = bi_mod_power(ctx->bi_ctx, dat_bi, ctx->e);
	bi_export(ctx->bi_ctx, decrypted_bi, block, byte_size);
#if 0
	{
		int n;
		for (n = 0; n < byte_size; n++) {
			print8(block[n]);
			puts(" ");
		}
		puts("\n");
	}
#endif
	while (block[i++] == 0xff && i < byte_size)
		;
	if (block[i - 2] != 0xff)
		return -1;

	size = byte_size - i;
	if (size <= 0)
		return -1;

	memcpy(out_data, &block[i], size);

	return size;
}

#endif
