#pragma once
#include <Table.h>
namespace liao::PrimedDB
{
	using TablePtr = std::shared_ptr<Table>;
	class Schema
	{
		std::string m_id;
		std::string m_name;
		std::vector<TablePtr> m_tables;

		auto find(const std::string& name) const;
		auto find(const std::string& name);
	public:
		const std::string& getId() const;
		const std::string& getName() const;
		void rename(std::string& name);
		const std::vector<TablePtr>& getTables() const;
		const Table& getTable(const std::string& name)const;
		void addTable(std::string& name, User& owner, UserLevel permission);
		void removeTable(std::string& name, User& user);
		Table& getTable(const std::string& name, const User& user);
	};
}
