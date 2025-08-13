#pragma once
#include "Global.h"

namespace liao::PrimedDB {
	class Table;
	class User
	{
		std::string m_name;
		std::string m_password;
        UserLevel m_level;
		const std::string m_id;
		std::vector<Table*> m_tables;
	public:
        User() = default;
		User(std::string name, std::string password, UserLevel level);

	};
}