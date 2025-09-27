#pragma once
#include "Global.h"
namespace liao::PrimedDB
{
	class Table;

	class Column
	{
		std::string m_columnName;
		short int m_byteSize;
		Table& m_owner;
		ColumnType m_type;
		mutable std::shared_mutex m_mutex;

		void constructFromFileLine(const std::string& line);
	public:
		Column(std::string& name, short int size, Table& owner);
		Column(Table&, const std::string&);
		Column(const Column&);
		Column(Column&&) noexcept;
		void rename(std::string&);
		void resize(unsigned short int);
		void setType(ColumnType);
		ColumnType getType() const;
		const std::string& getName() const;
        short int getByteSize() const;
        const Table& getOwner() const;
		const std::string& getId() const;
		std::string toString() const;
		Column& operator=(const Column&);
        ~Column() = default;
	};
	using ColumnPtr = std::shared_ptr<Column>;
}