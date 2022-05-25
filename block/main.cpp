#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.hpp"
#define BUFSIZE 1024
#define REC_FLS 1
#define REC_OFP 2
#define REC_IFP 4
#define REC_KEY 8
#define REC_ENC 16
#define REC_DEC 32
#define REC_CTR 64
#define REC_128 128
#define REC_256 256
#define REC_192 512
int main(int argc, char *argv[]) {
	int rec = 0;
	FILE *ifp = nullptr, *ofp = nullptr;
	uint8_t key[32] = {}, civ[16] = {};
	for (int i = 1; (rec & REC_FLS) == 0 and i < argc; i++)
		if (argv[i][0] == '-')
			if (argv[i][1] == 'k' and argv[i][2] == '\0')
				if ((rec & REC_KEY) == 0 and i + 1 < argc) {
					strncpy((char *)key, argv[++i], 32);
					rec |= REC_KEY;
				} else
					rec |= REC_FLS;
			else if (argv[i][1] == 'c' and argv[i][2] == '\0')
				if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0 and i + 1 < argc) {
					strncpy((char *)civ, argv[++i], 16);
					rec |= REC_CTR;
				} else
					rec |= REC_FLS;
			else if (argv[i][1] == 'd' and argv[i][2] == '\0')
				if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0)
					rec |= REC_DEC;
				else
					rec |= REC_FLS;
			else if (argv[i][1] == 'e' and argv[i][2] == '\0')
				if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0)
					rec |= REC_ENC;
				else
					rec |= REC_FLS;
			else if (argv[i][1] == '4' and argv[i][2] == '\0')
				if ((rec & (REC_128 | REC_192 | REC_256)) == 0)
					rec |= REC_128;
				else
					rec |= REC_FLS;
			else if (argv[i][1] == '6' and argv[i][2] == '\0')
				if ((rec & (REC_128 | REC_192 | REC_256)) == 0)
					rec |= REC_192;
				else
					rec |= REC_FLS;
			else if (argv[i][1] == '8' and argv[i][2] == '\0')
				if ((rec & (REC_128 | REC_192 | REC_256)) == 0)
					rec |= REC_256;
				else
					rec |= REC_FLS;
			else if (argv[i][1] == 'i' and argv[i][2] == '\0')
				if ((rec & REC_IFP) == 0 and i + 1 < argc and (ifp = fopen(argv[++i], "rb")))
					rec |= REC_IFP;
				else
					rec |= REC_FLS;
			else if (argv[i][1] == 'o' and argv[i][2] == '\0')
				if ((rec & REC_OFP) == 0 and i + 1 < argc and (ofp = fopen(argv[++i], "wb")))
					rec |= REC_OFP;
				else
					rec |= REC_FLS;
			else
				rec |= REC_FLS;
		else
			rec |= REC_FLS;
	if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0 or (rec & REC_KEY) == 0 or (rec & REC_FLS) != 0) {
		fprintf(stderr, "usage: %s [-4 | -6 | -8] -k KEY (-c CIV | -e | -d) [-i IFILENAME] [-o OFILENAME]\n", argv[0]);
		if ((rec & REC_IFP) != 0)
			fclose(ifp);
		if ((rec & REC_OFP) != 0)
			fclose(ofp);
		return 1;
	}
	if ((rec & REC_IFP) == 0)
		ifp = stdin;
	if ((rec & REC_OFP) == 0)
		ofp = stdout;
	BlockCipherFlow *pEnc;
	BlockCipherFlow *pDec;
	StreamCipherFlow *pCTR;
	if ((rec & (REC_192 | REC_256)) == 0) {
		pEnc = new Encrypter<AES128>(key);
		pDec = new Decrypter<AES128>(key);
		pCTR = new CTRCrypter<AES128>(civ, key);
	} else if ((rec & REC_192) != 0) {
		pEnc = new Encrypter<AES192>(key);
		pDec = new Decrypter<AES192>(key);
		pCTR = new CTRCrypter<AES192>(civ, key);
	} else if ((rec & REC_256) != 0) {
		pEnc = new Encrypter<AES256>(key);
		pDec = new Decrypter<AES256>(key);
		pCTR = new CTRCrypter<AES256>(civ, key);
	}
	if ((rec & REC_CTR) != 0) {
		uint8_t buf[BUFSIZE];
		while (fwrite(buf, 1, pCTR->update(buf, (uint8_t *)buf + fread(buf, 1, BUFSIZE, ifp), buf) - (uint8_t *)buf, ofp))
			;
	} else if ((rec & REC_ENC) != 0) {
		size_t read;
		uint8_t src[BUFSIZE], dst[BUFSIZE + 16];
		while ((read = fread(src, 1, BUFSIZE, ifp)) == BUFSIZE)
			fwrite(dst, 1, pEnc->update(src, (uint8_t *)src + BUFSIZE, dst, false) - (uint8_t *)dst, ofp);
		fwrite(dst, 1, pEnc->update(src, (uint8_t *)src + read, dst, true) - (uint8_t *)dst, ofp);
	} else if ((rec & REC_DEC) != 0) {
		size_t read;
		uint8_t src[BUFSIZE], dst[BUFSIZE + 16];
		while ((read = fread(src, 1, BUFSIZE, ifp)) == BUFSIZE)
			fwrite(dst, 1, pDec->update(src, (uint8_t *)src + BUFSIZE, dst, false) - (uint8_t *)dst, ofp);
		fwrite(dst, 1, pDec->update(src, (uint8_t *)src + read, dst, true) - (uint8_t *)dst, ofp);
	}
	fclose(ifp);
	fclose(ofp);
	delete pEnc;
	delete pDec;
	delete pCTR;
	return 0;
}
