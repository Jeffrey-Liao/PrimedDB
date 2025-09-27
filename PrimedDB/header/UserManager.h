#pragma once
#include "Session.h"
namespace liao::PrimedDB
{
	class Session;
	class UserManager:public Singleton<UserManager>
	{
		std::unordered_map<std::string,UserPtr>m_allUsers;
		std::vector<Session> m_took;
		static User System;
		mutable ShareMutex m_mutex;

		bool allowLogin(const std::string& name,const std::string& password);
		bool allowControl(UserLevel operatorLevel, const std::string& name);
		bool levelQualified(UserLevel level, const std::string& name);
		static bool InvalidName(std::string& name);
		static bool isSafePassword(std::string& password);
		void save();
		UserManager();
	public:

		bool exist(const std::string& name) const;
		UserPtr create(std::string& name,std::string& password, UserLevel userLevel);
		bool remove(const User& executor,const std::string& who);
		bool login(const std::string& name,const std::string& password);
		bool logout(const std::string& name);
		bool forceLogout(const User& executor, const std::string& who);
		auto userInSession(const std::string& who);
		bool isUserInSession(const std::string& who);
		int userCount() const;
		int sessionCount() const;
		const std::unordered_map<std::string, UserPtr> all()const;
		UserPtr get(const std::string& name);
        const UserPtr get(const std::string& name) const;
		static const User& GetSystemUser();
	};
}
