#include <iostream>
#include "strumok.h"

int main()
{
	
	// Tests 
	vector<uint64_t> keyVec{ 0x8000000000000000, 0, 0, 0, 0, 0, 0, 0};
	vector<uint64_t> IV{ 0x0000000000000004,0x0000000000000003,0x0000000000000002, 0x0000000000000001 };

	StrumokCipher cipher(StrumokCipher::Mode::k512Bit); 
	cipher.Init(keyVec, IV);

	for (int index = 0; index < 7; index++)
	{
		auto res = cipher.Strm();
		cout << std::hex << res << endl;
		cipher.Next();
	}

}