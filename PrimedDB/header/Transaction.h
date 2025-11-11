#pragma once
#include "Global.h"

namespace liao::PrimedDB
{
	//DYNAMIC
	//	concept Concept_TransectionTypeRequired = requires
	//{
	//	std::same_as<T, PrimedDB::Column>
	//		|| std::same_as<T, PrimedDB::User>
	//		|| std::same_as<T, PrimedDB::Table>
	//		|| std::same_as<T, PrimedDB::Schema>
	//		|| std::same_as<T, PrimedDB::Session>;
	//};

	enum class SQLType
	{
		Select,
		Insert,
		Update,
		Delete,

		Create,
		None
	};
	
	class Transaction
	{
		std::string m_id;
		std::string m_table;
		SQLType m_operation;
		std::string m_operator;
		int m_blockId;
		int m_location;
		bool m_valid;
		unsigned m_size;
		SCharPtr m_memory;

	public:
		Transaction();
		Transaction(const std::string& oprtor,const std::string&, SQLType operation, int blockId, int location, unsigned size = 0, UCharPtr memory = nullptr);
		Transaction(const std::string& oprtor, const std::string& name,const std::string& str, bool neg = false);
		Transaction(const std::string& oprtor, const std::string& name, bool neg = false);
		Transaction(Transaction&&) noexcept;
		void operator=(Transaction&&) noexcept;
		SQLType getType() const;
		const std::string& getId() const;
		const std::string& getOperator()const;
		const std::string& getTable() const;
		int getBlockId() const;
		void setBlockId(int);
		int getLocation() const;
		unsigned size() const;
		bool isValid() const;
		SCharPtr getMemory();
		void setValid();
		void setInvalid();
		std::string toString() const;
		void fromString(const std::string& str);
		void negFromString(const std::string& str);
        ~Transaction();
	};
}
