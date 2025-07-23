#pragma once
#include <string>
namespace liao::db
{
	class Table;
	class Column
	{
		std::string columnName;
		short int byteSize;
		Table& owner;
	};
}