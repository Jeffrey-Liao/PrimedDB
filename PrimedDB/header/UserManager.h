#pragma once
#include "Session.h"
namespace liao::PrimedDB
{
	class Session;
	class UserManager:public Singleton<UserManager>
	{
		std::unordered_map<std::string,std::shared_ptr<User>>m_allUsers;
		std::vector<Session> m_took;
		static User System;
		mutable ShareMutex m_mutex;

		bool allowLogin(const std::string& name,const std::string& password);
		bool allowControl(UserLevel operatorLevel, const std::string& name);
		static bool InvalidName(std::string& name);
		static bool IsSavePassword(std::string& password);
	public:

		bool exist(const std::string& name) const;
		std::shared_ptr<User> create(std::string& name,std::string& password, UserLevel userLevel);
		bool remove(const User& executor,const std::string& who);
		bool login(const std::string& name,const std::string& password);
		bool logout(const std::string& name);
		bool forceLogout(const User& executor, const std::string& who);
		auto userInSession(const std::string& who);
		bool isUserInSession(const std::string& who);
		int userCount() const;
		int sessionCount() const;
		std::shared_ptr<User> get(const std::string& name);
        const std::shared_ptr<User> get(const std::string& name) const;
		static const User& GetSystemUser();
	};
}
