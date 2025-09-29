#pragma once
#include <vector>
#include <gmpxx.h>
namespace liao::Math
{
	using GmpBigNumber = mpz_class;
	class PrimeNumberConvert {
	public:
		static unsigned int generate(unsigned int number, unsigned int digits = 0);
		static void generate(std::vector<unsigned int>& data, unsigned int digits = 0);
		static void generate_big(GmpBigNumber& number, unsigned int digits = 0);
		static bool is_prime(const GmpBigNumber& number);
	};
	static constexpr bool isLittleEndian()
	{
		union
		{
			uint32_t i;
			uint8_t c[4];
		} test = { 0x01020304 };
		return test.c[0] == 0x04; // 小端序：最低有效字节在最低地址
	}

    static constexpr bool isBigEndian()
	{
		union
		{
			uint32_t i;
			uint8_t c[4];
		} test = { 0x01020304 };

		return test.c[0] == 0x01; // 大端序：最高有效字节在最低地址
	}

}
