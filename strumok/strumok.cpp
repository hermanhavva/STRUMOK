#include "strumok.h"


StrumokCipher::StrumokCipher(const StrumokCipher::Mode mode) :
	curMode(mode) 
{
	curState = make_shared<IternalState>();
}

void StrumokCipher::Init(vector<uint64_t> key, vector<uint64_t> IV)
{
	if (ifInitialized)
	{
		throw runtime_error(format("Error at {}, the cipher is already initialized", __func__));
	}

	if (key.size() != static_cast<size_t>(curMode) || IV.size() != kIvLength)
	{
		throw runtime_error(format("Error at {}, wrong arguments length", __func__));
	}

	// Reverse the key and IV 
	reverse(key.begin(), key.end());
	reverse(IV.begin(), IV.end());

	// Init linear register

	if (curMode == Mode::k256Bit)
	{
		for (uint64_t index = 0; index < curState->linearRegisters.size(); index++)
		{
			uint64_t curRegisterValue = 0;

			switch (index)
			{
			case 0:
				curRegisterValue = key[3] ^ IV[0];
				break;
			case 1:
				curRegisterValue = key[2];
				break;
			case 2:
				curRegisterValue = key[1] ^ IV[1];
				break;
			case 3:
				curRegisterValue = key[0] ^ IV[2];
				break;
			case 4:
				curRegisterValue = key[3];
				break;
			case 5:
				curRegisterValue = key[2] ^ IV[3];
				break;
			case 6:
				curRegisterValue = ~key[1];
				break;
			case 7:
				curRegisterValue = ~key[0];
				break;
			case 8:
				curRegisterValue = key[3];
				break;
			case 9:
				curRegisterValue = key[2];
				break;
			case 10:
				curRegisterValue = ~key[1];
				break;
			case 11:
				curRegisterValue = key[0];
				break;
			case 12:
				curRegisterValue = key[3];
				break;
			case 13:
				curRegisterValue = ~key[2];
				break;
			case 14:
				curRegisterValue = key[1];
				break;
			case 15:
				curRegisterValue = ~key[0];
				break;
			default:
				break;
			}

			curState->linearRegisters[index] = curRegisterValue;
		}

	}
	else
	{
		for (uint64_t index = 0; index < curState->linearRegisters.size(); index++)
		{
			uint64_t curRegisterValue = 0;

			switch (index)
			{
			case 0:
				curRegisterValue = key[7] ^ IV[0];
				break;
			case 1:
				curRegisterValue = key[6];
				break;
			case 2:
				curRegisterValue = key[5];
				break;
			case 3:
				curRegisterValue = key[4] ^ IV[1];
				break;
			case 4:
				curRegisterValue = key[3];
				break;
			case 5:
				curRegisterValue = key[2] ^ IV[2];
				break;
			case 6:
				curRegisterValue = key[1];
				break;
			case 7:
				curRegisterValue = ~key[0];
				break;
			case 8:
				curRegisterValue = key[4] ^ IV[3];
				break;
			case 9:
				curRegisterValue = ~key[6];
				break;
			case 10:
				curRegisterValue = key[5];
				break;
			case 11:
				curRegisterValue = ~key[7];
				break;
			case 12:
				curRegisterValue = key[3];
				break;
			case 13:
				curRegisterValue = key[2];
				break;
			case 14:
				curRegisterValue = ~key[1];
				break;
			case 15:
				curRegisterValue = key[0];
				break;
			default:
				break;
			}

			curState->linearRegisters[index] = curRegisterValue;
		}
	}

	// Set r1, r2 to 0 
	curState->r1 = 0;
	curState->r2 = 0;

	// 32 Next tackts with init specified 
	for (uint64_t index = 0; index < 32; index++)
	{
		Next(NextMode::kInit);
	}

	Next();
	ifInitialized = true;
}

void StrumokCipher::Next(const StrumokCipher::NextMode nextMode /*= StrumokCipher::NextMode::kNormal*/) noexcept
{
	IternalState stateBeforeNext(*curState);

	curState->r2 = transform_T(stateBeforeNext.r1);
	curState->r1 = (stateBeforeNext.r2 + stateBeforeNext.linearRegisters[13])/* % kTwoToPower64*/;

	for (uint64_t index = 0; index < 15; index++)
	{
		curState->linearRegisters[index] = stateBeforeNext.linearRegisters[index + 1];
	}

	if (nextMode == NextMode::kNormal)
	{
		curState->linearRegisters[15] =
			a_mul(stateBeforeNext.linearRegisters[0]) ^
			ainv_mul(stateBeforeNext.linearRegisters[11]) ^
			stateBeforeNext.linearRegisters[13];

	}
	else
	{
		curState->linearRegisters[15] =
			FSM(stateBeforeNext.linearRegisters[15], stateBeforeNext.r1, stateBeforeNext.r2) ^
			a_mul(stateBeforeNext.linearRegisters[0]) ^
			ainv_mul(stateBeforeNext.linearRegisters[11]) ^
			stateBeforeNext.linearRegisters[13];
	}
}

uint64_t StrumokCipher::Strm() const noexcept
{
	return
		FSM(curState->linearRegisters[15], curState->r1, curState->r2) ^
		curState->linearRegisters[0];
} 




uint64_t StrumokCipher::FSM(uint64_t x, uint64_t y, uint64_t z)
{
	return ((x + y) /* % kTwoToPower64*/) ^ z;
}

uint64_t StrumokCipher::a_mul(const uint64_t x)
{
	return (((x) << 8) ^ (strumok_alpha_mul[x >> 56]));
}

uint64_t StrumokCipher::ainv_mul(const uint64_t x)
{
	return (((x) >> 8) ^ (strumok_alphainv_mul[x & 0xff]));
}

uint64_t StrumokCipher::transform_T(const uint64_t x)
{
	return ((strumok_T0[x & 0xff]) ^
		(strumok_T1[(x >> 8) & 0xff]) ^
		(strumok_T2[(x >> 16) & 0xff]) ^
		(strumok_T3[(x >> 24) & 0xff]) ^
		(strumok_T4[(x >> 32) & 0xff]) ^
		(strumok_T5[(x >> 40) & 0xff]) ^
		(strumok_T6[(x >> 48) & 0xff]) ^
		(strumok_T7[(x >> 56) & 0xff]));
}
