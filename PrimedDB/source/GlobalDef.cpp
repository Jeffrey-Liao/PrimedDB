#include "Global.h"
#include <sstream>
#include <random>

#include "Log.h"
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
	std::string StaticFunc::GetHashKey(const std::string& seed, Math::HashType type)
	{
		Math::HashContainer container;
		container.generate(seed, type);
		return container.getHashHex();
	}
	void StaticFunc::Split(vector<string>& out,const string& s, char delimiter)
	{
		if (!s.empty())
		{
			string token;
			istringstream tokenStream(s);
			while (getline(tokenStream, token, delimiter)) {
				out.push_back(token);
			}
		}

	}
	std::shared_ptr<fstream> StaticFunc::OpenDataFile(const std::string& name)
	{
		if (!filesystem::exists(Util::Setting::Get().getDataDirectory()))
		{
			filesystem::create_directories(Util::Setting::Get().getDataDirectory());
		}
		shared_ptr<fstream> filePtr = make_shared<fstream>(Util::Setting::Get().getDataDirectory()/(name+".dat"),ios::in | ios::out | ios::binary);
		return filePtr;
	}
	void StaticFunc::StringToVectorBool(std::vector<bool>& result, const std::string& data)
	{
		result.clear();
		result.resize(data.size());  // 预分配空间
		for (int n = 0;n<data.size();++n)
		{
			result[n] = (data[n] == '1');
		}
	}
	unsigned StaticFunc::ByteConvert(unsigned byte)
	{
		return byte*2;
	}
	bool StaticFunc::ValidName(std::string& name)
	{
		if (name.size()>COLUMN_NAME_LEN)
            return false;
		for (auto& c : name)
		{
			if (!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')))
			{
				return false;
			}
		}
		return true;
	}
	void StaticFunc::SplitFast(std::vector<size_t>& delimiters, const std::string& str, char delimiter)
	{
		for (size_t i = 0; i < str.size(); ++i)
		{
			if (str[i] == delimiter)
				delimiters.push_back(i);
		}
	}
	void StaticFunc::clearMemory(void* memory, size_t size)
	{
		memset(memory, 0, size);
	}
	std::string StaticFunc::vectorToString(const std::deque<shared_ptr<string>>& vec, const std::vector<bool>& mask)
	{
		string result;
		for (int n = 0;n<vec.size();++n)
		{
			if (mask[n])
			{
				if (vec[n]->empty())
					result += "null:";
				else
					result += *vec[n] + ":";
			}

		}
		if (!result.empty())
			result.pop_back();
		return result;
	}
	void StaticFunc::WriteInfo(const std::string& title, const std::string& message)
	{
		Infor::Log::Get()[Infor::LogType::Info]<<title+":  "<<message<< Infor::Log::LogEndl;
	}
	unsigned StaticFunc::MaxSizeForBlock(unsigned byteSize)
	{
		return Util::Setting::Get().getBlockSize() / byteSize;
	}
	void Global::set(std::string name, int value)
	{
		m_values[name] = value;
	}
	int Global::get(std::string name)
	{
		if (m_values.contains(name))
            return m_values[name];
		else
			return -1;
	}
	bool Global::exist(std::string name)
	{
		return m_values.contains(name);
	}
}