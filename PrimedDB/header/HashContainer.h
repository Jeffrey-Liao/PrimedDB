#pragma once
#include <sha.h>
#include <hex.h>
#include <filters.h>
#include <vector>
namespace liao::primed::hash
{
	
	class HashContainer
	{
		CryptoPP::byte m_hashByte[CryptoPP::SHA256::DIGESTSIZE];
		bool m_containsHash;
		std::string m_hashHex;
		std::vector<unsigned int> m_hashNumebrs;
	public:
		static const unsigned int LENGTH = CryptoPP::SHA256::DIGESTSIZE;
		void generate(std::string&& message);
		void generate(std::string& message);
		HashContainer();
		HashContainer(std::string& message);
		bool contains() const;
		const std::vector<unsigned int>& getHashNumbers() const;
		const CryptoPP::byte* getHashByte() const;
		const std::string& getHashHex() const;
	};
}
