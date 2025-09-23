#pragma once
#include "User.h"

namespace liao::PrimedDB
{
	class Session
	{
		UserPtr m_user;
		std::string m_id;
		std::thread m_thread;
		std::atomic_bool m_flag;
		std::string m_ip;
		void operation();
	public:
		Session();
		Session(const UserPtr& user);
		Session(Session&& other);
		void execute();
		void terminate();
		bool isEmpty()const;
		bool compare(Session& other)const;
		bool compare(const UserPtr& user) const;
		Session& operator=(Session&& other);
		bool operator==(const UserPtr& user)const;
		bool operator==(Session& other)const;
		~Session();
	};
}
