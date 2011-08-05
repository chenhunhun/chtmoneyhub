/*******************************************************************
  asn.c

    ASN.1, BER and DER codec.
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
 *******************************************************************/

#include    "asn.h"
#include    "mpialgo.h"

#define MSBMASK(x)          (((UINT)0xff) << ((sizeof (x)-1)*8))
#define MSB(x)              ((((UINT)(x)) >> ((sizeof (x)-1)*8))&0xff)


/* Encode the ASN.1 'length' field.
   len is the byte-length of the contents octets. If len < 0, indefinite
    length is used.
   Returns the byte-length of the encoded 'length' field, otherwise 0
    indicating an error.
 */
int ASN_LengthEncode (asn, len)
BYTE                    *asn;
int                     len;
{
    int                 i, m;

    if (len <= 127) {
        if (asn) {
            asn[1] = len < 0 ? (BYTE)0x80 : (BYTE)len;
        }
        return (1);
    }
    for (m = sizeof (len); ! (len&(int)MSBMASK(len)); len <<= 8, --m);
    if (asn) {
        asn[1] = (BYTE)(m|0x80);
        for (i = 0; i < m; ++i, len <<= 8) {
            asn[2+i] = (BYTE)MSB (len);
        }
    }
    return (m+1);
}

/* Decode the ASN.1 'length' field.
   Output *len is the byte-length of the contents octets.
   Returns the byte-length of the encoded 'length' field, otherwise -1
    indicating an indefinite length and 0 an error, such as asn == NULL.
 */
int ASN_LengthDecode (len, asn)
const BYTE              *asn;
int                     *len;
{
    int                 i, k, m;

    if (asn) {
        if ((m = (int)asn[1]) <= 127) {
            *len = m;
            return (1);
        }
        if (! (m &= 0x7f)) {
            return (-1);
        }
        if (m > sizeof (int)) {
            return (0);
        }
        for (k = i = 0; i < m; k = (k << 8)+asn[2+(i++)]);
        *len = k;
        return (m+1);
    }
    return (0);
}

/* Convert MPI to DER Encoded ASN.1 Integer format.
   len is the length of mpi counted in U_WORDs.
   If asn == (BYTE *)0 then only returns the length. Encoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 integer data, otherwise
    0 indicating an error.
 */
int ASN_MPIToASNInteger (asn, mpi)
BYTE                    *asn;
CONST_MPI               mpi;
{
    U_WORD              u;
    int                 i, k, l, m;

    if (mpi == (MPI)0) {
        return (0);
    }
    if (! (l = MPI_Len (mpi))) {
        if (asn) {
            asn[0] = (BYTE)2, asn[1] = (BYTE)1, asn[2] = (BYTE)0;
        }
        return (3);
    }
    for (u = mpi[l], i = MPI_UWORDBYTES; ! (u >> (MPI_UWORDBITS-8));
      --i, u <<= 8);
    k = MPI_UWORDBYTES*(l-1)+i;
    if ((u&MPI_UWORDMSB)) {                     /* MPI is non-negative  */
        ++k;                                    /* Thus should insert a */
    }                                           /* 0 if it's MSb is 1   */
    if (! (m = ASN_LengthEncode (asn, k))) {
        return (0);
    }
    k += m+1;
    if (asn) {
        asn[0] = (BYTE)2;
        if ((u&MPI_UWORDMSB)) {                 /* MPI is non-negative  */
            asn[++m] = 0;                       /* Thus should insert a */
        }                                       /* 0 if it's MSb is 1   */
        for (k = m+1; i > 0; --i, u <<= 8) {
            asn[k++] = (BYTE)(u >> (MPI_UWORDBITS-8));
        }
        for (--l; l > 0; --l) {
            for (u = mpi[l], i = MPI_UWORDBYTES; i > 0; --i, u <<= 8) {
                asn[k++] = (BYTE)(u >> (MPI_UWORDBITS-8));
            }
        }
    }
    return (k);
}

/* Convert BER Encoded ASN.1 Integer to MPI format.
   If mpi == (MPI)0 then only returns the length. Decoded data
    is NOT stored.
   Output *len is the length of mpi counted in U_WORDs.
   Returns the byte-length of the encoded ASN.1 integer data, otherwise
    0 indicating an error.
 */
