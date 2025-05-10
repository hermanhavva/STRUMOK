#pragma once 

#include "strumok.h"

struct Tester
{
	bool TestAll() const
	{
		return Test1() && Test2();
	}


	bool Test1() const
	{
		bool flag = true;

		vector<uint64_t> keyVec
		{
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa
		};
		vector<uint64_t> IV
		{
			0x0000000000000000,
			0x0000000000000000,
			0x0000000000000000,
			0x0000000000000000
		};

		StrumokCipher cipher(StrumokCipher::Mode::k512Bit);
		cipher.Init(keyVec, IV);

		vector<unsigned long long> correctValues
		{
			0xd2a6103c50bd4e04, 0xdc6a21af5eb13b73,
			0xdf4ca6cb07797265, 0xf453c253d8d01876,
			0x039a64dc7a01800c, 0x688ce327dccb7e84,
			0x41e0250b5e526403, 0x9936e478aa200f22
		};

		for (int i = 0; i <= ROUNDS; i++)
		{
			uint64_t result = cipher.Strm();
			flag = result == correctValues[i];
			cipher.Next();
		}

		return flag;
	}

	bool Test2() const
	{
		bool flag = true;

		vector<uint64_t> keyVec
		{
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa,
			0xaaaaaaaaaaaaaaaa
		};
		vector<uint64_t> IV
		{
			0x0000000000000000,
			0x0000000000000000,
			0x0000000000000000,
			0x0000000000000000
		};

		StrumokCipher cipher(StrumokCipher::Mode::k256Bit);
		cipher.Init(keyVec, IV);

		vector<unsigned long long> correctValues
		{
			0xa7510b38c7a95d1d, 0xcd5ea28a15b8654f,
			0xc5e2e2771d0373b2, 0x98ae829686d5fcee,
			0x45bddf65c523dbb8, 0x32a93fcdd950001f,
			0x752a7fb588af8c51, 0x9de92736664212d4
		};

		for (int i = 0; i <= ROUNDS; i++)
		{
			uint64_t result = cipher.Strm();
			flag = result == correctValues[i];
			cipher.Next();
		}

		return flag;
	}

	const int ROUNDS = 7;
};


