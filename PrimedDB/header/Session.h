#pragma once
#include "User.h"
namespace liao ::PrimedDB
{
	class Session
	{
		std::shared_ptr<User> m_user;
		std::string m_id;
		std::thread m_thread;
		std::atomic_bool m_flag;
		std::string m_ip;
		void operation();
	public:
		Session();
		Session(const std::shared_ptr<User>& user);
		void execute();
		void terminate();
		bool isEmpty()const;
		bool compare(Session& other)const;
		bool compare(const std::shared_ptr<User>& user) const;
		bool operator==(const std::shared_ptr<User>& user)const;
		bool operator==(Session& other)const;
	};
}
