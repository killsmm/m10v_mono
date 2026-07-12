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

/**
 * Some primitive asn methods for extraction ASN.1 data.
 */
#include "asm/utils.h"
#include "string.h"
#include "crypto.h"
#include "crypto_misc.h"

#define SIG_OID_PREFIX_SIZE 8
#define SIG_IIS6_OID_SIZE   5
#define SIG_SUBJECT_ALT_NAME_SIZE 3

/* Must be an RSA algorithm with either SHA1 or MD5 for verifying to work */
static const uint8_t sig_oid_prefix[SIG_OID_PREFIX_SIZE] = 
{
    0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01
};

static const uint8_t sig_sha1WithRSAEncrypt[SIG_IIS6_OID_SIZE] =
{
    0x2b, 0x0e, 0x03, 0x02, 0x1d
};

static const uint8_t sig_subject_alt_name[SIG_SUBJECT_ALT_NAME_SIZE] =
{
    0x55, 0x1d, 0x11
};

/* CN, O, OU */
static const uint8_t g_dn_types[] = { 3, 10, 11 };

int get_asn1_length(const uint8_t *buf, int *offset)
{
    int len, i;

    if (!(buf[*offset] & 0x80)) /* short form */
    {
        len = buf[(*offset)++];
    }
    else  /* long form */
    {
        int length_bytes = buf[(*offset)++]&0x7f;
        len = 0;
        for (i = 0; i < length_bytes; i++)
        {
            len <<= 8;
            len += buf[(*offset)++];
        }
    }

    return len;
}

/**
 * Skip the ASN1.1 object type and its length. Get ready to read the object's
 * data.
 */
int asn1_next_obj(const uint8_t *buf, int *offset, int obj_type)
{
    if (buf[*offset] != obj_type)
        return -1;
    (*offset)++;
    return get_asn1_length(buf, offset);
}

/**
 * Skip over an ASN.1 object type completely. Get ready to read the next
 * object.
 */
int asn1_skip_obj(const uint8_t *buf, int *offset, int obj_type)
{
    int len;

    if (buf[*offset] != obj_type)
        return -1;
    (*offset)++;
    len = get_asn1_length(buf, offset);
    *offset += len;
    return 0;
}

/**
 * Read an integer value for ASN.1 data
 * Note: This function allocates memory which must be freed by the user.
 */
int asn1_get_int(const uint8_t *buf, int *offset, uint8_t **object)
{
    int len;

    if ((len = asn1_next_obj(buf, offset, ASN1_INTEGER)) < 0)
        goto end_int_array;

    if (len > 1 && buf[*offset] == 0x00)    /* ignore the negative byte */
    {
        len--;
        (*offset)++;
    }

    *object = (uint8_t *)malloc(len);

    memcpy(*object, &buf[*offset], len);
    *offset += len;

end_int_array:
    return len;
}

/**
 * Read the modulus and public exponent of a certificate.
 */
int asn1_public_key(const uint8_t *cert, int *offset, RSA_CTX *rsa_ctx)
{
    int mod_len, pub_len;
    uint8_t *modulus = NULL, *pub_exp = NULL;

    if (asn1_next_obj(cert, offset, ASN1_SEQUENCE) < 0 ||
            asn1_skip_obj(cert, offset, ASN1_SEQUENCE) ||
            asn1_next_obj(cert, offset, ASN1_BIT_STRING) < 0)
        return 1;

    (*offset)++;        /* ignore the padding bit field */

    if (asn1_next_obj(cert, offset, ASN1_SEQUENCE) < 0)
        return 1;

    mod_len = asn1_get_int(cert, offset, &modulus);
    pub_len = asn1_get_int(cert, offset, &pub_exp);

    RSA_pub_key_new(rsa_ctx, modulus, mod_len, pub_exp, pub_len);
    
    return 0;
}

#endif
