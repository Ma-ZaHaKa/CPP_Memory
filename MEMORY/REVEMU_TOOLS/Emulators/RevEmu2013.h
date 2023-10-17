#pragma once
#include "Windows.h"
#include <iostream>
#include "RevSpoofer.h"
#include "..\Cryptography\CRijndael.h"
#include "..\Cryptography\SHA.h"
#include "..\Cryptography\MessageDigest.h"


int _SIZE_REVEMU2013 = 168 + 8; // 8 for steamid
int SIZE_REVEMU2013 = 330; // pticket size new 330 ?? work it ничё не даёт нового

static void CreateRandomString(char* pszDest, int nLength)
{
	static const char c_szAlphaNum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < nLength; ++i)
		pszDest[i] = c_szAlphaNum[rand() % (sizeof(c_szAlphaNum) - 1)];

	pszDest[nLength] = '\0';
}


static int GenerateRevEmu2013(void* pDest, int nSteamID = 0, int* outRevHash = nullptr)
{
	char szhwid[33];

	for (int tries = 5; ; tries--)
	{
		CreateRandomString(szhwid, 32);
		std::cout << "\n!!!!!!!HWID: " << szhwid << "\n";
		if (!RevSpoof(szhwid, nSteamID) && !tries)
			return 0;
		else
			break;
	}

	memset(pDest, 0, SIZE_REVEMU2013);

	auto pTicket = (int*)pDest; //!!!!!
	auto pbTicket = (unsigned char*)pDest;

	auto revHash = RevHash(szhwid);

	if (revHash == 0)
		revHash = 0xDFDC1C3D;

	pTicket[0] = 'S';                      // +0
	pTicket[1] = revHash;                  // +4
	pTicket[2] = 'rev';                    // +8
	pTicket[3] = 0;                        // +12
	pTicket[4] = revHash << 1;             // +16
	pTicket[5] = 0x01100001;               // +20
	pTicket[6] = (int)_time64(0) + 90123;  // +24
	pbTicket[27] = ~(pbTicket[27] + pbTicket[24]);
	pTicket[7] = ~(int)_time64(0);         // +28
	pTicket[8] = revHash * 2 >> 3;         // +32
	pTicket[9] = 0;                        // +36

	/*pTicket[0] = 'U';                      // +0
	pTicket[1] = revHash;                  // +4
	pTicket[2] = 'rev';                    // +8
	pTicket[3] = 0;                        // +12
	pTicket[4] = revHash << 1;             // +16
	pTicket[5] = 0x01100001;               // +20
	pTicket[6] = (int)_time64(0) + 90123;  // +24
	pbTicket[27] = ~(pbTicket[27] + pbTicket[24]);
	pTicket[7] = ~(int)_time64(0);         // +28
	pTicket[8] = revHash * 2 >> 3;         // +32
	pTicket[9] = 0;                        // +36*/

	if (outRevHash)
		*outRevHash = revHash;

	//static const char c_szAESKeyRand[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
	static const char c_szAESKeyRand[] = "11111111111111111111111111111111";

	char szAESHashRand[32];
	auto AESRand = CRijndael();
	AESRand.MakeKey(c_szAESKeyRand, CRijndael::sm_chain0, 32, 32);
	AESRand.EncryptBlock(szhwid, szAESHashRand);
	memcpy(&pbTicket[40], szAESHashRand, 32);

	static const char c_szAESKeyRev[] = "_YOU_SERIOUSLY_NEED_TO_GET_LAID_";
	//static const char c_szAESKeyRev[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
	char AESHashRev[32];
	auto AESRev = CRijndael();
	AESRev.MakeKey(c_szAESKeyRev, CRijndael::sm_chain0, 32, 32);
	AESRev.EncryptBlock(c_szAESKeyRand, AESHashRev);
	memcpy(&pbTicket[72], AESHashRev, 32);

	char szSHAHash[32];
	auto sha = CSHA(CSHA::SHA256);
	sha.AddData(szhwid, 32);
	sha.FinalDigest(szSHAHash);
	memcpy(&pbTicket[104], szSHAHash, 32);



	static auto pzsSentimentMsg = "I hate birzyk and his emulators";
	//static auto pzsSentimentMsg = "hate birzyk and his emulators";
	memcpy((char*)&pbTicket[136], pzsSentimentMsg, strlen(pzsSentimentMsg) + 1);

	/*int i = 0;
	for (i = 136; i < 167; i++)
		pbTicket[i] = 0;
	pbTicket[168] = 32;
	for (i = 169; i < 171; i++)
		pbTicket[i] = 0;
	pbTicket[172] = 3;
	pbTicket[176] = 14;
	pbTicket[180] = 66;
	pbTicket[181] = 90;
	pbTicket[182] = 104;
	pbTicket[183] = 49;
	pbTicket[184] = 23;
	pbTicket[185] = 114;
	pbTicket[186] = 69;
	pbTicket[187] = 56;
	pbTicket[188] = 80;
	pbTicket[189] = 144;*/


	return SIZE_REVEMU2013;
}

