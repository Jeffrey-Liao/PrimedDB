#include "Global.h"
#include <sstream>
#include <random>

#include "Setting.h"
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
	void StaticFunc::Split(vector<string>& out,const string& s, char delimiter)
	{
		string token;
		istringstream tokenStream(s);
		while (getline(tokenStream, token, delimiter)) {
			out.push_back(token);
		}
	}
	std::shared_ptr<fstream> StaticFunc::OpenDataFile(const std::string& name)
	{
		shared_ptr<fstream> filePtr = make_shared<fstream>(Util::Setting::Get().getDataDirectory().data()+name+".dat",ios::in | ios::out | ios::binary);
		return filePtr;
	}
	Math::HashType Configuration::UserIDHashType = Math::HashType::MD5;
	string Configuration::UserInforFile = "users.dat";

}