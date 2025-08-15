#pragma once
#include <vector>
#include<hex.h>
#include<md5.h>
#include <sha3.h>
#include "Defs.h"
namespace liao::Math
{
	DYNAMIC
	concept Concept_HashTypeName = requires
	{
		std::is_same_v<T, CryptoPP::MD5> ||
		std::is_same_v<T, CryptoPP::SHA256> ||
		std::is_same_v<T, CryptoPP::SHA512>||
		std::is_same_v<T, CryptoPP::SHA3_256>||
		std::is_same_v<T, CryptoPP::SHA3_512>;
	};
	enum class HashType
	{
		MD5,
		SHA256,
		SHA512,
		SHA3_256,
		SHA3_512
	};
	class HashContainer
	{
		unsigned int LENGTH = CryptoPP::SHA3_512::DIGESTSIZE;

		mutable ShareMutex m_mutex;
		CryptoPP::byte m_hashByte[CryptoPP::SHA3_512::DIGESTSIZE];
		bool m_containsHash;
		std::string m_hashHex;
		std::vector<unsigned int> m_hashNumbers;


		DYNAMICCON(Concept_HashTypeName)
		void hash(T& hash, const std::string& message)
		{
			LENGTH = T::DIGESTSIZE;
			hash.Update((const CryptoPP::byte*)message.data(), message.size());
			hash.Final(this->m_hashByte);
			convertByteToString();
		}

		void hashByMD5(const std::string& message);
		void hashBySHA256(const std::string& message);
		void hashBySHA512(const std::string& message);
		void hashBySHA3_256(const std::string& message);
		void hashBySHA3_512(const std::string& message);
	public:
		void convertByteToString();
		static int GetByteSize(HashType);
		void generate(std::string&& message, HashType type = HashType::SHA256);
		void generate(const std::string& message, HashType type = HashType::SHA256);
		HashContainer();
		HashContainer(const std::string& message);
		HashContainer(const HashContainer& obj);
		HashContainer(HashContainer&& move) noexcept;
		bool contains() const;
		const std::vector<unsigned int>& getHashNumbers() const;
		const CryptoPP::byte* getHashByte() const;
		const std::string& getHashHex() const;
		std::string&& moveHashHex();
	};
}
