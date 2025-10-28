#include "BlockManager.h"
#include "Table.h"
#include "User.h"
#include "UserManager.h"
USESTD;
namespace liao::PrimedDB
{
	Table::Table(Table&& move) noexcept
		:m_name(std::move(move.m_name)),m_available(std::move(move.m_available)),m_permission(move.m_permission), m_size(move.m_size), m_ownerId(std::move(move.m_ownerId)), m_columns(std::move(move.m_columns)),m_byteSize(move.m_byteSize),m_primedSize(move.m_primedSize), m_pendingOperations(std::move(move.m_pendingOperations)),m_recordByte(move.m_recordByte)
	{}
	Table::Table(const std::string& fileLine)
	{
		vector<string> labels;
        construct( labels,fileLine);
	}
	Table::Table(const std::string& fileLine, std::vector<std::string>& labels)
	{
		construct(labels, fileLine);
	}
	Table::Table(std::string ownerId, std::string& name, UserLevel permission, set<Column>& columns)
		:m_ownerId(std::move(ownerId)), m_permission(permission), m_byteSize{0}, m_primedSize{0},m_recordByte(0)
	{
		rename(name);
		for (auto& column:columns)
		{
			m_columns.emplace_back(std::move(column));
		}
		for (auto& column:m_columns)
		{
			m_byteSize += column.size();
			m_primedSize += column.primed();
			m_recordByte += column.primed();
		}
	}
	void Table::construct(vector<string>& labels,const std::string& fileLine)
	{
		labels.clear();
		StaticFunc::Split(labels, fileLine, ':');
		size_t n = 0;
        m_name = labels[n++];
		StaticFunc::StringToVectorBool(m_available,labels[n++]);
		m_permission = static_cast<UserLevel>(stoi(labels[n++]));
		m_size = stoi(labels[n++]);
		m_ownerId = labels[n++];
		m_byteSize = stoi(labels[n++]);
		m_primedSize = stoi(labels[n++]);
		m_recordByte = stoi(labels[n++]);
		for (;n < labels.size();++n)
		{
			m_columns.emplace_back(m_name,labels[n]);
		}
	}
	std::pair<int, int> Table::convertBlockPos(unsigned position)
	{
		int recordPerBlock = Util::Setting::Get().getBlockSize()/byte();
        return std::make_pair(position / recordPerBlock, position % recordPerBlock);
	}

	void Table:: primize(UCharPtr& memory,unsigned byteSize,unsigned primedSize,unsigned recordSize)
	{
		char* origin = memory.release();
		char* ptr = new char[primedSize + recordSize];
		memset(ptr, 0, primedSize + recordSize);
		Math::GmpBigNumber convert,recordNumber=1;
		unsigned m = 0;
		size_t count = 0;
		for (unsigned n = 0; n < byteSize;m+=8)
		{
			unsigned size = byteSize - n;
			if (size<4)
			{
				if (size == 1)
					convert = static_cast<unsigned char>(*(origin + n));
				else if (size == 2)
					convert = static_cast<unsigned short>(*((unsigned short*)origin + n));
				else
				{
					convert = (static_cast<uint32_t>(*(origin + n)) << 16) |
						(static_cast<uint32_t>(*(origin + n + 1 )) << 8) |
						(static_cast<uint32_t>(*(origin + n + 2)) << 0);
				}
			}
			else
				convert = static_cast<unsigned int>(
					*((unsigned int*)(origin+n)));
			Math::PrimeNumberConvert::generate_big(convert, 9);

			recordNumber *= convert;
			mpz_export(ptr + m, &count, Math::isBigEndian() ? 1 : -1, 1, 0, 0, convert.get_mpz_t());
			n += 4;
		}
		mpz_export(ptr + m , nullptr, Math::isBigEndian() ? 1 : -1, 1, 0, 0, recordNumber.get_mpz_t());
		count = 0;
		memory.reset(ptr);
		delete[] origin;
	}
	void Table::deprimize(UCharPtr& memory,unsigned byteSize,unsigned primedSize, unsigned recordSize)
	{
		char* origin = memory.release();
		char* ptr = new char[byteSize];
		memset(ptr, 0, byteSize);
		Math::GmpBigNumber convert;
		unsigned m = 0;
		for (unsigned n =0 ;n< primedSize&&m< byteSize;n+=8)
		{
			mpz_import(convert.get_mpz_t(), 8, Math::isBigEndian() ? 1 : -1, 1, 0, 0, origin+n);
			convert /= Util::Setting::Get().getEnlargeSize();
			mpz_export(ptr + m, nullptr, Math::isBigEndian()? 1:-1, 1, 0, 0, convert.get_mpz_t());
			unsigned size = byteSize - m;
			if (size > 4)
				m += 4;
			else
				m += size-1;
		}
		memory.reset(ptr);
		delete[] origin;
	}

