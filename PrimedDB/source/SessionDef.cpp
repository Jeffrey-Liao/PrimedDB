#include "Session.h"
USESTD;
namespace liao::PrimedDB
{
	Session::Session()
		:m_id(StaticFunc::GetUniqueId()),m_user(nullptr),m_flag(false),m_ip("")
	{}
	Session::Session(const std::shared_ptr<User>& user)
		:m_id(StaticFunc::GetUniqueId()),m_user(user), m_flag(false),m_ip("")
	{}
	bool Session::isEmpty()const
	{
		return m_user == nullptr;
	}
	void Session::operation()
	{
		while (!m_flag)
		{

		}
	}
	void Session::execute()
	{
		m_thread = thread(operation);
		m_thread.detach();
	}
	void Session::terminate()
	{
		m_flag = true;
	}
	bool Session::compare(Session& other)const
	{
		return m_id == other.m_id;
	}
	bool Session::compare(const std::shared_ptr<User>& user)const
	{
		return m_user == user;
	}
	bool Session::operator==(const std::shared_ptr<User>& user)const
	{
		return compare(user);
	}
	bool Session::operator==(Session& other)const
	{
		return compare(other);
	}
}
