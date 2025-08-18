#include "UserManager.h"
USESTD;
namespace liao::PrimedDB
{
	User UserManager::System;
	const User& UserManager::GetSystemUser()
	{
		if (System.getName() == User::GetNullRef().getName())
		{
			string str = "system";
			System.rename(str);
			str = StaticFunc::GetUniqueId();
			System.setPassword(str);
			System.setLevel(UserLevel::System);
		}
		return System;
	}
	bool UserManager::exist(const std::string& name)const
	{
		return m_allUsers.contains(name);
	}
	auto UserManager::userInSession(const std::string& who)
	{
		return std::find(m_took.begin(), m_took.end(), m_allUsers[who]);
	}
	bool UserManager::isUserInSession(const std::string& who)
	{
		return userInSession(who) == m_took.end();
	}
	bool UserManager::invalidName(std::string& name) const
	{
		string cmp = name;
		std::transform(cmp.begin(), cmp.end(), cmp.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return cmp.empty() || cmp.length() <= 3 || cmp.length() > 50 || cmp == "null" || cmp == "system";
	}
	bool UserManager::isSavePassword(std::string& password) const
	{
		return password.length()>=8 && 
			std::any_of(password.begin(), password.end(), [](char c){return ispunct(static_cast<unsigned char>(c));})&& 
			std::any_of(password.begin(), password.end(), [](char c) { return isalnum(static_cast<unsigned char>(c));});
	}
	std::shared_ptr<User> UserManager::create(std::string& name, std::string& password,UserLevel level)
	{
		if (exist(name)||invalidName(name)||!isSavePassword(password))
		{
			return nullptr;
		}
		string construName = name;
		shared_ptr<User> ptr(new User(construName, password, level));
		m_allUsers.emplace(name,ptr);
		return m_allUsers[name];
	}
	bool UserManager::allowControl(UserLevel operatorLevel, const std::string& name)
	{
		return isUserInSession(name) && operatorLevel >= UserLevel::Manager&&operatorLevel>m_allUsers[name]->getLevel();
	}
	bool UserManager::remove(const User& executor, const std::string& who)
	{
		if (allowControl(executor.getLevel(),who))
		{
			if (exist(who)&&executor.getLevel()>m_allUsers[who]->getLevel())
			{
				m_allUsers.erase(who);
				return true;
			}
		}
		return false;
	}
	bool UserManager::allowLogin(const std::string& name, const std::string& password)
	{
		return exist(name) && !isUserInSession(name) && m_allUsers[name]->validate(User::PassWordHash(password));
	}
	bool UserManager::login(const std::string& name, const std::string& password)
	{
		if (allowLogin(name, password))
		{
			m_took.emplace_back(m_allUsers[name]);
			return true;
		}
		return false;
	}
	bool UserManager::logout(const std::string& name)
	{
		auto iter = userInSession(name);
		if (iter!=m_took.end())
		{
			m_took.erase(iter);
			return true;
		}
		return false;
	}
	bool UserManager::forceLogout(const User& executor, const std::string& who)
	{
		if (allowControl(executor.getLevel(),who))
		{
			m_took.erase(userInSession(who));
			return true;
		}
		return false;
	}
	std::shared_ptr<User> UserManager::get(const std::string& name)
	{
		return m_allUsers[name];
	}
	const std::shared_ptr<User> UserManager::get(const std::string& name) const
	{
		return m_allUsers.find(name)->second;
	}
}
