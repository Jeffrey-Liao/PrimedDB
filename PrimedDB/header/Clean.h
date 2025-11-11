#pragma once
#include <deque>
#include <string>
#include <vector>

class Clean
{

public:
	std::vector<std::string> m_tables;
	Clean();
	Clean(std::vector<std::string>& tables);
	void clear();

};
