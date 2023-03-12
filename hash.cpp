#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Hash/md5.hpp"
#include "Hash/sm3.hpp"
#define BUFSIZE 1024
int main(int argc, char *argv[]) {
	Hasher<MD5> md5;
	Hasher<SM3> sm3;
	uint8_t buf[BUFSIZE];
	size_t read;
	while ((read = fread(buf, 1, BUFSIZE, stdin)) == BUFSIZE) {
		md5.update(buf, (uint8_t *)buf + BUFSIZE);
		sm3.update(buf, (uint8_t *)buf + BUFSIZE);
	}
	md5.update(buf, (uint8_t *)buf + read);
	sm3.update(buf, (uint8_t *)buf + read);
	uint8_t md5sum[16];
	uint8_t sm3sum[32];
	md5.digest(md5sum);
	sm3.digest(sm3sum);
	printf("MD5SUM: ");
	for (int i = 0; i < 16; i++) {
		printf("%02x", md5sum[i]);
	}
	printf("\n");
	printf("SM3SUM: ");
	for (int i = 0; i < 32; i++) {
		printf("%02x", sm3sum[i]);
	}
	printf("\n");
	return 0;
}
