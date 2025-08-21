#pragma once
#include "UserManager.h"
namespace liao ::PrimedDB
{
	class Session
	{
		std::shared_ptr<User> m_user;
		std::string m_id;
		std::thread m_thread;
		std::atomic_bool m_flag;

	public:
		void execute();
		void terminate();
		bool compare(Session& other)const;
		bool operator==(Session& other)const;
	};
}
