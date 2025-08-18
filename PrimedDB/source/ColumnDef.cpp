#include "Column.h"
#include "Table.h"
using namespace std;
namespace liao::PrimedDB {
    Column::Column()
        : m_byteSize(-1), m_columnName("null"), m_owner(Table::GetNullRef()), m_id("null")
    {}
    Column::Column(string& name, short int size, Table& owner)
        :m_byteSize(size), m_owner(owner), m_id(StaticFunc::GetUniqueId())
    {
        rename(name);
    }
    Column::Column(const std::string& fileLine, Table& owner):m_owner(owner), m_id(StaticFunc::GetUniqueId())
    {
    }
    Column::Column(Column&& object) noexcept
        :m_columnName(std::move(object.m_columnName)), m_byteSize(object.m_byteSize), m_owner(object.m_owner), m_id(std::move(object.m_id))
    {}
    Column::Column(const Column& column)
        :m_columnName(column.m_columnName), m_byteSize (column.m_byteSize),m_owner(column.m_owner),m_id(StaticFunc::GetUniqueId())
    {
    }
    const std::string& Column::getId() const
    {
        ReadLock lock(m_mutex);
        return m_id;
    }
    void Column::rename(std::string& name)
    {
        WriteLock lock(m_mutex);
        if (m_columnName !="null")
        {
            m_columnName = std::move(name);
        }
    }
    void Column::resize(short int size)
    {
        WriteLock lock(m_mutex);
        m_byteSize = size;
    }
    const string& Column::getName() const
    {
        ReadLock lock(m_mutex);
        return m_columnName;
    }
    short int Column::getByteSize() const
    {
        ReadLock lock(m_mutex);
        return m_byteSize;
    }
    const Table& Column::getOwner() const
    {
        ReadLock lock(m_mutex);
        return m_owner;
    }
    std::string Column::toString() const
    {
        std::ostringstream oss;
        ReadLock lock(m_mutex);
        oss << format("{}:{}:{}",m_id,m_columnName,m_byteSize);
        return oss.str();
    }
    Column& Column::operator=(const Column& object)
    {
        m_id = object.m_id;
        m_columnName = object.m_columnName;
        m_byteSize = object.m_byteSize;
        m_owner = object.m_owner;
        return *this;
    }
}