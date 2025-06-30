#pragma once
#include "externalLib/cryptopp890/sha.h"
#include "externalLib/cryptopp890/hex.h"
#include "externalLib/cryptopp890/filters.h"
namespace liao::primed::hash
{
	
	class HashGenerator
	{
		CryptoPP::byte hashByte[CryptoPP::SHA256::DIGESTSIZE];
		bool containsHash;
		CryptoPP::Integer hashInteger;
	public:
		void generate(std::string& message);
		HashGenerator();
		HashGenerator(std::string& message);
		bool contains() const;
		const CryptoPP::Integer& getHashInteger() const;
		const CryptoPP::byte* getHashByte() const;
		std::string getHashHex() const;
	};
}
