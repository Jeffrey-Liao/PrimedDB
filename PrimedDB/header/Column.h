#pragma once
#include "Global.h"
namespace liao::PrimedDB
{
	class Table;

	class Column:public NullRefProvider<Column>
	{
		std::string m_columnName;
		short int m_byteSize;
		Table& m_owner;
		mutable std::shared_mutex m_mutex;
		ColumnType m_type;
		Column* ref;
		std::string m_id;
	public:
		Column();
		Column(std::string& name, short int size, Table& owner);
		Column(const std::string&,Table&);
		Column(const Column&);
		Column(Column&&) noexcept;
		void rename(std::string&);
		void resize(short int);
		const std::string& getName() const;
        short int getByteSize() const;
        const Table& getOwner() const;
		const std::string& getId() const;
		std::string toString() const;
		Column& operator=(const Column&);
        ~Column() = default;
	};
}