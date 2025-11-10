#pragma once
#include "Global.h"
namespace liao::PrimedDB {
	class DataFile
	{
		std::shared_ptr<std::fstream> m_file;
		int m_byteSize;
		std::vector<bool> m_available;
		std::deque<int> m_owned;
	public:
		DataFile() = default;
		DataFile(const std::string& name);

		~DataFile();
	};

}