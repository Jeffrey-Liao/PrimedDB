#pragma once
#include <sha.h>
#include <vector>
#include<hex.h>
#include <mutex>
namespace liao::PrimedDB::math
{
	class HashContainer
	{
		mutable std::mutex m_mutex;
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
