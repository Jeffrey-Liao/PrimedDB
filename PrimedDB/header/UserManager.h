#pragma once
#include "Session.h"
namespace liao::PrimedDB
{
	class Session;
	class UserManager:public Singleton<UserManager>
	{
		INVITESINGLETON;
		std::unordered_map<std::string,UserPtr> m_allUsers;
		static UserPtr System;
		static ShareMutex SystemMutex;
		mutable ShareMutex m_mutex;
		//check given name and password
		bool allowLogin(const std::string& name,const std::string& password);
		//check given level is higher than Manager or not
		bool levelQualified(UserLevel level, const std::string& name);\
		//check given name is valid or not
		//this function will reject name which are:
		//1.empty string
		//2.not start with english characters
		//3.longer than maximum name length (50)
		//4.short than 3 characters
		//5.using reserved words like 'system' and 'null'
		static bool InvalidName(std::string& name);
		//check given password is safe or not
		//this function require password with:
        //1.longer than 8 characters
        //2.contains numbers,letters and special characters
		static bool isSafePassword(std::string& password);
		//update entire user file
		void save();

		UserPtr constructSystem(std::string& fileLine);
		UserManager();
	public:

		bool exist(const std::string& name) const;
		UserPtr create(std::string& name,std::string& password, UserLevel userLevel);
		bool remove(const User& executor,const std::string& who);
		bool login(const std::string& name,const std::string& password);
		bool logout(const std::string& name);
		int userCount() const;
		const std::unordered_map<std::string, UserPtr> all()const;
		UserPtr get(const std::string& name);
        const UserPtr get(const std::string& name) const;
		static User& GetSystemUser();
	};
}
