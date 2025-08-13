#include "Column.h"
using namespace std;
namespace liao::PrimedDB { 
    Column::Column(string name, short int size, Table& owner)
        : m_columnName(name), m_byteSize(size), m_owner(owner)
    {}
    Column::Column(const Column& column)
        :m_columnName(column.m_columnName), m_byteSize (column.m_byteSize),m_owner(column.m_owner)
    {}
    void Column::rename(const std::string& name)
    {
        WriteLock lock(m_mutex);
        m_columnName = name;
    }
    void Column::resize(short int size)
    {
        WriteLock lock(m_mutex);
        m_byteSize = size;
    }
    string Column::getName() const
    {
        ReadLock lock(m_mutex);
        return m_columnName;
    }
    short int Column::getByteSize() const
    {
        ReadLock lock(m_mutex);
        return m_byteSize;
    }
    Table& Column::getOwner() const
    {
        ReadLock lock(m_mutex);
        return m_owner;
    }
    Column::~Column()
    {}
}