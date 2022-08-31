#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cipher/aes.hpp"
#include "Cipher/sm4.hpp"
#define BUFSIZE 1024
#define REC_ERR 1
#define REC_OFP 2
#define REC_IFP 4
#define REC_KEY 8
#define REC_ENC 16
#define REC_DEC 32
#define REC_CTR 64
#define REC_128 128
#define REC_256 256
#define REC_192 512
#define REC_SM4 1024
template <typename SCF>
void scrypt(SCF scf, FILE *ifp, FILE *ofp) {
	uint8_t buf[BUFSIZE];
	while (fwrite(buf, 1, scf.update(buf, (uint8_t *)buf + fread(buf, 1, BUFSIZE, ifp), buf) - (uint8_t *)buf, ofp) == BUFSIZE) {}
}
template <typename BCF>
void bcrypt(BCF bcf, FILE *ifp, FILE *ofp) {
	uint8_t src[BUFSIZE], dst[BUFSIZE + 16];
	size_t read;
	while ((read = fread(src, 1, BUFSIZE, ifp)) == BUFSIZE) {
		fwrite(dst, 1, bcf.update(src, (uint8_t *)src + BUFSIZE, dst, false) - (uint8_t *)dst, ofp);
	}
	fwrite(dst, 1, bcf.update(src, (uint8_t *)src + read, dst, true) - (uint8_t *)dst, ofp);
}
template <typename T>
void choose(int rec, FILE *ifp, FILE *ofp, uint8_t *iv, uint8_t *key) {
	if ((rec & REC_CTR) != 0) {
		scrypt(CTRCrypter<T>(iv, key), ifp, ofp);
	} else if ((rec & REC_ENC) != 0) {
		bcrypt(Encrypter<T>(key), ifp, ofp);
	} else if ((rec & REC_DEC) != 0) {
		bcrypt(Decrypter<T>(key), ifp, ofp);
	}
}
int main(int argc, char *argv[]) {
	int rec = 0;
	FILE *ifp = stdin, *ofp = stdout;
	uint8_t key[32] = {}, civ[16] = {};
	for (int i = 1; (rec & REC_ERR) == 0 && i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'k' && argv[i][2] == '\0') {
				if ((rec & REC_KEY) == 0 && i + 1 < argc) {
					strncpy((char *)key, argv[++i], 32);
					rec |= REC_KEY;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == 'c' && argv[i][2] == '\0') {
				if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0 && i + 1 < argc) {
					strncpy((char *)civ, argv[++i], 16);
					rec |= REC_CTR;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == 'd' && argv[i][2] == '\0') {
				if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0) {
					rec |= REC_DEC;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == 'e' && argv[i][2] == '\0') {
				if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0) {
					rec |= REC_ENC;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == 'S' && argv[i][2] == '\0') {
				if ((rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0) {
					rec |= REC_SM4;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == '4' && argv[i][2] == '\0') {
				if ((rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0) {
					rec |= REC_128;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == '6' && argv[i][2] == '\0') {
				if ((rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0) {
					rec |= REC_192;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == '8' && argv[i][2] == '\0') {
				if ((rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0) {
					rec |= REC_256;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == 'i' && argv[i][2] == '\0') {
				if ((rec & REC_IFP) == 0 && i + 1 < argc && (ifp = fopen(argv[++i], "rb"))) {
					rec |= REC_IFP;
				} else {
					rec |= REC_ERR;
				}
			} else if (argv[i][1] == 'o' && argv[i][2] == '\0') {
				if ((rec & REC_OFP) == 0 && i + 1 < argc && (ofp = fopen(argv[++i], "wb"))) {
					rec |= REC_OFP;
				} else {
					rec |= REC_ERR;
				}
			} else {
				rec |= REC_ERR;
			}
		} else {
			rec |= REC_ERR;
		}
	}
	if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0 || (rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0 || (rec & REC_KEY) == 0 || (rec & REC_ERR) != 0) {
		fprintf(stderr, "usage: %s [-i INFILE] [-o OUTFILE] (-c IV | -e | -d) (-S | -4 | -6 | -8) -k KEY\n", argv[0]);
		if ((rec & REC_IFP) != 0) {
			fclose(ifp);
		}
		if ((rec & REC_OFP) != 0) {
			fclose(ofp);
		}
		return 1;
	}
	if ((rec & REC_SM4) != 0) {
		choose<SM4>(rec, ifp, ofp, civ, key);
	} else if ((rec & REC_128) != 0) {
		choose<AES128>(rec, ifp, ofp, civ, key);
	} else if ((rec & REC_192) != 0) {
		choose<AES192>(rec, ifp, ofp, civ, key);
	} else if ((rec & REC_256) != 0) {
		choose<AES256>(rec, ifp, ofp, civ, key);
	}
	if ((rec & REC_IFP) != 0) {
		fclose(ifp);
	}
	if ((rec & REC_OFP) != 0) {
		fclose(ofp);
	}
	return 0;
}
