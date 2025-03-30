#include <iostream>
#include <unordered_map>
#include "strumok.h"


int main()
{
	// ##########
	// Tests 
	// ##########
	/*
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
	*/ 

	vector<uint64_t> keyVec
	{ 
		0x8000000000000000, 
		0x8000000000000000, 
		0x8000000000000000, 
		0x8000000000000000, 
		0x8000000000000000, 
		0x8000000000000000, 
		0x8000000000000000, 
		0x8000000000000000 
	};
	vector<uint64_t> IV
	{ 
		0x0000000000000004,
		0x0000000000000003,
		0x0000000000000002, 
		0x0000000000000001 
	};

	StrumokCipher cipher(StrumokCipher::Mode::k512Bit);
	vector<int64_t> R(100), S(100), Z(100);
	
	cipher.Init(keyVec, IV);

	// ######################
	// extract the red values
	
	unordered_map<string, int64_t> map;

	for (int t = 0; t <= 20; t++)
	{
		int64_t result = cipher.Strm();

		auto curState = cipher.GetCurState(); 
		for (int index = 0; index < curState.linearRegisters.size(); index++)
		{
			map[format("S_{}", t + index)] = curState.linearRegisters[index];
		}
		map[format("Z_{}", t)] = result;
		map[format("R_{}", t)] = curState.r2;

		cipher.Next();
	}

	// fill known values
	for (auto& index : vector<int>{22, 19, 20, 18, 17})
	{
		string curIndex = format("S_{}", index);
		S[index] = map[curIndex];
		cout << curIndex + " " << std::hex << map[curIndex] << endl;
	}

	for (auto& index : vector<int>{ 6, 5 })
	{
		string curIndex = format("R_{}", index);
		R[index] = map[curIndex];
		cout << curIndex + " " << std::hex << map[curIndex] << endl;
	}

	for (int i = 0; i <= 13; i++)
	{
		Z[i] = map[format("Z_{}", i)];
		cout << format("Z_{} ", i) << map[format("Z_{}", i)] << endl;
	} 
	

	// ###############################
	// trying to find what is possible 

	// known:
	// S_22, S_19, S_20, R_6, S_18, R_5, S_17,       S_6, R_9, R_7, S_5, R_8, S_7, S_21, R_11, S_16, S_23, R_12, S_24, S_8
	
	// S22 = S6 * alpha xor (S17 * alpha_inv) xor S19
	S[6] = StrumokCipher::ainv_mul(S[22] ^ S[19] ^ (StrumokCipher::ainv_mul(S[17])));
	
	// S_15 = R_6 - R_5 
	// S[15] = R[6] - R[5];

	
	R[9] = StrumokCipher::transform_T(R[6]);

	
	S[5] = Z[5] ^ StrumokCipher::FSM(S[20], R[5], R[6]); 

	R[7] = StrumokCipher::transform_T(R[5] + S[18]);

	// Z_6 = FSM(S_21, R_1_6, R_2_6) ^ S_6; R_1_6 = R_2_5 + S_18 
	S[21] = Z[6] ^ S[6] ^ R[6] - (R[5] + S[18]); 

	R[8] = StrumokCipher::transform_T(R[6] + S[19]);

	// R_1_7 = R_2_6 + S_19
	S[7] = Z[7] ^ StrumokCipher::FSM(S[22], R[6] + S[19], R[7]); 

	// R_2_t+2 = T(R_2_t + S_13+t) | R_2_10 = T(R_2_8 + S[21])
	R[10] = StrumokCipher::transform_T(R[8] + S[21]);

	R[11] = StrumokCipher::transform_T(R[9] + S[22]);
	
	// S_21 = a_mul(S_5) ^ ainv_mul(S_16) ^ S_18 
	S[16] = StrumokCipher::a_mul(S[21] ^ S[18] ^ StrumokCipher::a_mul(S[5]));

	// just use formula
	S[23] = StrumokCipher::a_mul(S[7]) ^ StrumokCipher::ainv_mul(S[18]) ^ S[20];

	// Z_8 = FSM(S_23, R_1_8, R_2_8) ^ S_8; R_1_8 = R_2_7 + S_20
	S[8] = Z[8] ^ StrumokCipher::FSM(S[23], R[7] + S[20], R[8]);

	// R_2_t+2 = T(R_2_t + S_13+t)
	R[12] = StrumokCipher::transform_T(R[10] + S[23]);

	// just use formula
	S[24] = StrumokCipher::a_mul(S[8]) ^ StrumokCipher::ainv_mul(S[19]) ^ S[8];


	// Z_9 = a_mul(S_9) ^ ainv_mul(S_20) ^ S_22 
	S[9] = StrumokCipher::ainv_mul((Z[9] ^ S[22] ^ StrumokCipher::ainv_mul(S[20]))); 


 }