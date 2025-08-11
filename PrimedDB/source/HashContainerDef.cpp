#include <HashContainer.h>
using namespace CryptoPP;
using namespace std;
namespace liao::PrimedDB::math
{
	void HashContainer::generate(std::string&& message)
	{
		generate(message);
	}
	void HashContainer::generate(std::string& message)
	{
		SHA256 hash;
		hash.Update((const CryptoPP::byte*)message.data(), message.size());
		lock_guard<mutex> guard(m_mutex);
		hash.Final(this->m_hashByte);
		m_hashHex.clear();
		HexEncoder encoder(new StringSink(m_hashHex));
		encoder.Put(m_hashByte, sizeof(m_hashByte));
		encoder.MessageEnd();
		m_containsHash = true;//
		m_hashNumebrs.clear();
		for (int n = 0; n < LENGTH; n += 4)
		{
			m_hashNumebrs.push_back(*(m_hashByte + n));
		}

	}
	HashContainer::HashContainer()
		:m_containsHash(false), m_hashByte{}, m_hashNumebrs(8)
	{
		m_hashNumebrs.reserve(256);
	}
	HashContainer::HashContainer(std::string& message)
	{
		generate(message);
	}
	bool HashContainer::contains() const
	{
		lock_guard<std::mutex> lock(m_mutex);
		return m_containsHash;
	}
	const vector<unsigned int>& HashContainer::getHashNumbers() const
	{
		lock_guard<std::mutex> lock(m_mutex);
		return m_hashNumebrs;
	}
	const CryptoPP::byte* HashContainer::getHashByte() const
	{
		lock_guard<std::mutex> lock(m_mutex);
		return m_hashByte;
	}
	const std::string& HashContainer::getHashHex() const
	{
		lock_guard<std::mutex> lock(m_mutex);
		return m_hashHex;
	}
}
