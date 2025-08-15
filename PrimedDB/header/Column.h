#pragma once
#include "Global.h"
namespace liao::PrimedDB
{
	class Table;

	class Column:public NullRefProvider<Column>
	{
		std::string m_columnName;
		short int m_byteSize;
		const Table& m_owner;
		mutable std::shared_mutex m_mutex;
		std::string m_id;

		Column();
	public:
		Column(std::string& name, short int size, Table& owner);
		Column(const std::string&,Table&);
		Column(const Column&);
		void rename(std::string&);
		void resize(short int);
		const std::string& getName() const;
        short int getByteSize() const;
        const Table& getOwner() const;
		const std::string& getId() const;
		std::string toString() const;
        ~Column() = default;
	};
}