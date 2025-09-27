
#include "StructFile.h"
USESTD;
namespace liao::Util
{
	StructFile::StructFile(std::string& fileName)
	{
		file.open(fileName, std::ios::in | std::ios::out | std::ios::app);
	}
	std::string& StructFile::get(const std::string& name)
	{
		return structure[name];
	}
	void StructFile::set(std::string& key, std::string& value)
	{
		structure[key] = value;
	}
	bool StructFile::read()
	{
		vector<string> key_value;
		string line;
		if (file.fail())
			return false;
		else
		{
			while (std::getline(file, line)) {
				// 处理每一行
                StaticFunc::Split(key_value, line, ':');
                set(key_value[0], key_value[1]);
			}
			return true;
		}
	}
	void StructFile::save()
	{
		for (pair<string,string> item : structure)
		{
			file<<format("{0}:{1}\n",item.first,item.second);
		}
		file.flush();
	}
	std::string& StructFile::operator[](const std::string& name)
	{
		return get(name);
	}
	StructFile::~StructFile()
	{
		file.close();
	}
}