#pragma once

#include <stdint.h>
#include <vector>
#include <format>
#include "strumok_tables.h"

using namespace std;


class StrumokCipher
{
public: 

	// ##############################################
	// # enum class Mode 
	// # needed to specify the Cipher mode in StrumokCipher constructor
	// ##############################################
	enum class Mode : uint64_t
	{
		k256Bit = 4,
		k512Bit = 8
	};

	// ##############################################
	// # enum class NextMode 
	// # needed to specify the mode of Next()
	// ##############################################
	enum class NextMode : uint64_t
	{
		kInit = 0,
		kNormal = 1
	};

	StrumokCipher(const Mode mode);

	// ##############################################
	// # void Init(key, IV); 
	// # key and IV are in Big Endian, (e.g. [0] is the most significant)
	// ##############################################
	void Init( vector<uint64_t> key, vector<uint64_t> IV);


	// ##############################################
	// # void Next(nextMode); 
	// # nextMode is an optional argument, default is StrumokCipher::NextMode::kNormal
	// ##############################################
	void Next(const NextMode nextMode = NextMode::kNormal) noexcept;
	

	// ##############################################
	// # uint64_t Strm(); 
	// # returns next gamma byte
	// ##############################################
	uint64_t Strm() const noexcept;

private:

	// ##############################################
	// # struct IternalState
	// # contains the linear register and FSM rigisters (r1 r2)
	// ##############################################
	struct IternalState
	{
		IternalState()
		{
			linearRegisters = vector<uint64_t>(16, 0);
			r1 = 0;
			r2 = 0;
		}
		IternalState(const IternalState& other) :
			linearRegisters(other.linearRegisters),
			r1(other.r1),
			r2(other.r2)
		{}

		vector<uint64_t>linearRegisters;
		uint64_t r1;
		uint64_t r2;
	};


	shared_ptr<IternalState>	 curState;
	Mode						 curMode;
	bool						 ifInitialized = false;
	const static inline uint64_t kIvLength = 4;

	static inline uint64_t FSM(uint64_t x, uint64_t y, uint64_t z);	

	static inline uint64_t a_mul(const uint64_t x);

	static inline uint64_t ainv_mul(const uint64_t x);

	static inline uint64_t transform_T(const uint64_t x);

};