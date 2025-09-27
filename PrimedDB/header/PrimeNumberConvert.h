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
    static constexpr bool isLittleEndian();

    static constexpr bool isBigEndian();

}