static int TestGenerateRevEmu2013(void* pDest, int nSteamID = 0, int* outRevHash = nullptr)
{
	char szhwid[33];
	// найти строку которая сработает 
	for (int tries = 5; ; tries--)
	{
		CreateRandomString(szhwid, 32);
		if (!RevSpoof(szhwid, nSteamID) && !tries)
			return 0;
		else
			break;
	}

	memset(pDest, 0, SIZE_REVEMU2013);

	auto pTicket = (int*)pDest; //!!!!!
	auto pbTicket = (unsigned char*)pDest;

	auto revHash = RevHash(szhwid);

	if (revHash == 0)
		revHash = 0xDFDC1C3D;

	/*pTicket[0] = 'S';                      // +0
	pTicket[1] = revHash;                  // +4
	pTicket[2] = 'rev';                    // +8
	pTicket[3] = 0;                        // +12
	pTicket[4] = revHash << 1;             // +16
	pTicket[5] = 0x01100001;               // +20
	pTicket[6] = (int)_time64(0) + 90123;  // +24
	pbTicket[27] = ~(pbTicket[27] + pbTicket[24]);
	pTicket[7] = ~(int)_time64(0);         // +28
	pTicket[8] = revHash * 2 >> 3;         // +32
	pTicket[9] = 0;                        // +36*/

	pTicket[0] = 'U';                      // +0
	pTicket[1] = revHash;                  // +4
	pTicket[2] = 'rev';                    // +8
	pTicket[3] = 0;                        // +12
	pTicket[4] = revHash << 1;             // +16
	pTicket[5] = 0x01100001;               // +20
	pTicket[6] = (int)_time64(0) + 90123;  // +24
	pbTicket[27] = ~(pbTicket[27] + pbTicket[24]);
	pTicket[7] = ~(int)_time64(0);         // +28
	pTicket[8] = revHash * 2 >> 3;         // +32
	pTicket[9] = 0;                        // +36

	if (outRevHash)
		*outRevHash = revHash;

	static const char c_szAESKeyRand[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
	//static const char c_szAESKeyRand[] = "11111111111111111111111111111111";

	char szAESHashRand[32];
	auto AESRand = CRijndael();
	AESRand.MakeKey(c_szAESKeyRand, CRijndael::sm_chain0, 32, 32);
	AESRand.EncryptBlock(szhwid, szAESHashRand);
	memcpy(&pbTicket[40], szAESHashRand, 32);

	static const char c_szAESKeyRev[] = "_YOU_SERIOUSLY_NEED_TO_GET_LAID_";
	//static const char c_szAESKeyRev[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
	char AESHashRev[32];
	auto AESRev = CRijndael();
	AESRev.MakeKey(c_szAESKeyRev, CRijndael::sm_chain0, 32, 32);
	AESRev.EncryptBlock(c_szAESKeyRand, AESHashRev);
	memcpy(&pbTicket[72], AESHashRev, 32);

	char szSHAHash[32];
	auto sha = CSHA(CSHA::SHA256);
	sha.AddData(szhwid, 32);
	sha.FinalDigest(szSHAHash);
	memcpy(&pbTicket[104], szSHAHash, 32);



	/*static auto pzsSentimentMsg = "I hate birzyk and his emulators";
	//static auto pzsSentimentMsg = "hate birzyk and his emulators";
	memcpy((char*)&pbTicket[136], pzsSentimentMsg, strlen(pzsSentimentMsg) + 1);*/

	//int offset_parent = 8; //!! transponse data 8

	int i = 0; //  136 -  167  (144->175 => 0)
	for (i = 136; i < 168; i++) { pbTicket[i] = 0; } // 2 пустые строки

	//!! 176 - 261inc https://4exchange.ru/job/index.php
	//176->261 => UNK_TICKET SHIT PHP SRV SIZE=86  ticket field 96 - 86 = 10 (zero byte)


	//for (i = 168; i < 254; i++) { pbTicket[i] = 0; } // <== KEY ENCODED FROM SRV
	for (i = 254; i < 264; i++) { pbTicket[i] = 0; } // добить серверный-ключ-строку нулями


	for (i = 264; i < 296; i++) { pbTicket[i] = 0; } // 2 пустые строки

	//------------- 2 строки конца билета (статика)
	pbTicket[296] = 32; //0x20
	pbTicket[297] = 0;
	pbTicket[298] = 0;
	pbTicket[299] = 0;
	pbTicket[300] = 3; // 0x03
	pbTicket[301] = 0;
	pbTicket[302] = 0;
	pbTicket[303] = 0;
	pbTicket[304] = 14; // 0x0e
	pbTicket[305] = 0;
	pbTicket[306] = 0;
	pbTicket[307] = 0;
	pbTicket[308] = 66; // 0x42
	pbTicket[309] = 90; // 0x5a
	pbTicket[310] = 104; // 0x68
	pbTicket[311] = 49; // 0x31
	pbTicket[312] = 23; // 0x17
	pbTicket[313] = 114; // 0x72
	pbTicket[314] = 69; // 0x45
	pbTicket[315] = 56; // 0x38
	pbTicket[316] = 80; // 0x50
	pbTicket[317] = 144; // 0x90
	pbTicket[318] = 0;
	pbTicket[319] = 0;
	pbTicket[320] = 0;
	pbTicket[321] = 0;
	//pbTicket[321] = 255; // testsing call this func flag //WORK

	// конец 321 при длине 330 бо входной указатель смещён на 8
	// SIZE 330 MAX INDEX 330 - 8 pdata = 321
	return SIZE_REVEMU2013;
}

/*pbTicket[168] = 32;
for (i = 169; i < 171; i++) { pbTicket[i] = 0; }
pbTicket[172] = 3;
pbTicket[176] = 14;
pbTicket[180] = 66;
pbTicket[181] = 90;
pbTicket[182] = 104;
pbTicket[183] = 49;
pbTicket[184] = 23;
pbTicket[185] = 114;
pbTicket[186] = 69;
pbTicket[187] = 56;
pbTicket[188] = 80;
pbTicket[189] = 144;*/