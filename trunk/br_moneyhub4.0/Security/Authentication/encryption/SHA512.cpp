#include <string.h>

#include "sha512.h"


#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define ROTL64(x, n) (((x) << (n)) | ((x) >> (64 - (n))))
#define ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

#define Ch(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define Maj(x, y, z) (((x) & ((y) | (z))) | ((y) & (z)))
#define SIGMA0(x) (ROTR64((x), 28) ^ ROTR64((x), 34) ^ ROTR64((x), 39))
#define SIGMA1(x) (ROTR64((x), 14) ^ ROTR64((x), 18) ^ ROTR64((x), 41))
#define sigma0(x) (ROTR64((x), 1) ^ ROTR64((x), 8) ^ ((x) >> 7))
#define sigma1(x) (ROTR64((x), 19) ^ ROTR64((x), 61) ^ ((x) >> 6))

#define DO_ROUND() { \
	t1 = h + SIGMA1(e) + Ch(e, f, g) + *(Kp++) + *(W++); \
	t2 = SIGMA0(a) + Maj(a, b, c); \
	h = g; \
	g = f; \
	f = e; \
	e = d + t1; \
	d = c; \
	c = b; \
	b = a; \
	a = t1 + t2; \
}

static const uint64_t K[80] = {
	0x428a2f98d728ae22LL, 0x7137449123ef65cdLL,
	0xb5c0fbcfec4d3b2fLL, 0xe9b5dba58189dbbcLL,
	0x3956c25bf348b538LL, 0x59f111f1b605d019LL,
	0x923f82a4af194f9bLL, 0xab1c5ed5da6d8118LL,
	0xd807aa98a3030242LL, 0x12835b0145706fbeLL,
	0x243185be4ee4b28cLL, 0x550c7dc3d5ffb4e2LL,
	0x72be5d74f27b896fLL, 0x80deb1fe3b1696b1LL,
	0x9bdc06a725c71235LL, 0xc19bf174cf692694LL,
	0xe49b69c19ef14ad2LL, 0xefbe4786384f25e3LL,
	0x0fc19dc68b8cd5b5LL, 0x240ca1cc77ac9c65LL,
	0x2de92c6f592b0275LL, 0x4a7484aa6ea6e483LL,
	0x5cb0a9dcbd41fbd4LL, 0x76f988da831153b5LL,
	0x983e5152ee66dfabLL, 0xa831c66d2db43210LL,
	0xb00327c898fb213fLL, 0xbf597fc7beef0ee4LL,
	0xc6e00bf33da88fc2LL, 0xd5a79147930aa725LL,
	0x06ca6351e003826fLL, 0x142929670a0e6e70LL,
	0x27b70a8546d22ffcLL, 0x2e1b21385c26c926LL,
	0x4d2c6dfc5ac42aedLL, 0x53380d139d95b3dfLL,
	0x650a73548baf63deLL, 0x766a0abb3c77b2a8LL,
	0x81c2c92e47edaee6LL, 0x92722c851482353bLL,
	0xa2bfe8a14cf10364LL, 0xa81a664bbc423001LL,
	0xc24b8b70d0f89791LL, 0xc76c51a30654be30LL,
	0xd192e819d6ef5218LL, 0xd69906245565a910LL,
	0xf40e35855771202aLL, 0x106aa07032bbd1b8LL,
	0x19a4c116b8d2d0c8LL, 0x1e376c085141ab53LL,
	0x2748774cdf8eeb99LL, 0x34b0bcb5e19b48a8LL,
	0x391c0cb3c5c95a63LL, 0x4ed8aa4ae3418acbLL,
	0x5b9cca4f7763e373LL, 0x682e6ff3d6b2b8a3LL,
	0x748f82ee5defb2fcLL, 0x78a5636f43172f60LL,
	0x84c87814a1f0ab72LL, 0x8cc702081a6439ecLL,
	0x90befffa23631e28LL, 0xa4506cebde82bde9LL,
	0xbef9a3f7b2c67915LL, 0xc67178f2e372532bLL,
	0xca273eceea26619cLL, 0xd186b8c721c0c207LL,
	0xeada7dd6cde0eb1eLL, 0xf57d4f7fee6ed178LL,
	0x06f067aa72176fbaLL, 0x0a637dc5a2c898a6LL,
	0x113f9804bef90daeLL, 0x1b710b35131c471bLL,
	0x28db77f523047d84LL, 0x32caab7b40c72493LL,
	0x3c9ebe0a15c9bebcLL, 0x431d67c49c100d4cLL,
	0x4cc5d4becb3e42b6LL, 0x597f299cfc657e2aLL,
	0x5fcb6fab3ad6faecLL, 0x6c44198c4a475817LL
};

#define BYTESWAP(x) ((ROTR((x), 8) & 0xff00ff00L) | \
	(ROTL((x), 8) & 0x00ff00ffL))
#define BYTESWAP64(x) _byteswap64(x)

static inline uint64_t _byteswap64(uint64_t x)
{
	uint32_t a = x >> 32;
	uint32_t b = (uint32_t) x;
	return ((uint64_t) BYTESWAP(b) << 32) | (uint64_t) BYTESWAP(a);
}

