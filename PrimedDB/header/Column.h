#pragma once
#include "Global.h"
namespace liao::PrimedDB
{
	enum class DataType :char
	{
		Null,
		Int,
		Varchar,
		Number,
		Text,
		Time,
		Date
	};
	class Column
	{
		unsigned int m_byteSize;
		std::string m_columnName;
		unsigned int m_primed;
		std::string m_owner;
		DataType m_type;
		mutable std::shared_mutex m_mutex;

		void constructFromFileLine(const std::string& line);
	public:
		Column(std::string& name, unsigned size, const std::string& owner, DataType);
		Column(const std::string&, const std::string&);
		Column(const Column&);
		Column(Column&&) noexcept;
		void rename(std::string&);
		void resize(unsigned);
		void setType(DataType);
		DataType getType() const;
		const std::string& getName() const;
        unsigned size() const;
		unsigned primed() const;
		const std::string& getOwner() const;
		const std::string& getId() const;
		std::string toString() const;
		bool operator<(const Column&);
		void operator=(const Column&&);
        ~Column() = default;
	};
	using ColumnPtr = std::shared_ptr<Column>;
}