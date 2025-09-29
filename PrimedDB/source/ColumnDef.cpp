#include "Column.h"
#include "Table.h"
#include "User.h"
using namespace std;
namespace liao::PrimedDB {
    Column::Column(string& name, short int size, Table& owner)
        :m_byteSize(size), m_owner(owner)
    {
        rename(name);
    }
    Column::Column(Table& owner, const std::string& fileLine):m_owner(owner)
    {
        constructFromFileLine(fileLine);
    }
    Column::Column(Column&& object) noexcept
        :m_columnName(std::move(object.m_columnName)), m_byteSize(object.m_byteSize), m_owner(object.m_owner)
    {}
    Column::Column(const Column& column)
        :m_columnName(column.m_columnName), m_byteSize (column.m_byteSize),m_owner(column.m_owner)
    {
    }
    void Column::constructFromFileLine(const std::string& line)
    {
        vector<string> vec;
        int n = 0;
        StaticFunc::Split(vec, line ,':');
        if (vec.size() != 3)
        {
            return;
        }
        m_columnName = vec[n++];
        m_type = static_cast<ColumnType>(stoi(vec[n++]));
        m_byteSize = stoi(vec[n]);
    }
    void Column::setType(ColumnType type)
    {
        this->m_type = type;
    }
    ColumnType Column::getType() const
    {
        return m_type;
    }
    void Column::rename(std::string& name)
    {
        WriteLock lock(m_mutex);
        if (m_columnName !="null" && !m_owner.exist(name)&&name.size() < COLUMN_NAME_LEN)
            m_columnName = std::move(name);
    }
    void Column::resize(unsigned short int size)
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
    const std::string& Column::getId() const
    {
        return m_columnName;
    }
    Table& Column::getOwner() const
    {
        ReadLock lock(m_mutex);
        return m_owner;
    }
    std::string Column::toString() const
    {
        std::ostringstream oss;
        ReadLock lock(m_mutex);
        oss << format("{}:{}:{}",m_columnName,static_cast<int>(m_type),m_byteSize);
        return oss.str();
    }
    Column& Column::operator=(const Column& object)
    {
        m_columnName = object.m_columnName;
        m_byteSize = object.m_byteSize;
        m_owner = object.m_owner;
        return *this;
    }
}