#ifdef USE_MINE_LIBC
extern "C"
{
int memcmp(const void *buf1,  const void *buf2,  size_t count)
{
	unsigned char *p = (unsigned char *)buf1;
	unsigned char *q = (unsigned char *)buf2;
	for (size_t i=0;i<count;i++,p++,q++)
		if (*p < *q)
			return -1;
		else if (*p > *q)
			return 1;
	return 0;
}

void *memmove(void *dest, const void *source, size_t length)
{
	const char *s0 = (const char *)source;
	char *d0 = (char *)dest;
	char *d1 = (char *)dest;
	if (s0 < d1)
		/* Moving from low mem to hi mem; start at end.  */
		for (s0 += length, d1 += length; length; --length)
			*--d1 = *--s0;
		else if (s0 != d1)
		{
			/* Moving from hi mem to low mem; start at beginning.  */
			for (; length; --length)
				*d1++ = *s0++;
		}
		return (void *) d0;
}


void * memset (void *str, int c, unsigned int len)
{
	register char *st = (char *)str;
	while (len-- > 0)
		*st++ = (char)c;
	return str;
}


};
#endif