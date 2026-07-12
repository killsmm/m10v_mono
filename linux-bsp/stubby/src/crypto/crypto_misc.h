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
 * @file crypto_misc.h
 */

#ifndef HEADER_CRYPTO_MISC_H
#define HEADER_CRYPTO_MISC_H

#include "crypto.h"
#include "bigint.h"

#define ASN1_INTEGER            0x02
#define ASN1_BIT_STRING         0x03
#define ASN1_OCTET_STRING       0x04
#define ASN1_NULL               0x05
#define ASN1_PRINTABLE_STR2     0x0C
#define ASN1_OID                0x06
#define ASN1_PRINTABLE_STR2     0x0C
#define ASN1_PRINTABLE_STR      0x13
#define ASN1_TELETEX_STR        0x14
#define ASN1_IA5_STR            0x16
#define ASN1_UTC_TIME           0x17
#define ASN1_UNICODE_STR        0x1e
#define ASN1_SEQUENCE           0x30
#define ASN1_CONTEXT_DNSNAME	0x82
#define ASN1_SET                0x31
#define ASN1_V3_DATA			0xa3
#define ASN1_IMPLICIT_TAG       0x80
#define ASN1_CONTEXT_DNSNAME	0x82
#define ASN1_EXPLICIT_TAG       0xa0
#define ASN1_V3_DATA			0xa3

#define SIG_TYPE_MD2            0x02
#define SIG_TYPE_MD5            0x04
#define SIG_TYPE_SHA1           0x05

int get_asn1_length(const uint8_t *buf, int *offset);
int asn1_next_obj(const uint8_t *buf, int *offset, int obj_type);
int asn1_skip_obj(const uint8_t *buf, int *offset, int obj_type);
int asn1_get_int(const uint8_t *buf, int *offset, uint8_t **object);
int asn1_name(const uint8_t *cert, int *offset, char *dn[]);
int asn1_public_key(const uint8_t *cert, int *offset, RSA_CTX *rsa_ctx); 
int asn1_compare_dn(char * const dn1[], char * const dn2[]);

#endif 
