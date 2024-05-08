#include <iostream>
#include <bitset>
#include <vector>
using namespace std;

static int sbox[16] = { 0xc,0x5,0x6,0xb,0x9,0x0,0xa,0xd,0x3,0xe,0xf,0x8,0x4,0x7,0x1,0x2 };

void generateRoundKeys(bitset<80>& k, int round) {

	//第一步，行移位
	bitset<61> temp;
	for (int i = 0; i < 61; i++) {
		temp[i] = k[i + 19];
	}
	for (int i = 0; i < 19; i++) {
		k[i + 61] = k[i];
	}
	for (int i = 0; i < 61; i++) {
		k[i] = temp[i];
	}
	//第二步，s盒
	int a = k[79] * 8 + k[78] * 4 + k[77] * 2 + k[76];
	a = sbox[a];
	for (int i = 0; i < 4; i++) {
		k[76 + i] = a & 1;
		a = a / 2;
	}
	//第三步，异或
	a = k[19] * 16 + k[18] * 8 + k[17] * 4 + k[16] * 2 + k[15];
	a = (a ^ round);
	for (int i = 0; i < 5; i++) {
		k[15 + i] = a & 1;
		a = a / 2;
	}
}

void addRoundKey(bitset<64>& f, bitset<80> k) {
	for (int i = 0; i < 64; i++) {
		f[63 - i] = (f[63 - i] ^ k[79 - i]);
	}
}

void sBoxLayer(bitset<64>& f) {
	for (int i = 0; i < 16; i++) {
		int a = f[i * 4 + 3] * 8 + f[i * 4 + 2] * 4 + f[i * 4 + 1] * 2 + f[i * 4];
		a = sbox[a];
		f[i * 4] = a & 0b0001;
		f[i * 4 + 1] = a & 0b0010;
		f[i * 4 + 2] = a & 0b0100;
		f[i * 4 + 3] = a & 0b1000;
	}
}

void pLayer(bitset<64>& f) {

	bitset<64> temp;
	for (int i = 0; i < 63; i++) {
		temp[i * 16 % 63] = f[i];
	}
	temp[63] = f[63];
	f = temp;

}

void encrypt(char* flag, char* key, int flaglen) {

	// padding mode: pkcs5
	int pad = flaglen % 8 == 0 ? 8 : 8 - flaglen % 8;
	for (int i = 0; i < pad; i++) {
		flag[flaglen + i] = char(pad);
	}
	flaglen += pad;
	flag[flaglen] = '\0';

	// flag str to bit
	vector<bitset<64>> bit_flag;
	int bit_flag_count = flaglen / 8;
	for (int i = 0; i < bit_flag_count; i++) {
		bitset<64> temp;
		for (int j = 0; j < 8; j++) {
			bitset<8> temp1(flag[(i + 1) * 8 - 1 - j]);
			for (int k = 0; k < 8; k++) {
				temp[j * 8 + k] = temp1[k];
			}
		}
		bit_flag.push_back(temp);
	}

	// key str to bit
	bitset<80> bit_key;
	for (int i = 0; i < 10; i++) {
		bitset<8> temp(key[9 - i]);
		for (int j = 0; j < 8; j++) {
			bit_key[i * 8 + j] = temp[j];
		}
	}

	// encrypt
	for (int i = 0; i < bit_flag_count; i++) {
		bitset<64> flagi = bit_flag[i];
		bitset<80> keyi = bit_key;
		for (int j = 1; j <= 31; j++) {
			addRoundKey(flagi, keyi);
			sBoxLayer(flagi);
			pLayer(flagi);
			generateRoundKeys(keyi, j);
		}
		addRoundKey(flagi, keyi);
		bit_flag[i] = flagi;
	}

	for (int i = 0; i < bit_flag_count; i++) {
		for (int j = 7; j >= 0; j--) {
			int a = 0;
			for (int k = 0; k < 8; k++) {
				a += (bit_flag[i][j * 8 + k] << k);
			}
			printf("%02x", a);
		}
	}
}
int main() {
	char flag[256] = "";
	cin >> flag;
	int flaglen = strlen(flag);
	char key[81] = "TNESERPUQH";    //your key

	encrypt(flag, key, flaglen);
	system("pause");
	return 0;
}
/*
eg.
flag{657af618-0c7e-11ef-920a-f4b3011954e6}
ba41c43b3f1810810645a64fd72691d0c597d96cf027c1372a1a5d8a8481576dbf8376663383d093dce1cf36c3e8156a
*/
