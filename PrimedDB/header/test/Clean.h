#pragma once
#include <deque>
#include <string>
#include <vector>
namespace liao::Test
{
	class Clean
	{

	public:
		std::vector<std::string> m_tables;
		Clean();
		Clean(std::vector<std::string>& tables);
		void clear();

	};
}

