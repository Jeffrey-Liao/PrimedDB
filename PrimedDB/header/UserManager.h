#pragma once
#include "User.h"
namespace liao::PrimedDB
{
	class UserManager:public Singleton<UserManager>
	{
		std::unordered_map<std::string,std::shared_ptr<User>>m_allUsers;
		std::vector<std::shared_ptr<User>> m_took;
		static User System;


		bool allowLogin(const std::string& name,const std::string& password);
		bool allowControl(UserLevel operatorLevel, const std::string& name);
		bool invalidName(std::string& name) const;
		bool isSavePassword(std::string& password)const;
	public:

		bool exist(const std::string& name) const;
		std::shared_ptr<User> create(std::string& name,std::string& password, UserLevel userLevel);
		bool remove(const User& executor,const std::string& who);
		bool login(const std::string& name,const std::string& password);
		bool logout(const std::string& name);
		bool forceLogout(const User& executor, const std::string& who);
		auto userInSession(const std::string& who);
		bool isUserInSession(const std::string& who);
		std::shared_ptr<User> get(const std::string& name);
        const std::shared_ptr<User> get(const std::string& name) const;
		static const User& GetSystemUser();
	};
}