int ASN_ASNIntegerToMPI (mpi, len, asn)
MPI                     mpi;
const BYTE              *asn;
int                     *len;
{
    U_WORD              u;
    int                 i, k, l, m;

    if ((m = ASN_LengthDecode (&k, asn)) <= 0 || asn[0] != 2) {
        return (0);
    }
    if ((asn[m+1]&(BYTE)0x80)) {                /* MPI is non-negative */
        return (0);
    }
    while (k > 0 && ! asn[m+1]) {
        ++m, --k;
    }
    *len = (k+MPI_UWORDBYTES-1)/MPI_UWORDBYTES;
    if (mpi) {
        if ((*mpi = (U_WORD)(l = *len))) {
            i = (k-1)%MPI_UWORDBYTES+1;
            for (k = m+1; l > 0; mpi[l--] = u, i = MPI_UWORDBYTES) {
                for (u = 0; i > 0; --i, u = (u << 8)|asn[k++]);
            }
        }
    }
    else {
        k += m+1;
    }
    return (k);
}

/* Convert BitString to DER Encoded ASN.1 BitString format.
   BitString is presented in BYTE *bitStr, with the 1st bit in bit7 of
    bitStr[0], 2nd bit in bit6 of bitStr[0]...8th bit in bit0 of bitStr[0]
    ...
   bitLen is the number of bits.
   If asn == (BYTE *)0 then only returns the length. Encoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 BitString data, otherwise
    0 indicating an error.
 */
int ASN_BitStringToASNBitString (asn, bitStr, bitLen)
BYTE                    *asn;
const BYTE              *bitStr;
int                     bitLen;
{
    int                 len, m, unused;

    len = unused = 0;
    if (bitLen > 0) {
        len = (bitLen+15)/8, unused = (8-(bitLen%8))%8;
    }
    if (! (m = ASN_LengthEncode (asn, len))) {
        return (0);
    }
    if (asn) {
        asn[0] = (BYTE)3;
        if (len) {
            asn[1+m] = (BYTE)unused;
            T_memcpy (asn+2+m, bitStr, len-1);
        }
    }
    return (1+m+len);
}

/* Convert BER encoded ASN.1 BitString to BitString format.
   BitString is presented in BYTE *bitStr, with the 1st bit in bit7 of
    bitStr[0], 2nd bit in bit6 of bitStr[0]...8th bit in bit0 of bitStr[0]
    ...
   *bitLen is the number of bits.
   If bitStr == (BYTE *)0 then only returns the length. Decoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 BitString data, otherwise
    0 indicating an error.
 */
int ASN_ASNBitStringToBitString (bitStr, bitLen, asn)
BYTE                    *bitStr;
const BYTE              *asn;
int                     *bitLen;
{
    BYTE                *p;
    int                 bl, i, j, k, len, m, t;

    if ((m = ASN_LengthDecode (&len, asn)) == 0
     || (asn[0] != 3 && asn[0] != 0x23)) {
        return (0);
    }
    if (asn[0] == 3) {
        if (m < 0) {
            return (0);
        }
        if (len) {
            if ((t = (int)asn[1+m]) > 7) {
                return (0);
            }
            if (bitStr) {
                T_memcpy (bitStr, asn+2+m, len-1);
            }
        }
        *bitLen = len ? (len-1)*8-t : 0;
        return (1+m+len);
    }
    m = (t = m) < 0 ? 1 : m;
    for (bl = i = 0; (t > 0 && i < len) || (t < 0 && (asn[2+i]|asn[3+i])); ) {
        if (bl%8) {
            return (0);
        }
        p = bitStr ? bitStr+bl/8 : (BYTE *)0;
        if (! (j = ASN_ASNBitStringToBitString (p, &k, asn+1+m+i))) {
            return (0);
        }
        bl += k, i += j;
    }
    if (t > 0 && i != len) {
        return (0);
    }
    *bitLen = bl;
    m = t < 0 ? 3 : m;
    return (1+m+i);
}

/* Convert ByteString to DER Encoded ASN.1 OctetString format.
   If asn == (BYTE *)0 then only returns the length. Encoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 OctetString data, otherwise
    0 indicating an error.
 */
int ASN_ByteStringToASNOctetString (asn, str, len)
BYTE                    *asn;
const BYTE              *str;
int                     len;
{
    int                 m;

    if (! (m = ASN_LengthEncode (asn, len))) {
        return (0);
    }
    if (asn) {
        asn[0] = (BYTE)4;
        if (len) {
            T_memcpy (asn+1+m, str, len);
        }
    }
    return (1+m+len);
}

/* Convert BER encoded ASN.1 OctetString to ByteString format.
   If str == (BYTE *)0 then only returns the length. Decoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 OctetString data, otherwise
    0 indicating an error.
 */
