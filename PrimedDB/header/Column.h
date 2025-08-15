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
		mutable std::shared_mutex m_mutex;
	public:
		Column(std::string& name, short int size, Table& owner);
		Column(const Column&);
		void rename(std::string&);
		void resize(short int);
		const std::string& getName() const;
        short int getByteSize() const;
        const Table& getOwner() const;
        ~Column() = default;
	};
}