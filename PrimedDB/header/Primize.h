#pragma once
#include <memory>

#include "PrimeNumberConvert.h"
namespace liao::Math
{

	class Primize
	{
		GmpBigNumber m_primedMemory;
	public:
		Primize(std::unique_ptr<char> originData,unsigned int byteSize);
		Primize(GmpBigNumber& primedData);
		Primize(Primize&) = delete;
		Primize(Primize&&);
		const GmpBigNumber& getPrimedNumber()const;
		std::unique_ptr<char> getOriginData();
		static constexpr unsigned int ENLARGE_SIZE = 512;
		static GmpBigNumber primize(std::unique_ptr<char>& originData, unsigned int byteSize);
		static std::unique_ptr<char> deprimize(GmpBigNumber& primedData);
		static std::unique_ptr<char> toString(GmpBigNumber& primedData);
		static GmpBigNumber fromString(std::unique_ptr<char>& data,unsigned int byteSize);
	};
}