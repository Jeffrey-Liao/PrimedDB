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

	enum class TransectionType
	{
		None,
		Update,
		Insert,
		Delete
	};
	//DYNAMICCON(Concept_TransectionTypeRequired)
	class Transection
	{
		std::string m_id;
		std::string m_table;
		TransectionType m_operation;
		std::string m_operator;
		int m_blockId;
		int m_location;
		bool m_valid;
		unsigned m_size;
		SCharPtr m_memory;

	public:
		Transection();
		Transection(const std::string& oprtor,const std::string&, TransectionType operation, int blockId, int location, unsigned size = 0, UCharPtr memory = nullptr);
		Transection(const std::string& oprtor, const std::string& name,const std::string& str, bool neg = false);
		Transection(const std::string& oprtor, const std::string& name, bool neg = false);
		Transection(Transection&&);
		void operator=(Transection&&);
		TransectionType getType() const;
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
        ~Transection();
	};
}
