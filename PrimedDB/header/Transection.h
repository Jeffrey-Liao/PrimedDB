#pragma once
#include "Global.h"
#include "User.h"

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

	enum class TransectionType
	{
		Update,
		Insert,
        Delete
	};
	//DYNAMICCON(Concept_TransectionTypeRequired)
	class Transection
	{
		std::string m_id;
		TransectionType m_operation;
        User& m_operator;
		std::function<void()> m_function;
		std::atomic<bool> m_valid = true;
	public:
		template<class F, class... Args>
		Transection(User& m_operator, TransectionType operation, F&& func, Args&&... args)
			: m_operator(m_operator),m_operation(operation),m_id(StaticFunc::GetUniqueId(Math::HashType::MD5))
		{
			m_function = [func = std::forward<F>(func), args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable
				{
						std::apply(func, args_tuple);
				};
		}
		bool compare(Transection& other)const
		{
			return m_id == other.m_id;
		}
		bool valid()const
		{
			return m_valid;
		}
		void commit()
		{
			m_function();
		}
		void rollback()
		{
			
		}
	};
}
