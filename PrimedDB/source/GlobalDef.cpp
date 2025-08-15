#include "Global.h"
#include <sstream>
#include <random>
USESTD;
USECRPT;
namespace liao {
	std::string StaticFunc::GetUniqueId(Math::HashType type)
	{
		int random = GetRandom(0, 100);
		Util::TimeStamp timeStamp(Util::TimeStamp::SystemClock::now());
		Math::HashContainer container;
		container.generate(timeStamp.getString() + to_string(random),type);
		return container.getHashHex();
	}
	static void Split(vector<string>& out,const string& s, char delimiter)
	{
		string token;
		istringstream tokenStream(s);
		while (getline(tokenStream, token, delimiter)) {
			out.push_back(token);
		}
	}
	Math::HashType Configuration::UserIDHashType = Math::HashType::SHA256;
	string Configuration::UserInforFile = "users.dat";
}