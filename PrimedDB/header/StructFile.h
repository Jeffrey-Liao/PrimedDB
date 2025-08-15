#pragma once
#include "Global.h"
namespace liao::Util
{
	class StructFile
	{
		std::unordered_map<std::string, std::string> structure;
		std::fstream file;
	public:
		StructFile(std::string&);
		std::string& get(const std::string&);
		void set(std::string&,std::string&);
		bool read();
		void save();
		std::string& operator[](const std::string&);
		~StructFile();
	};
}
