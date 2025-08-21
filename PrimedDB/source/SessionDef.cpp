#include "Session.h"
USESTD;
namespace liao::PrimedDB
{
	Session::Session()
		:m_id(StaticFunc::GetUniqueId()),m_user(nullptr),m_flag(false),m_ip("")
	{
		execute();
	}
	Session::Session(const std::shared_ptr<User>& user)
		:m_id(StaticFunc::GetUniqueId()),m_user(user), m_flag(false),m_ip("")
	{
		execute();
	}
	bool Session::isEmpty()const
	{
		return m_user == nullptr;
	}
	void Session::operation()
	{
		while (!m_flag)
		{
			cout<<"Session "<<m_id<<" is running..."<<endl;
		}
	}
	Session::Session(Session&& other)
		:m_id(std::move(other.m_id)),m_user(std::move(other.m_user)),m_flag(other.m_flag.exchange(true)),m_ip(std::move(other.m_ip)),m_thread(std::move(other.m_thread))
	{
		
	}
	Session& Session::operator=(Session&& other)
	{
		m_id = std::move(other.m_id);
		m_user = std::move(other.m_user);
		m_flag = other.m_flag.exchange(true);
		m_ip = std::move(other.m_ip);
		m_thread = std::move(other.m_thread);
		return *this;
	}
	void Session::execute()
	{
		m_thread = std::thread(&Session::operation,this);
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
	Session::~Session()
	{
		terminate();
	}
}