static const uint8_t padding[128] = {
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

CSHA512::CSHA512()
{
#ifdef RUNTIME_ENDIAN
	setEndian (&m_pContext->littleEndian);
#endif /* RUNTIME_ENDIAN */

	m_pContext = new SHA512Context;

	m_pContext->totalLength[0] = 0LL;
	m_pContext->totalLength[1] = 0LL;
	m_pContext->hash[0] = 0x6a09e667f3bcc908LL;
	m_pContext->hash[1] = 0xbb67ae8584caa73bLL;
	m_pContext->hash[2] = 0x3c6ef372fe94f82bLL;
	m_pContext->hash[3] = 0xa54ff53a5f1d36f1LL;
	m_pContext->hash[4] = 0x510e527fade682d1LL;
	m_pContext->hash[5] = 0x9b05688c2b3e6c1fLL;
	m_pContext->hash[6] = 0x1f83d9abfb41bd6bLL;
	m_pContext->hash[7] = 0x5be0cd19137e2179LL;
	m_pContext->bufferLength = 0L;
}

CSHA512::~CSHA512()
{
	delete m_pContext;
}

void CSHA512::burnStack (int size)
{
	char buf[128];

	memset (buf, 0, sizeof (buf));
	size -= sizeof (buf);
	if (size > 0)
		burnStack (size);
}

void CSHA512::SHA512Guts (const uint64_t *cbuf)
{
	uint64_t buf[80];
	uint64_t *W, *W2, *W7, *W15, *W16;
	uint64_t a, b, c, d, e, f, g, h;
	uint64_t t1, t2;
	const uint64_t *Kp;
	int i;

	W = buf;

	for (i = 15; i >= 0; i--) {
		*(W++) = BYTESWAP64(*cbuf);
		cbuf++;
	}

	W16 = &buf[0];
	W15 = &buf[1];
	W7 = &buf[9];
	W2 = &buf[14];

	for (i = 63; i >= 0; i--) {
		*(W++) = sigma1(*W2) + *(W7++) + sigma0(*W15) + *(W16++);
		W2++;
		W15++;
	}

	a = m_pContext->hash[0];
	b = m_pContext->hash[1];
	c = m_pContext->hash[2];
	d = m_pContext->hash[3];
	e = m_pContext->hash[4];
	f = m_pContext->hash[5];
	g = m_pContext->hash[6];
	h = m_pContext->hash[7];

	Kp = K;
	W = buf;

	for (i = 79; i >= 0; i--)
		DO_ROUND();

	m_pContext->hash[0] += a;
	m_pContext->hash[1] += b;
	m_pContext->hash[2] += c;
	m_pContext->hash[3] += d;
	m_pContext->hash[4] += e;
	m_pContext->hash[5] += f;
	m_pContext->hash[6] += g;
	m_pContext->hash[7] += h;
}

void CSHA512::SHA512Update (const void *vdata, uint32_t len)
{
	const uint8_t *data = (const uint8_t *)vdata;
	uint32_t bufferBytesLeft;
	uint32_t bytesToCopy;
	uint64_t carryCheck;
	int needBurn = 0;

	if (m_pContext->bufferLength) {
		bufferBytesLeft = 128L - m_pContext->bufferLength;

		bytesToCopy = bufferBytesLeft;
		if (bytesToCopy > len)
			bytesToCopy = len;

		memcpy (&m_pContext->buffer.bytes[m_pContext->bufferLength], data, bytesToCopy);

		carryCheck = m_pContext->totalLength[1];
		m_pContext->totalLength[1] += bytesToCopy * 8L;
		if (m_pContext->totalLength[1] < carryCheck)
			m_pContext->totalLength[0]++;

		m_pContext->bufferLength += bytesToCopy;
		data += bytesToCopy;
		len -= bytesToCopy;

		if (m_pContext->bufferLength == 128L) {
			SHA512Guts (m_pContext->buffer.words);
			needBurn = 1;
			m_pContext->bufferLength = 0L;
		}
	}

	while (len > 127) {
		carryCheck = m_pContext->totalLength[1];
		m_pContext->totalLength[1] += 1024L;
		if (m_pContext->totalLength[1] < carryCheck)
			m_pContext->totalLength[0]++;

		SHA512Guts ((const uint64_t *)data);
		needBurn = 1;

		data += 128L;
		len -= 128L;
	}

	if (len) {
		memcpy (&m_pContext->buffer.bytes[m_pContext->bufferLength], data, len);

		carryCheck = m_pContext->totalLength[1];
		m_pContext->totalLength[1] += len * 8L;
		if (m_pContext->totalLength[1] < carryCheck)
			m_pContext->totalLength[0]++;

		m_pContext->bufferLength += len;
	}

	if (needBurn)
		burnStack (sizeof (uint64_t[90]) + sizeof (uint64_t *[6]) + sizeof (int));
}

uint8_t* CSHA512::SHA512Final ()
{
	uint32_t bytesToPad;
	uint64_t lengthPad[2];
	int i;
	uint8_t* hash = m_hash;

	bytesToPad = 240L - m_pContext->bufferLength;
	if (bytesToPad > 128L)
		bytesToPad -= 128L;

	lengthPad[0] = BYTESWAP64(m_pContext->totalLength[0]);
	lengthPad[1] = BYTESWAP64(m_pContext->totalLength[1]);

	SHA512Update (padding, bytesToPad);
	SHA512Update (lengthPad, 16L);

	if (hash) {
		for (i = 0; i < SHA512_HASH_WORDS; i++) {
			*((uint64_t *) hash) = BYTESWAP64(m_pContext->hash[i]);

			hash += 8;
		}
	}

	return m_hash;
}