	bool Table::isEmpty()const
	{
		ReadLock lock(m_mutex);
		return m_size == 0;
	}
	const std::string& Table::getName() const
	{
		ReadLock lock(m_mutex);
		return m_name;
	}
	auto Table::findColumn(const std::string& name)
	{
		ReadLock lock(m_mutex);
		return std::find_if(m_columns.begin(), m_columns.end(), [&name](const Column& column)
			{
				return column.getName() == name;
			});
	}
	auto Table::findColumn(const std::string& name) const
	{
		ReadLock lock(m_mutex);
		return std::find_if(m_columns.begin(), m_columns.end(), [&name](const Column& column)
			{
				return column.getName() == name;
			});
	}
	void Table::renameColumn(const std::string& name, std::string& newName)
	{
		WriteLock lock(m_mutex);
		auto iter = findColumn(name);
		if (iter != m_columns.end() && StaticFunc::ValidName(newName))
		{
            (*iter).rename(newName);
		}
	}
	//check is given name belongs to a column in the table
	bool Table::existColumn(const std::string& name) const
	{
		auto iter = findColumn(name);
		ReadLock lock(m_mutex);
		return iter != m_columns.end();
	}
	std::vector<bool>& Table::getAvailable()
	{
		return m_available;
	}
	std::deque<int>& Table::getOwned()
	{
		return m_owned;
	}
	UserLevel Table::getPermission() const
	{
		ReadLock lock(m_mutex);
		return m_permission;
	}
	void Table::setUnavailable(unsigned pos)
	{
		WriteLock lock(m_mutex);
		m_available[pos] = false;
	}
	void Table::setAvailable(unsigned pos)
	{
		WriteLock lock(m_mutex);
		m_available[pos] = true;
	}
	unsigned Table::incrementSize()
	{
		WriteLock lock(m_mutex);
		m_size++;
		m_available.emplace_back(true);
		return m_size;
	}
	void Table::clear()
	{
		WriteLock lock(m_mutex);
		for (auto& b : m_owned)
		{
			BlockManager::Get().drop(b);
		}
		m_owned.clear();
		m_columns.clear();
	}
	unsigned Table::byte()const
	{
		ReadLock lock(m_mutex);
		return m_byteSize;
	}
	unsigned Table::primedByte()const
	{
		ReadLock lock(m_mutex);
		return m_primedSize;
	}
	bool Table::addColumn(std::string& name, unsigned byteSize, DataType type)
	{
		if (existColumn(name))
			return false;
		else
		{
			WriteLock lock(m_mutex);
			m_columns.emplace_back(Column(name, byteSize, m_name, type));
			m_byteSize+=byteSize;
			m_primedSize += StaticFunc::ByteConvert(byteSize);
			m_recordByte += m_primedSize;
			return true;
		}
	}
	void Table::dropColumn(const std::string& name)
	{
		auto iter = findColumn(name);
		WriteLock lock(m_mutex);
		if (iter != m_columns.end())
		{
			m_columns.erase(iter);
		}
	}
	bool Table::rename(std::string& name)
	{
		if (!TableManager::Get().exist(name)&&StaticFunc::ValidName(name))
		{
			WriteLock lock(m_mutex);
			m_name = std::move(name);
			return true;
		}
		else
		{
			Util::ErrorManager::Get().set(Util::ErrorLevel::Warning, "NameInvalid", "Given name for rename operation is invalid or name is already exists.", Infor::ClassInfor(THISFUNC, THISFILE));
			return false;
		}
	}
	unsigned Table::byteSize()const
	{
		ReadLock lock(m_mutex);
		return m_byteSize;
	}
	unsigned Table::totalByte()const
	{
		ReadLock lock(m_mutex);
		return m_primedSize+m_recordByte;
	}
	size_t Table::size() const
	{
		ReadLock lock(m_mutex);
		return m_size;
	}
	const string& Table::getOwnerId() const
	{
		ReadLock lock(m_mutex);
		return m_ownerId;
	}
	string Table::toString()
	{
		std::ostringstream oss;
        ReadLock lock(m_mutex);
		oss << m_name << ":";
		for (int n =0;n<m_available.size();++n)
		{
			oss << m_available[n];
		}
		oss << ":";
		oss<<static_cast<int>(m_permission) << ":";
        oss << m_size << ":";
        oss << m_ownerId << ":";
		oss << m_byteSize << ":";
		oss<< m_primedSize << ":";
		oss << m_recordByte << ":";
		for (int n = 0; n < m_columns.size(); ++n)
		{
			oss<<m_columns[n].toString();
			if (n + 1 != m_columns.size())
				oss << ":";
		}
        return oss.str();
	}
	void Table::insert(const std::string& userId, UCharPtr memory, unsigned size)
	{
		auto blockPos = convertBlockPos(m_size);
		WriteLock lock(m_mutex);
		int blockIndex = m_owned.empty() || blockPos.first >= m_owned.size() ? -1 : m_owned[blockPos.first];
		primize(memory,m_byteSize,m_primedSize,m_recordByte);
		m_pendingOperations.emplace(Transection(userId, m_name, TransectionType::Insert, blockIndex, blockPos.second, size, std::move(memory)));
	}
	void Table::update(const std::string& userId, unsigned position, UCharPtr memory, unsigned size)
	{
		auto blockPos = convertBlockPos(position);
		WriteLock lock(m_mutex);
		m_pendingOperations.emplace(Transection(userId, m_name, TransectionType::Update, blockPos.first, blockPos.second, size, std::move(memory)));
	}
	void Table::remove(const std::string& userId, unsigned position)
	{
		auto blockPos = convertBlockPos(position);
		WriteLock lock(m_mutex);
		m_pendingOperations.emplace(Transection(userId, m_name, TransectionType::Delete, blockPos.first, blockPos.second));
	}

