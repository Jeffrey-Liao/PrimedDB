#include <config_int.h>

#include "Primize.h"
using namespace std;
namespace liao::Math
{
	Primize::Primize(std::unique_ptr<char[]> originData, unsigned int byteSize)
	{
		m_primedMemory = primize(originData, byteSize);
	}
	Primize::Primize(GmpBigNumber& originData)
		:m_primedMemory(std::move(originData))
	{}
	Primize::Primize(Primize&& object)
		:m_primedMemory(std::move(object.m_primedMemory))
	{}
	const GmpBigNumber& Primize::getPrimedNumber() const
	{
		return m_primedMemory;
	}
	std::unique_ptr<char[]> Primize::getOriginData()
	{
		return deprimize(m_primedMemory);
	}
	GmpBigNumber Primize::primize(std::unique_ptr<char[]>& originData, unsigned int byteSize)
	{
		char* origin = originData.get();
		GmpBigNumber number;
		mpz_import(number.get_mpz_t(), byteSize, isBigEndian() ? 1 : -1, sizeof(unsigned int), 0, 0, origin);
		PrimeNumberConvert::generate_big(number);
		return number;
	}
	std::unique_ptr<char[]> Primize::deprimize(GmpBigNumber& primedData)
	{
		GmpBigNumber number = primedData/ ENLARGE_SIZE;
		return toString(number);
	}
	GmpBigNumber Primize::fromString(std::unique_ptr<char[]>& data,unsigned int byteSize)
	{
		GmpBigNumber number;
		mpz_import(number.get_mpz_t(), byteSize, isBigEndian() ? 1 : -1, sizeof(unsigned int), 0, 0, data.get());
		return number;
	}
	std::unique_ptr<char[]> Primize::toString(GmpBigNumber& primedData)
	{
		void* result = nullptr;
		size_t actualSize;
		result = mpz_export(nullptr, &actualSize, isBigEndian() ? 1 : -1, sizeof(unsigned char), 0, 0, primedData.get_mpz_t());
		return std::unique_ptr<char[]>(static_cast<char*>(result));
	}
}