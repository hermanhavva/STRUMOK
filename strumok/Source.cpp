#include <iostream>
#include <unordered_map>
#include "strumok.h"
#include "tests.hpp"


int main()
{

	Tester tester;
	if (!tester.TestAll())
	{
		cerr << "Tests failed";
		return -1;
	}

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

	for (int t = 0; t <= 31; t++)
	{
		int64_t result = cipher.Strm();

		auto curState = cipher.GetCurState(); 
		for (int index = 0; index < curState.linearRegisters.size(); index++)
		{
			map[format("S_{}", t + index)] = curState.linearRegisters[index];
		}
		map[format("Z_{}", t)] = result;
		map[format("R_{}", t)] = curState.r2;

		Z[t] = map[format("Z_{}", t)];
		cout << format("Z_{} ", t) << map[format("Z_{}", t)] << endl;

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


	// ###############################
	// trying to find what is possible 

	// known: S_22, S_19, S_20, R_6, S_18, R_5, S_17
	// ############################### 
	// Calculated: S_6, R_9, R_7, S_5, R_8, S_7, S_21, R_11, R_10, R_13, S_16, S_23, R_12, S_24, S_25, S_8, S_10, S_26, R_14, R_15, S_11, S_27, R_16, S_12, S_28, R_17, S_13, S_29, R_18
	
	// S[6], S[7], S[8], S[9], S[10], S[11], S[12], S[13], S[14], S[15], S[16], S[17], S[18], S[19], S[20], S[21], S[22], S[23], S[24], S[25], S[26], S[27], S[28], S[29]


	// S22 = S6 * alpha xor (S17 * alpha_inv) xor S19
	S[6] = StrumokCipher::ainv_mul(S[22] ^ S[19] ^ (StrumokCipher::ainv_mul(S[17])));
	


	// R_2_t+2 = T(R_2_t + S_13+t) 
	R[7] = StrumokCipher::transform_T(R[5] + S[18]);
	R[8] = StrumokCipher::transform_T(R[6] + S[19]);
	R[9] = StrumokCipher::transform_T(R[7] + S[20]);
	R[11] = StrumokCipher::transform_T(R[9] + S[22]);



	// Z_6 = FSM(S_21, R_1_6, R_2_6) ^ S_6; R_1_6 = R_2_5 + S_18 
	S[21] = (Z[6] ^ S[6] ^ R[6]) - (R[5] + S[18]); 

	R[8] = StrumokCipher::transform_T(R[6] + S[19]);

	// R_1_7 = R_2_6 + S_19
	S[7] = Z[7] ^ StrumokCipher::FSM(S[22], R[6] + S[19], R[7]); 

	// R_2_t+2 = T(R_2_t + S_13+t) | R_2_10 = T(R_2_8 + S[21])
	R[10] = StrumokCipher::transform_T(R[8] + S[21]);

	// use standart formula for calculating S_15 on the tact t
	S[23] = StrumokCipher::a_mul(S[7]) ^ StrumokCipher::ainv_mul(S[18]) ^ S[20];

	// Z_8 = FSM(S_23, R_1_8, R_2_8) ^ S_8; R_1_8 = R_2_7 + S_20
	S[8] = Z[8] ^ StrumokCipher::FSM(S[23], R[7] + S[20], R[8]);

	// R_2_t+2 = T(R_2_t + S_13+t)
	R[12] = StrumokCipher::transform_T(R[10] + S[23]);

	// use standart formula for calculating S_15 on the tact t
	S[24] = StrumokCipher::a_mul(S[8]) ^ StrumokCipher::ainv_mul(S[19]) ^ S[21];


	// Z_9 = FSM(S_24, R_1_9, R_2_9) ^ S_9; 
	S[9] = Z[9] ^ StrumokCipher::FSM(S[24], R[8] + S[21], R[9]);   
	
	// R_2_t+2 = T(R_2_t + S_13+t) 
	R[13] = StrumokCipher::transform_T(R[11] + S[24]); 

	// use standart formula for calculating S_15 on the tact t
	S[25] = StrumokCipher::a_mul(S[9]) ^ StrumokCipher::ainv_mul(S[20]) ^ S[22];

	// Z_10 = S_10 ^ FSM(S_25, R_1_10, R_2_10)
	S[10] = Z[10] ^ StrumokCipher::FSM(S[25], R[9] + S[22], R[10]);

	// use standart formula for calculating S_15 on the tact t
	S[26] = StrumokCipher::a_mul(S[10]) ^ StrumokCipher::ainv_mul(S[21]) ^ S[23];
	
	// ##############################################
	// Now the graph is fininshed, lets find the possible R`s: 
	// ##############################################

	// R_2_t+2 = T(R_2_t + S_13+t)
	R[14] = StrumokCipher::transform_T(R[12] + S[25]);
	R[15] = StrumokCipher::transform_T(R[13] + S[26]);

	// We want to find next value: S_27, for it we need S_11, S_22, S_24 
	// We do not yet know the S_11, it can be found with Z_11, S_26 (they are known)
	// Z_11 = FSM(S_26, R_1_11, R_2_11) ^ S_11; 
	S[11] = Z[11] ^ StrumokCipher::FSM(S[26], R[10] + S[23], R[11]);
	S[27] = StrumokCipher::a_mul(S[11]) ^ StrumokCipher::ainv_mul(S[22]) ^ S[24];

	// Now we can find R again
	R[16] = StrumokCipher::transform_T(R[14] + S[27]);

	// We want to find next value: S_28, for it we need S_12, S_23, S_25 
	// We do not yet know the S_12, it can be found with Z_12, S_27 (they are known)
	// Z_12 = FSM(S_27, R_1_12, R_2_12) ^ S_12; 
	S[12] = Z[12] ^ StrumokCipher::FSM(S[27], R[11] + S[24], R[12]);
	S[28] = StrumokCipher::a_mul(S[12]) ^ StrumokCipher::ainv_mul(S[23]) ^ S[25];

	// Now we can find R again
	R[17] = StrumokCipher::transform_T(R[15] + S[28]);

	// We want to find next value: S_29, for it we need S_13, S_24, S_26 
	// We do not yet know the S_13, it can be found with Z_13, S_28 (they are known)
	// Z_13 = FSM(S_28, R_1_13, R_2_13) ^ S_13;
	S[13] = Z[13] ^ StrumokCipher::FSM(S[28], R[12] + S[25], R[13]);
	S[29] = StrumokCipher::a_mul(S[13]) ^ StrumokCipher::ainv_mul(S[24]) ^ S[26];

	// Also lets find S_14, S_30, S_15, S_31, S_16 
	S[14] = Z[14] ^ StrumokCipher::FSM(S[29], R[13] + S[26], R[14]);
	S[30] = StrumokCipher::a_mul(S[14]) ^ StrumokCipher::ainv_mul(S[25]) ^ S[27];
	
	S[15] = Z[15] ^ StrumokCipher::FSM(S[30], R[14] + S[27], R[15]); 
	
	S[31] = StrumokCipher::a_mul(S[15]) ^ StrumokCipher::ainv_mul(S[26]) ^ S[28];
	S[16] = Z[16] ^ StrumokCipher::FSM(S[31], R[15] + S[28], R[16]); 


	// BINGO, we are done 
	
	// Verify S
	for (int i = 6; i <= 31; i++)
	{
		if (map[format("S_{}", i)] == S[i])
			cout << "S value " << std::dec << i << " guessed" << endl;
		else
			cout << "S value " << std::dec << i << " not guessed" << endl;
	}
	for (int i = 5; i <= 17; i++)
	{
		if (map[format("R_{}", i)] == R[i])
			cout << "R value " << std::dec << i << " guessed" << endl;
		else
			cout << "R value " << std::dec << i << " not guessed" << endl;
	}

 }