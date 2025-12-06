#include "BlockManager.h"
#include "Table.h"
#include "User.h"
#include "UserManager.h"
USESTD;
namespace liao::PrimedDB
{
	Table::Table(Table&& move) noexcept
		:m_name(std::move(move.m_name)),m_available(std::move(move.m_available)),m_permission(move.m_permission), m_size(move.m_size), m_ownerId(std::move(move.m_ownerId)), m_columns(std::move(move.m_columns)),m_byteSize(move.m_byteSize),m_primedSize(move.m_primedSize),m_recordByte(move.m_recordByte)
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
	Table::Table(std::string ownerId, std::string& name, UserLevel permission, deque<Column>& columns)
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
		 
		m_size = labels[n] == "" ? 0 : stoi(labels[n++]);
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
		if (m_owned.empty())
		{
			return make_pair(-1, 0);
		}
		return make_pair(m_owned[position/Util::Setting::Get().getBlockSize()], position % Util::Setting::Get().getBlockSize());
	}
	void Table::intoString(int index, deque<deque<shared_ptr<string>>>& results, vector<char*>& block,bool raw)
	{
		Math::GmpBigNumber number;
		char* line;
		for (int n = index,sum = 0;n<block.size()&&n<m_size;++n)
		{
			results.emplace_back();
			auto& lineData = results.back();
			line = block[n];
			{
				ReadLock lock(m_mutex);
				if (!m_available[n])
					continue;
			}
			
			sum = 0;
			auto ptr = UCharPtr(line);
			deprimize(ptr, byte(), totalByte(), this->m_recordByte);
			for (auto& column:m_columns)
			{
				//Read primized data
				if (raw)
				{
					mpz_import(number.get_mpz_t(),
						column.primed(),              
						Math::isBigEndian() ? 1 : -1, 
						sizeof(char),             
						0,                        
						0,                        
						line+sum);
					lineData.emplace_back(make_shared<string>(std::move(number.get_str())));
					sum += column.primed();
				}
				//Read deprimized data
				else
				{
					string result;
					auto type = column.getType();
					if (type == DataType::Int)
					{
						Math::GmpBigNumber number;
						mpz_import(number.get_mpz_t(),
							column.size(),              // 字节数
							Math::isBigEndian() ? 1 : -1, // 字节序
							sizeof(char),             // 每个单位的大小
							0,                        // 无特定顺序
							0,                        // 无填充位
							ptr.get() + sum);
						result = number.get_str();
					}
					else if (type == DataType::Number)
					{
						double number;
						memcpy_s(&number, sizeof(double), ptr.get() + sum, column.size());
						result = std::to_string(number);
					}
					else if (type == DataType::Varchar)
					{
						result = std::string(ptr.get() + sum, column.size());
						result = result.substr(0,result.find_first_of('\0'));
					}
					lineData.emplace_back(make_shared<string>(result));
                    sum += column.size();
				}
				
			}
			auto recordNumber = recordByte();
			auto totalNumber = totalByte();
			//get record number
			mpz_import(number.get_mpz_t(),
				recordNumber,              // 字节数
				Math::isBigEndian() ? 1 : -1, // 字节序
				sizeof(char),             // 每个单位的大小
				0,                        // 无特定顺序
				0,                        // 无填充位
				line + totalNumber - recordNumber);
			lineData.emplace_back(make_shared<string>(std::move(number.get_str())));
		}
		return;
	}
	void Table::primize(UCharPtr& memory,unsigned byteSize,unsigned primedSize,unsigned recordSize)
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
				{
					convert = *(unsigned short*)(origin + n);
				}
				else
				{
					convert = (static_cast<uint32_t>(*(origin + n)) << 16) |
						(static_cast<uint32_t>(*(origin + n + 1 )) << 8) |
						(static_cast<uint32_t>(*(origin + n + 2)) << 0);
				}
			}
			else
				convert = convert = *(unsigned int*)(origin + n);

			string before = convert.get_str();
			Math::PrimeNumberConvert::generate_big(convert, Util::Setting::Get().getEnlargePower());
			StaticFunc::WriteInfo("primize", std::format("From {} to {}", before, convert.get_str()));
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
		memory.reset(ptr);
		memset(ptr, 0, byteSize);
		Math::GmpBigNumber convert;
		unsigned m = 0;
		for (unsigned n =0 ;n< primedSize&&m< byteSize;n+=8)
		{
			unsigned size = byteSize - m;
			if (size < 4)
			{
				if (size == 1)
					convert = static_cast<unsigned char>(origin[n]);
                else if (size == 2)
					convert = *(unsigned short*)(origin + n);
			    else
					convert = (static_cast<uint32_t>(origin[n]) << 16) |
						(static_cast<uint32_t>(origin[n + 1]) << 8) |
						(static_cast<uint32_t>(origin[n + 2]) << 0);
			}
			mpz_import(convert.get_mpz_t(), 8, Math::isBigEndian() ? 1 : -1, 1, 0, 0, origin+n);
			auto before = convert.get_str();
			convert /= Util::Setting::Get().getEnlargeSize();
			StaticFunc::WriteInfo("deprimize", std::format("From {} to {}", before, convert.get_str()));
			if (convert != 0)
				mpz_export(ptr + m, nullptr, Math::isBigEndian() ? 1 : -1, 1, 0, 0, convert.get_mpz_t());
			m += 4;
		}

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
	Column& Table::getColumn(const std::string& name)
	{
		return *findColumn(name);
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
	unsigned Table::decrementSize(unsigned pos)
	{
		WriteLock lock(m_mutex);
		m_size--;
		m_available[pos] = false;
		return m_size;
	}
	void Table::clear()
	{
		if (!BlockDead)
		{
			WriteLock lock(m_mutex);
			m_owned.clear();
			m_columns.clear();
		}

	}
	bool Table::read()
	{
		auto fileName = m_name + ".dat";
		fstream tableFile(Util::Setting::Get().getDataDirectory()/fileName, ios::in | ios::binary);
		if (tableFile.fail())
		{
			tableFile.close();
			if (tableFile.fail())
			{
				tableFile.open(Util::Setting::Get().getDataDirectory() / fileName, ios::out);
				tableFile << "";
				tableFile.close();
				BlockManager::Get().allocateWithOutRead(shared_from_this());
				return false;
			}


		}
		int count = 0;
		do
		{
			BlockManager::Get().allocateWithOutRead(shared_from_this());
			Block& block = BlockManager::Get().get_noLock(m_owned.back());
			tableFile.read(block.reference(), Util::Setting::Get().getBlockSize());
			if (tableFile.gcount() == 0 || tableFile.gcount() < Util::Setting::Get().getBlockSize())
				break;
			count += Util::Setting::Get().getBlockSize();
		}
		while (!tableFile.eof() && count < m_available.size());
		tableFile.close();
		return true;
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
	unsigned Table::columnSize() const
	{
		return m_columns.size();
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
	unsigned Table::recordByte()const
	{
		ReadLock lock(m_mutex);
		return m_recordByte;
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
	void Table::insert(const std::string& userName, UCharPtr memory, unsigned size)
	{
		auto blockPos = convertBlockPos(this->m_size);
		WriteLock lock(m_mutex);
		if (m_owned.empty() || m_available.size()/Util::Setting::Get().getBlockSize()+1 > m_owned.size())
			blockPos.first = -1;
		primize(memory,m_byteSize,m_primedSize,m_recordByte);
		StaticFunc::WriteInfo(m_name, std::format("Received request from {} to write data into block", userName));
		const char* ptr = memory.get();
		auto user = UserManager::Get().get(userName);
		if (user != nullptr)
		{
			user->submit(Transaction(userName, m_name, SQLType::Insert, blockPos.first, blockPos.second, size, std::move(memory)));
		}
	}
	void Table::update(const std::string& userName, unsigned position, UCharPtr memory, unsigned size)
	{
		auto blockPos = convertBlockPos(position);
		WriteLock lock(m_mutex);
		auto user = UserManager::Get().get(userName);
		user->submit(Transaction(userName, m_name, SQLType::Update, blockPos.first, blockPos.second, size, std::move(memory)));
	}
	void Table::remove(const std::string& userName, unsigned position)
	{
		auto blockPos = convertBlockPos(position);
		WriteLock lock(m_mutex);
		auto user = UserManager::Get().get(userName);
		user->submit(Transaction(userName, m_name, SQLType::Delete, blockPos.first, blockPos.second));
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
		tranFile.close();
		Transaction transection(name,m_name,buffer,true);
		BlockManager::Get().operate(transection);
	}
	void Table::addBlock(unsigned pos)
	{
		int index;
		string fileName;
		{
			WriteLock lock(m_mutex);
			m_owned.emplace_back(pos);
			index = m_owned.size() - 1;
			fileName = m_name + ".dat";
		}
		auto lineNumber = index * StaticFunc::MaxSizeForBlock(totalByte());
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
	std::vector<bool> Table::where(std::unordered_map<std::string, std::string>& column_value)
	{
		ReadLock lock(m_mutex);
		Math::GmpBigNumber tempRecordNum = 1,cmp;
		std::vector<bool> result(m_available.size(),true);
		for (auto& column:m_columns)
		{
			if (column_value.contains(column.getName()))
			{
				if (column.getType() == DataType::Int)
					tempRecordNum *= Math::PrimeNumberConvert::generate(stoi(column_value[column.getName()]), Util::Setting::Get().getEnlargePower());
				else if (column.getType() == DataType::Varchar)
					tempRecordNum *= Math::PrimeNumberConvert::generateFromString(column_value[column.getName()]);
			}
		}
		if (tempRecordNum == 1)
			return {};
		for (unsigned n = 0, m = 0;n<m_owned.size()&&m<m_size;++n)
		{
			if (m_owned[n] == -1)
				BlockManager::Get().allocate(shared_from_this(),n * StaticFunc::MaxSizeForBlock(totalByte()));
			auto& block = BlockManager::Get().get_noLock(n);
			char* records = block.reference();
			auto total = totalByte();
			for (int m = 0,c = 0;m<block.max();m+=total)
			{
				if (c >= m_available.size())
					break;
				if (!m_available[c])
					continue;
				char* current = records + m;
				mpz_import(cmp.get_mpz_t(),
					m_recordByte
					, Math::isBigEndian() ? 1 : -1, // 字节序
					sizeof(char),             // 每个单位的大小
					0,                        // 无特定顺序
					0,                        // 无填充位
					current + total - m_recordByte);
				if (cmp == 0 || cmp % tempRecordNum != 0)
				{
					result[c] = false;
				}
				c++;
			}
		}
		return result;
	}
	std::vector<Column>& Table::getColumns()
	{
		ReadLock lock(m_mutex);
		return m_columns;
	}
	std::string Table::format()
	{
		ReadLock lock(m_mutex);
		std::ostringstream oss;
		oss << m_name;
		for (unsigned n=0;n<m_columns.size();++n)
		{
			oss<<"|"<<m_columns[n].getName()<<":"<<static_cast<unsigned int>(m_columns[n].getType());
		}
		return oss.str();
	}
	void Table::setOwner(const std::string& ownerid)
	{
		WriteLock lock(m_mutex);
		m_ownerId = ownerid;
	}
	Record Table::select(bool raw)
	{

		deque<deque<shared_ptr<string>>> references;
		unordered_map<string, int> header;
		auto& blockManager = BlockManager::Get();
		int byteSize = 0;
		{
			ReadLock lock(m_mutex);
			for (int n = 0; n < m_columns.size(); ++n)
			{
				header[m_columns[n].getName()] = n;
			}
		}

		header["recordNum"] = header.size();
		int index = 0;
		deque<int> copyOwned;
		{
			ReadLock lock(m_mutex);
			copyOwned = m_owned;
		}

		for (auto& o : copyOwned)
		{
			auto& block = blockManager.get_noLock(o);
			{
				intoString(index*Util::Setting::Get().getBlockSize(),references, block.getRecords(), raw);
			}
			index++;
		}
		return Record(header, references);
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
		return *this;
	}
	Table::~Table()
	{
		clear();
	}
}
