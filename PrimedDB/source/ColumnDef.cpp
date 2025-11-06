#include "Column.h"
#include "ErrorManager.h"
#include "Table.h"
#include "User.h"
using namespace std;
namespace liao::PrimedDB {
    Column::Column(string& name, unsigned size, const std::string& owner,DataType type)
        :m_byteSize(size), m_owner(owner),m_primed(StaticFunc::ByteConvert(m_byteSize)),m_type(type)
    {
        rename(name);
    }
    Column::Column(const std::string& owner, const std::string& fileLine):m_owner(owner)
    {
        constructFromFileLine(fileLine);
    }
    Column::Column(Column&& object) noexcept
        :m_columnName(std::move(object.m_columnName)), m_byteSize(object.m_byteSize),m_primed(object.m_primed), m_owner(object.m_owner),m_type(object.m_type)
    {}
    Column::Column(const Column& column)
        :m_columnName(column.m_columnName), m_byteSize (column.m_byteSize), m_primed(column.m_primed),m_owner(column.m_owner), m_type(column.m_type)
    {
    }
    void Column::constructFromFileLine(const std::string& line)
    {
        vector<string> vec;
        int n = 0;
        StaticFunc::Split(vec, line ,'|');
        if (vec.size() != 4)
        {
            Util::ErrorManager::Get().set(Util::ErrorLevel::Fatal,"StructureFileBroken","The structure file is broken at position of table "+m_owner,Infor::ClassInfor(THISFUNC,THISFILE,THISLINE));
            return;
        }
        m_columnName = vec[n++];
        m_type = static_cast<DataType>(stoi(vec[n++]));
        m_byteSize = stoi(vec[n]);
        m_primed = stoi(vec[n]);
    }
    void Column::setType(DataType type)
    {
        this->m_type = type;
    }
    DataType Column::getType() const
    {
        return m_type;
    }
    void Column::rename(std::string& name)
    {
        WriteLock lock(m_mutex);
        m_columnName = std::move(name);
    }
    void Column::resize(unsigned size)
    {
        WriteLock lock(m_mutex);
        m_byteSize = size;
    }
    const string& Column::getName() const
    {
        ReadLock lock(m_mutex);
        return m_columnName;
    }
    unsigned Column::size() const
    {
        ReadLock lock(m_mutex);
        return m_byteSize;
    }
    unsigned Column::primed() const
    {
        return m_primed;
    }
    const std::string& Column::getId() const
    {
        return m_columnName;
    }
    const string& Column::getOwner() const
    {
        ReadLock lock(m_mutex);
        return m_owner;
    }
    std::string Column::toString() const
    {
        std::ostringstream oss;
        ReadLock lock(m_mutex);
        oss << format("{}|{}|{}|{}",m_columnName,static_cast<int>(m_type),m_byteSize,m_primed);
        return oss.str();
    }
    bool Column::operator<(const Column& object)
    {
        return m_columnName < object.m_columnName;
    }
    bool Column::operator<(const Column& object)const
    {
        return m_columnName < object.m_columnName;
    }
    void Column::operator=(const Column&& object)
    {
        m_columnName = std::move(object.m_columnName);
        m_byteSize = object.m_byteSize;
        m_primed = object.m_primed;
        m_owner = std::move(object.m_owner);
        m_type= object.m_type;
    }
}