/*******************************************************************
  asn.h

    ASN.1, BER and DER codec.
    Written by Zhuang Hao S.S.* (Jerrey <jerrey@usa.net>)
    Copyright(C) 1998
    All Rights Reserved.
 -------------------------------------------------------------------
    ANY MODIFICATION MADE TO THIS SOURCE CODE IS AT YOUR OWN RISK.
 *******************************************************************/

#ifndef __SS_ASN_DEFINED__
#define __SS_ASN_DEFINED__ 1

#include    "mpidefs.h"

#ifdef __cplusplus
extern "C" {
#endif


/* (asn, maxLen)
   Recursively check if the input BER code bares a valid length within
    the given boundary. If this check is passed, all the components in
    the value have been tested complying to this length limitation.
   This is the most important security check on a incoming BER code to
    avoid malicious designed BER codes cracking the BER decode procedure.
   IT MUST BE PERFORMED FIRST TO ANY INCOMING BER CODE PRIOR TO ANY
    FURTHER PROCESSING.
   Returns a positive byte-length of the whole BER code if successful,
    otherwise a 0 indicating an bad code.
 */
extern int ASN_BoundaryCheck PROTO_LIST ((const BYTE *, int));

/* (asn, len)
   Encode the ASN.1 'length' field.
   len is the byte-length of the contents octets. If len < 0, indefinite
    length is used.
   Returns the byte-length of the encoded 'length' field, otherwise 0
    indicating an error.
 */
extern int ASN_LengthEncode PROTO_LIST ((BYTE *, int));

/* (asn, len)
   Decode the ASN.1 'length' field.
   Output *len is the byte-length of the contents octets.
   Returns the byte-length of the encoded 'length' field, otherwise -1
    indicating an indefinite length and 0 an error, such as asn == NULL.
 */
extern int ASN_LengthDecode PROTO_LIST ((int *, const BYTE *));

/* (asn, mpi)
   Convert MPI to DER Encoded ASN.1 Integer format.
   len is the length of mpi counted in U_WORDs.
   If asn == (BYTE *)0 then only returns the length. Encoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 integer data, otherwise
    0 indicating an error.
 */
extern int ASN_MPIToASNInteger PROTO_LIST ((BYTE *, CONST_MPI));

/* (mpi, asn, len)
   Convert BER Encoded ASN.1 Integer to MPI format.
   If mpi == (MPI)0 then only returns the length. Decoded data
    is NOT stored.
   Output *len is the length of mpi counted in U_WORDs.
   Returns the byte-length of the encoded ASN.1 integer data, otherwise
    0 indicating an error.
 */
extern int ASN_ASNIntegerToMPI PROTO_LIST ((MPI, int *, const BYTE *));

/* (asn, bitStr, bitLen)
   Convert BitString to DER Encoded ASN.1 BitString format.
   BitString is presented in BYTE *bitStr, with the 1st bit in bit7 of
    bitStr[0], 2nd bit in bit6 of bitStr[0]...8th bit in bit0 of bitStr[0]
    ...
   bitLen is the number of bits.
   If asn == (BYTE *)0 then only returns the length. Encoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 BitString data, otherwise
    0 indicating an error.
 */
extern int ASN_BitStringToASNBitString PROTO_LIST
    ((BYTE *, const BYTE *, int));

/* (bitStr, bitLen, asn)
   Convert BER encoded ASN.1 BitString to BitString format.
   BitString is presented in BYTE *bitStr, with the 1st bit in bit7 of
    bitStr[0], 2nd bit in bit6 of bitStr[0]...8th bit in bit0 of bitStr[0]
    ...
   *bitLen is the number of bits.
   If bitStr == (BYTE *)0 then only returns the length. Decoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 BitString data, otherwise
    0 indicating an error.
 */
extern int ASN_ASNBitStringToBitString PROTO_LIST
    ((BYTE *, int *, const BYTE *));

/* (asn, str, len)
   Convert ByteString to DER Encoded ASN.1 OctetString format.
   If asn == (BYTE *)0 then only returns the length. Encoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 OctetString data, otherwise
    0 indicating an error.
 */
extern int ASN_ByteStringToASNOctetString PROTO_LIST
    ((BYTE *, const BYTE *, int));

/* (str, len, asn)
   Convert BER encoded ASN.1 OctetString to ByteString format.
   If str == (BYTE *)0 then only returns the length. Decoded data
    is NOT stored.
   Returns the byte-length of the encoded ASN.1 OctetString data, otherwise
    0 indicating an error.
 */
extern int ASN_ASNOctetStringToByteString PROTO_LIST
    ((BYTE *, int *, const BYTE *));

/* (asn)
   Returns the byte-length of an BER coded value. Returns 0 means the
    input code is not a valid BER code.
 */
extern int ASN_Len PROTO_LIST ((const BYTE *));

/* (asn)
   Returns the byte-length of the unsigned ASN.1 integer if successful,
    otherwise -1 indicating an error.
 */
extern int ASN_ASNIntegerByteLen PROTO_LIST ((const BYTE *));

/* (asn1, asn2)
   Returns if the two input BER codes are the same OBJECT IDENTIFIER
 */
extern BOOL ASN_IsEqualObjectID PROTO_LIST ((const BYTE *, const BYTE *));

/* (asn)
   Return if the BER code is a NULL type.
 */
extern BOOL ASN_IsNull PROTO_LIST ((const BYTE *));


#ifdef __cplusplus
}
#endif

#endif