	void Table::commit()
	{
		WriteLock lock(m_mutex);
		while (!m_pendingOperations.empty())
		{
			BlockManager::Get().operate(m_pendingOperations.front());
			if (m_pendingOperations.front().getType() == TransectionType::Insert)
				++m_size;
			else if (m_pendingOperations.front().getType() == TransectionType::Delete)
				--m_size;
			m_pendingOperations.pop();
		}
	}
	void Table::rollback(const string& name)
	{
		string buffer;
		fstream tranFile(Util::Setting::Get().getDataDirectory() / (m_name + ".trs"),ios::ate);
		if (!tranFile.is_open()|| tranFile.tellg() <=0) {
			Util::ErrorManager::Get().set(Util::ErrorLevel::Warning, "TransactionFileNotFound", "Transaction file not found.", Infor::ClassInfor(THISFUNC, THISFILE));
			tranFile.close();
		}
		tranFile.seekg(-1, std::ios::end);
		char ch;
		bool found = false;
		while (tranFile.tellg() >= 0 && !found) {
			tranFile.get(ch);
			if (ch == '\n' || ch == '\r') { // 换行符
				found = true;
			}
			else {
				buffer = ch + buffer; // 添加到行开头
			}
			if (tranFile.tellg() > 0) {
				tranFile.seekg(-2, std::ios::cur); // 向前移动两位（当前字符 + 1 位）
			}
			else {
				break; // 到达文件开头
			}
		}
		Transection transection(name,m_name,buffer,true);
		BlockManager::Get().operate(transection);
	}
	void Table::addBlock(unsigned pos)
	{
		{
			WriteLock lock(m_mutex);
			m_owned.emplace_back(pos);
		}
		BlockManager::Get().get_noLock(pos).read();
	}
	void Table::dropBlock(unsigned pos)
	{
		WriteLock lock(m_mutex);
		(*std::find(m_owned.begin(), m_owned.end(), pos)) = -1;
	}
	void Table::dropBlockAt(unsigned pos)
	{
		WriteLock lock(m_mutex);
		m_owned[pos] = -1;
	}
	void Table::setOwner(const std::string& ownerid)
	{
		WriteLock lock(m_mutex);
		m_ownerId = ownerid;
	}
	Record Table::select(const std::string& column, bool raw)
	{
		vector<char*> reference;
		vector<ReadLock> locks;

		if (column == "all")
		{
			for (size_t i = 0;i<m_owned.size(); ++i)
			{
				if (m_owned[i] == -1)
				{
					m_owned[i] = BlockManager::Get().allocate();
				}
				auto& block = BlockManager::Get().get_noLock(i);
				locks.emplace_back(ReadLock(block.getMutex()));
				for (unsigned i = 0;i< block.size();++i)
				{
                    reference.push_back(block.get_noLock(i));
				}
			}

		}
	}
	Record Table::select(vector<string>&, bool raw)
	{

	}
	void Table::setSize(unsigned newSize)
	{
		WriteLock lock(m_mutex);
        m_size = newSize;
	}
	ShareMutex& Table::getMutex()
	{
		return m_mutex;
	}
	unsigned Table::blockSize()const
	{
		ReadLock lock(m_mutex);
		return m_owned.size();
	}
	Table& Table::operator=(Table&& object) noexcept
	{
		WriteLock lock(m_mutex);
		m_name = std::move(object.m_name);
        m_available = std::move(object.m_available);
        m_permission = object.m_permission;
        m_size = object.m_size;
        m_ownerId = std::move(object.m_ownerId);
        m_byteSize = object.m_byteSize;
        m_primedSize = object.m_primedSize;
        m_recordByte = object.m_recordByte;
        m_columns = std::move(object.m_columns);
        m_pendingOperations = std::move(object.m_pendingOperations);
		return *this;
	}
	Table::~Table()
	{
		clear();
	}
}
