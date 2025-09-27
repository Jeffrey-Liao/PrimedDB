#pragma once
#include <Table.h>
namespace liao::PrimedDB
{
	//in file "schemas.def", each line of file is a Schema
	class Schema
	{
		std::string m_name;
		std::vector<TablePtr> m_tables;
		mutable ShareMutex m_mutex;
		auto find(const std::string& name) const;
		auto find(const std::string& name);
		void constructTable(const std::string& fileLine);
	public:
        Schema(std::string& name);
		Schema(Schema&& other);
		void construct(const std::string& fileLine);
		const std::string& getId() const;
		const std::string& getName() const;
		void rename(std::string& name);
		const std::vector<TablePtr>& getTables() const;
		const TablePtr getTable(const std::string& name)const;
		void addTable(std::string& name, User& owner, UserLevel permission);
		void removeTable(std::string& name, User& user);
		TablePtr getTable(const std::string& name, const User& user);
		std::string toString() const;
	};
}
