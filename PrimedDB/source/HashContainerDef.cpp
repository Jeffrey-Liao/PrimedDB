#include <HashContainer.h>
USECRPT;
USESTD;
namespace liao::Math
{
	void HashContainer::convertByteToString()
	{
		m_hashHex.clear();
		CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(m_hashHex));
		encoder.Put(m_hashByte, LENGTH);
		encoder.MessageEnd();
	}
	int HashContainer::GetByteSize(HashType type)
	{
		if (type == HashType::MD5)
			return MD5::DIGESTSIZE;
		else if (type == HashType::SHA3_512)
		{
			return SHA3_512::DIGESTSIZE;
		}
		else if (type == HashType::SHA512)
		{
			return SHA512::DIGESTSIZE;
		}
		else if (type == HashType::SHA3_256)
		{
			return SHA3_256::DIGESTSIZE;
		}
		else
			return SHA256::DIGESTSIZE;
	}
	void HashContainer::hashBySHA256(const std::string& message)
	{
		SHA256 hashObject;
		hash(hashObject,message);
	}
	void HashContainer::hashBySHA512(const std::string& message)
	{
		SHA512 hashObject;
		hash(hashObject, message);
	}
	void HashContainer::hashBySHA3_256(const std::string& message)
	{
		SHA3_256 hashObject;
		hash(hashObject, message);
	}
	void HashContainer::hashBySHA3_512(const std::string& message)
	{
		SHA3_512 hashObject;
		hash(hashObject, message);
	}
	void HashContainer::hashByMD5(const std::string& message)
	{
		MD5 hashObject;
		hash(hashObject, message);
	}
	void HashContainer::generate(std::string&& message, HashType type)
	{
		generate(message,type);
	}
	void HashContainer::generate(const std::string& message, HashType type)
	{
		WriteLock guard(m_mutex);
		if (type == HashType::MD5)
		{
			hashByMD5(message);
		}
		else if (type == HashType::SHA512)
		{
			hashBySHA512(message);
		}
		else if (type == HashType::SHA3_256)
		{
			hashBySHA3_256(message);
		}
		else if (type == HashType::SHA3_512)
		{
			hashBySHA3_512(message);
		}
		else
		{
			hashBySHA256(message);
		}
		m_containsHash = true;//
		m_hashNumbers.clear();
		for (int n = 0; n < LENGTH; n += 4)
		{
			m_hashNumbers.push_back(*(m_hashByte + n));
		}
	}
	HashContainer::HashContainer()
		:m_containsHash(false), m_hashByte{}, m_hashNumbers(8)
	{
		m_hashNumbers.reserve(LENGTH);
	}
	HashContainer::HashContainer(const std::string& message)
	{
		generate(message);
	}
	HashContainer::HashContainer(const HashContainer& obj)
		:m_containsHash(obj.m_containsHash),m_hashHex(obj.m_hashHex),m_hashNumbers(obj.m_hashNumbers)

	{
		memcpy(m_hashByte, obj.m_hashByte,LENGTH);
	}
	HashContainer::HashContainer(HashContainer&& move) noexcept
		:m_containsHash(move.m_containsHash),m_hashHex(std::move(move.m_hashHex)),m_hashNumbers(std::move(move.m_hashNumbers))
	{
		memcpy(m_hashByte, move.m_hashByte, LENGTH);
	}
	bool HashContainer::contains() const
	{
		ReadLock lock(m_mutex);
		return m_containsHash&&!m_hashHex.empty();
	}
	const vector<unsigned int>& HashContainer::getHashNumbers() const
	{
		ReadLock lock(m_mutex);
		return m_hashNumbers;
	}
	const CryptoPP::byte* HashContainer::getHashByte() const
	{
		ReadLock lock(m_mutex);
		return m_hashByte;
	}
	const std::string& HashContainer::getHashHex() const
	{
		ReadLock lock(m_mutex);
		return m_hashHex;
	}
	std::string&& HashContainer::moveHashHex()
	{
		return std::move(m_hashHex);
	}
}
