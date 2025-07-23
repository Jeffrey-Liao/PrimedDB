#include <HashContainer.h>
using namespace CryptoPP;
using namespace std;
namespace liao::primed::hash
{
	void HashContainer::generate(std::string&& message)
	{
		generate(message);
	}
	void HashContainer::generate(std::string& message)
	{
		m_containsHash = true;//

		SHA256 hash;
		hash.Update((const CryptoPP::byte*)message.data(), message.size());
		hash.Final(this->m_hashByte);



		m_hashHex.clear();
		HexEncoder encoder(new StringSink(m_hashHex));
		encoder.Put(m_hashByte, sizeof(m_hashByte));
		encoder.MessageEnd();

		m_hashNumebrs.clear();
		for (int n = 0; n < LENGTH; n+=4)
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
		return m_containsHash;
	}
	const vector<unsigned int>& HashContainer::getHashNumbers() const
	{
		return m_hashNumebrs;
	}
	const CryptoPP::byte* HashContainer::getHashByte() const
	{
		return m_hashByte;
	}
	const std::string& HashContainer::getHashHex() const
	{
		return m_hashHex;
	}
}
