#pragma once


int base64_encode(const unsigned char *in_str, int length, unsigned char *out_str,int *ret_length);
int base64_decode(const unsigned char *in_str, int length, unsigned char *out_str, int *ret_length);
