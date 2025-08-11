#pragma once
#include "Columns.h"
namespace liao::db
{
	class User;
	class Table
	{
		std::string name;
		std::vector<Column*> columns;
		User& owner;
	};
}