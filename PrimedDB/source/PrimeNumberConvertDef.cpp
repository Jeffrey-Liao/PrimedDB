#include <PrimeNumberConvert.h>
namespace liao::Math
{
	unsigned int PrimeNumberConvert::generate(unsigned int number, unsigned int digits)
	{
        GmpBigNumber candidate;

        if (digits != 0)
        {
            mpz_ui_pow_ui(candidate.get_mpz_t(), 2, digits );  // 2^(target_bits-1)
        }
        candidate *= number;
        //如果数字为奇数，则加1
        if (mpz_even_p(candidate.get_mpz_t())) {
            ++candidate;
        }
        mpz_nextprime(candidate.get_mpz_t(), candidate.get_mpz_t());
        //如果转化出来的数据小于给定位数。
        while (mpz_sizeinbase(candidate.get_mpz_t(), 2) < digits) {
            candidate += 2;
            mpz_nextprime(candidate.get_mpz_t(), candidate.get_mpz_t());
        }
        if (candidate.fits_uint_p())
            return candidate.get_ui();
        else
            return 0;
	}
	void PrimeNumberConvert::generate(std::vector<unsigned int>& data, unsigned int digits)
	{
        for(int n =0; n < data.size(); n++)
        {
            data[n] = generate(data[n], digits);
		}
	}
    void PrimeNumberConvert::generate_big(GmpBigNumber& number, unsigned int digits)
    {
        GmpBigNumber enlarge;

        if (digits != 0)
        {
            mpz_ui_pow_ui(enlarge.get_mpz_t(), 2, digits);// 2^(target_bits)
        }
        number *= enlarge;
        if (mpz_even_p(number.get_mpz_t())) {
            ++number;
        }
        mpz_nextprime(number.get_mpz_t(), number.get_mpz_t());
        while (mpz_sizeinbase(number.get_mpz_t(), 2) < digits) {
            number += 2;
            mpz_nextprime(number.get_mpz_t(), number.get_mpz_t());
        }
    }
    bool PrimeNumberConvert::is_prime(const GmpBigNumber& number)
    {
        int reps = 25;
        int result = mpz_probab_prime_p(number.get_mpz_t(), reps);
        return (result == 2);
	}

    GmpBigNumber PrimeNumberConvert::generateFromString(const std::string& str)
	{
        const char* origin = str.c_str();
        int byteSize = str.size();
        Math::GmpBigNumber convert, recordNumber = 1;
        unsigned m = 0;
        for (unsigned n = 0; n < byteSize; m += 8)
        {
            unsigned size = byteSize - n;
            if (size < 4)
            {
                if (size == 1)
                    convert = static_cast<unsigned char>(*(origin + n));
                else if (size == 2)
                    convert = static_cast<unsigned short>(*((unsigned short*)origin + n));
                else
                {
                    convert = (static_cast<uint32_t>(*(origin + n)) << 16) |
                        (static_cast<uint32_t>(*(origin + n + 1)) << 8) |
                        (static_cast<uint32_t>(*(origin + n + 2)) << 0);
                }
            }
            else
                convert = static_cast<unsigned int>(
                    *((unsigned int*)(origin + n)));
            Math::PrimeNumberConvert::generate_big(convert, 9);
            recordNumber *= convert;
        }
        return recordNumber;
	}
    GmpBigNumber PrimeNumberConvert::generateFromDouble(double value)
	{
        GmpBigNumber convert;
        return convert;
	}
}
