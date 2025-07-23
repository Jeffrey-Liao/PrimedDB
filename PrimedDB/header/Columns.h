#pragma once
#include<list>
#include<vector>
#include<Log.h>
#include<Column.h>
namespace liao::db
{
	class Columns {
		std::vector<Column*> m_allColumns;

	private:
		void clear();
	public:
		Column* create(std::string name, short int byteSize, Table* owner);
		bool remove(Column* column);
		int size() const;
		bool contains(Column* column) const;
		bool contains(const std::string& name) const;
		Column* get(int index);

	};
}
