#pragma once
#include <deque>
#include <string>
#include <vector>

class Clean
{
	std::vector<std::string> m_tables;
public:
	Clean();
	Clean(std::vector<std::string>& tables);

	unsigned int size() const;
	void clear();

};