int ASN_ASNOctetStringToByteString (str, len, asn)
BYTE                    *str;
const BYTE              *asn;
int                     *len;
{
    BYTE                *p;
    int                 bl, i, j, k, m, t;

    if ((m = ASN_LengthDecode (len, asn)) == 0
     || (asn[0] != 4 && asn[0] != 0x24)) {
        return (0);
    }
    if (asn[0] == 4) {
        if (m < 0) {
            return (0);
        }
        if (*len && str) {
            T_memcpy (str, asn+1+m, *len);
        }
        return (1+m+*len);
    }
    m = (t = m) < 0 ? 1 : m;
    for (bl = i = 0; (t > 0 && i < *len) || (t < 0 && (asn[2+i]|asn[3+i])); ) {
        p = str ? str+bl : (BYTE *)0;
        if (! (j = ASN_ASNOctetStringToByteString (p, &k, asn+1+m+i))) {
            return (0);
        }
        bl += k, i += j;
    }
    if (t > 0 && i != *len) {
        return (0);
    }
    *len = bl;
    m = t < 0 ? 3 : m;
    return (1+m+i);
}

/* Recursively check if the input BER code bares a valid length within
    the given boundary. If this check is passed, all the components in
    the value have been tested complying to this length limitation.
   This is the most important security check on a incoming BER code to
    avoid malicious designed BER codes cracking the BER decode procedure.
   IT MUST BE PERFORMED FIRST TO ANY INCOMING BER CODE PRIOR TO ANY
    FURTHER PROCESSING.
   Returns a positive byte-length of the whole BER code if successful,
    otherwise a 0 indicating an bad code.
 */
int ASN_BoundaryCheck (asn, maxLen)
const BYTE              *asn;
int                     maxLen;
{
    int                 i, k, m, s, t;

    if ((t = ASN_LengthDecode (&k, asn)) == 0) {
        return (0);
    }
    m = t < 0 ? 1 : t;
    if (! (asn[0]&(BYTE)0x20)) {
        if (t < 0 || 1+m+k > maxLen) {
            return (0);
        }
        return (1+m+k);
    }
    for (i = 0; (t > 0 && i < k) || (t < 0 && (asn[2+i]|asn[3+i]));
      i += s) {
        if (! (s = ASN_BoundaryCheck (asn+1+m+i, maxLen-1-m-i))) {
            return (0);
        }
    }
    if (t > 0 && i != k) {
        return (0);
    }
    if (t < 0) {
        i += 2;
    }
    return (1+m+i);
}

/* Returns the byte-length of an BER coded value. Returns 0 means the
    input code is not a valid BER code.
 */
int ASN_Len (asn)
const BYTE              *asn;
{
    int                 i, k, m;

    if ((m = ASN_LengthDecode (&k, asn)) == 0) {
        return (0);
    }
    if (m > 0) {
        return (1+m+k);
    }
    if (! (asn[0]&(BYTE)0x20)) {
        return (0);
    }
    for (i = 0; asn[2+i]|asn[3+i]; i += m) {
        if (! (m = ASN_Len (asn+2+i))) {
            return (0);
        }
    }
    return (4+i);
}

/* Returns the byte-length of the unsigned ASN.1 integer if successful,
    otherwise -1 indicating an error.
 */
int ASN_ASNIntegerByteLen (asn)
const BYTE              *asn;
{
    int                 k, m;

    if ((m = ASN_LengthDecode (&k, asn)) <= 0 || asn[0] != 2) {
        return (-1);
    }
    while (k > 0 && ! asn[1+m]) {
        --k, ++m;
    }
    return (k);
}

/* Returns if the two input BER codes are the same OBJECT IDENTIFIER
 */
BOOL ASN_IsEqualObjectID (asn1, asn2)
const BYTE              *asn1, *asn2;
{
    int                 len1, len2, m1, m2;

    if (asn1[0] == 6 && asn2[0] == 6) {
        if ((m1 = ASN_LengthDecode (&len1, asn1)) > 0
         && (m2 = ASN_LengthDecode (&len2, asn2)) > 0) {
            if (len1 == len2 && ! T_memcmp (asn1+1+m1, asn2+1+m2, len1)) {
                return (TRUE);
            }
        }
    }
    return (FALSE);
}

/* Return if the BER code is a NULL type.
 */
BOOL ASN_IsNull (asn)
const BYTE              *asn;
{
    int                 k, m;

    if (asn[0] != 5 || (m = ASN_LengthDecode (&k, asn)) <= 0) {
        return (FALSE);
    }
    return (! (k));
}
