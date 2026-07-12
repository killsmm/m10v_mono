/*
 * (C) Copyright 2013 Linaro Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifdef CONFIG_RSA_SIGNATURE
/**
 * Implements the RSA public encryption algorithm. Uses the bigint library to
 * perform its calculations.
 */
#include "asm/utils.h"
#include "string.h"
#include "crypto/crypto.h"

#include "crypto.h"

static const unsigned char pubkey[] = {
#include "../pubkey.h"
};

static const char * const revocation_list[] = {
#include "../revocation-hashes.h"
};

void print_pubkey_fingerprint(void)
{
	int n, j;
	int offset = 0;
	RSA_CTX rsa_ctx;

	n = asn1_public_key(pubkey, &offset, &rsa_ctx);
	if (n) {
		puts("Failed to get pubkey\n");
		return;
	}
	n = rsa_ctx.m->size - 1;
	printdec((n + 1) * COMP_NUM_NIBBLES * 4);
	puts("/");

	if (n > 2)
		n = 2;
	
	/* just pick 32-bits at n = 2 */
	
	//for (; n >= 0; n--)
        	for (j = COMP_NUM_NIBBLES - 1; j >= 0; j--) {
			comp mask = 0xf << (j * 4);
			comp num = (rsa_ctx.m->comps[n] & mask) >> (j * 4);
			putc((num <= 9) ? (num + '0') : (num + 'A' - 10));
		}
}

static int hex_char(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

int verify_signature(unsigned char *proposed_digest, void *signature)
{
	RSA_CTX rsa_ctx;
	char out_data[512];
	unsigned char digest[BINARY_DIGEST_BYTES + 1];
	int n, m, offset = 0;
	int signature_len;
	int r, r1;

	digest[BINARY_DIGEST_BYTES] = '\0';
#if 0
	puts("proposed digest: ");
	for (r = 0; r < BINARY_DIGEST_BYTES; r++)
		print8(proposed_digest[r]);
	puts("\n");
#endif

#if 0
	puts("signature: ");
	for (r = 0; r < 256; r++)
		print8(((unsigned char *)signature)[r]);
	puts("\n");
#endif

	n = asn1_public_key(pubkey, &offset, &rsa_ctx);
	if (n)
		goto nope;

	n = RSA_decrypt(&rsa_ctx, signature, out_data);
	if (n <= 0)
		goto nope;

#ifdef CONFIG_HASH_TYPE_SHA1

	if (n < (BINARY_DIGEST_BYTES * 2) + 4)
		goto nope;
	if (out_data[0] !='S' || out_data[1] != 'H' || out_data[2] != 'A' ||
				      out_data[3] != '1' || out_data[4] != '(')
		goto nope;

	/* if not, then see if it matches */

	m = 5;
#endif

#ifdef CONFIG_HASH_TYPE_SHA256

	if (n < (BINARY_DIGEST_BYTES * 2) + 6)
		goto nope;
	if (out_data[0] !='S' || out_data[1] != 'H' || out_data[2] != 'A' ||
	    out_data[3] != '2' || out_data[4] != '5' || out_data[5] != '6' ||
	    out_data[6] != '(')
		goto nope;

	/* if not, then see if it matches */

	m = 7;
#endif

	while (m < (n - (BINARY_DIGEST_BYTES * 2)) && out_data[m] != ')')
		m++;

	if (m > (n - (BINARY_DIGEST_BYTES * 2)))
		goto nope;
	m++;
	if (out_data[m++] != '=')
		goto nope;
	if (out_data[m++] != ' ')
		goto nope;

	for (n = 0; n < (BINARY_DIGEST_BYTES * 2); n += 2) {
		r = hex_char(out_data[m + n]);
		if (r < 0)
			goto nope;
		r1 = hex_char(out_data[m + n + 1]);
		if (r1 < 0)
			goto nope;
		digest[n >> 1] = (r << 4) | r1;
	}
#if 0
	puts(out_data);
	puts("digest\n");
	for (n = 0; n < 20; n++)
		print8(digest[n]);
	puts("\n");
#endif
	if (memcmp(digest, proposed_digest, BINARY_DIGEST_BYTES))
		goto nope;

	/*
	 * check if this hash is revoked
	 */
	for (r = 0; r < ARRAY_SIZE(revocation_list); r++)
		if (!strcmp(revocation_list[r], out_data)) {
			puts("  Revoked sig\n");
			goto nope;
		}

	/*
	 * accept
	 */

	return 0;

nope:
	puts("   Failed sig check\n");
	return 1;
}

#endif

