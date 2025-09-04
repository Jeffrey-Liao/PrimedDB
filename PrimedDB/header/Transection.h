#pragma once
#include "Global.h"
namespace liao::PrimedDB
{
	class Session;
	class Schema;
	DYNAMIC
	concept Concept_TransectionTypeRequired = requires
	{
		std::same_as<T, PrimedDB::Column>
			|| std::same_as<T, PrimedDB::User>
			|| std::same_as<T, PrimedDB::Table>
			|| std::same_as<T, PrimedDB::Schema>
			|| std::same_as<T,PrimedDB::Session>;
	};
	DYNAMICCON(Concept_TransectionTypeRequired)
	class Transection
	{
		User& m_operator;
		std::string m_id;
		T& m_target;
		std::string m_operation;

		ShareMutex m_mutex;
	public:
		Transection(User& m_operator,T& target,std::string& operation)
			: m_operator(m_operator),m_target(target),m_operation(std::move(operation))
		{
			m_id = StaticFunc::GetUniqueId();
		}
		std::string toString()
		{
			
		}
	};
}
