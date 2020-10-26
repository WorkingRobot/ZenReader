#pragma once

#include "../Structs/FAESKey.h"

#include <wmmintrin.h>

namespace Zen::Helpers {
	class AES {
	public:
		struct KeySchedule {
			__m128i Key;
			__m128i Steps[14];
		};

		static __forceinline KeySchedule LoadKey(const Structs::FAESKey& Key) {
			__m128i EncStep1 = _mm_loadu_si128((const __m128i*) Key.Key);
			__m128i EncStep2 = _mm_loadu_si128((const __m128i*) (Key.Key + 16));

			KeySchedule Schedule;

#define KEYEXP128_H(K1, K2, I, S) _mm_xor_si128(aes128_keyexpand(K1), _mm_shuffle_epi32(_mm_aeskeygenassist_si128(K2, I), S))
#define KEYEXP256(K1, K2, I) KEYEXP128_H(K1, K2, I, 0xFF)
#define KEYEXP256_2(K1, K2)  KEYEXP128_H(K1, K2, 0x00, 0xAA)

#define RUN_STEP(I, TopInd) \
		EncStep1 = KEYEXP256(EncStep1, EncStep2, I); \
		EncStep2 = KEYEXP256_2(EncStep2, EncStep1); \
		Schedule.Steps[TopInd] = _mm_aesimc_si128(EncStep1); \
		Schedule.Steps[TopInd - 1] = _mm_aesimc_si128(EncStep2);

			Schedule.Key = EncStep1;
			Schedule.Steps[13] = _mm_aesimc_si128(EncStep2);
			RUN_STEP(0x01, 12);
			RUN_STEP(0x02, 10);
			RUN_STEP(0x04, 8);
			RUN_STEP(0x08, 6);
			RUN_STEP(0x10, 4);
			RUN_STEP(0x20, 2);
			Schedule.Steps[0] = KEYEXP256(EncStep1, EncStep2, 0x40);

#undef RUN_STEP
#undef KEYEXP256
#undef KEYEXP256_2
#undef KEYEXP128_H

			return Schedule;
		}

		static __forceinline void Decode(const KeySchedule& Schedule, const char Input[16], char Output[16]) {
			__m128i m = _mm_loadu_si128((__m128i*) Input);

			m = _mm_xor_si128(m, Schedule.Steps[0]);
			m = _mm_aesdec_si128(m, Schedule.Steps[1]);
			m = _mm_aesdec_si128(m, Schedule.Steps[2]);
			m = _mm_aesdec_si128(m, Schedule.Steps[3]);
			m = _mm_aesdec_si128(m, Schedule.Steps[4]);
			m = _mm_aesdec_si128(m, Schedule.Steps[5]);
			m = _mm_aesdec_si128(m, Schedule.Steps[6]);
			m = _mm_aesdec_si128(m, Schedule.Steps[7]);
			m = _mm_aesdec_si128(m, Schedule.Steps[8]);
			m = _mm_aesdec_si128(m, Schedule.Steps[9]);
			m = _mm_aesdec_si128(m, Schedule.Steps[10]);
			m = _mm_aesdec_si128(m, Schedule.Steps[11]);
			m = _mm_aesdec_si128(m, Schedule.Steps[12]);
			m = _mm_aesdec_si128(m, Schedule.Steps[13]);

			m = _mm_aesdeclast_si128(m, Schedule.Key);

			_mm_storeu_si128((__m128i*) Output, m);
		}

		static __forceinline void DecodeInPlace(const Structs::FAESKey& Key, char* Buffer, size_t BufferSize) {
			KeySchedule Schedule = LoadKey(Key);

			for (; BufferSize >= 16; BufferSize -= 16, Buffer += 16) {
				Decode(Schedule, Buffer, Buffer);
			}

			if (BufferSize) {
				char Data[16];
				memcpy(Data, Buffer, BufferSize);
				memset(Data + BufferSize, 0, 16 - BufferSize);
				Decode(Schedule, Data, Data);
				memcpy(Buffer, Data, BufferSize);
			}
		}

	private:
		static __forceinline __m128i aes128_keyexpand(__m128i key)
		{
			key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
			key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
			return _mm_xor_si128(key, _mm_slli_si128(key, 4));
		}
	};
}