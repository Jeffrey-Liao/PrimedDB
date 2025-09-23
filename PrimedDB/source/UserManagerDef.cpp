#include "UserManager.h"
USESTD;
namespace liao::PrimedDB
{
	
	User UserManager::System = User::createUser("system", "null", UserLevel::System);
	UserManager::UserManager()
	{
		ifstream file(Configuration::UserInforFile);
		string cache;
		if (!file.fail())
		{
			while (file.eof())
			{
				getline(file, cache);
                if (cache.empty())
                    break;
				vector<string> token;
                StaticFunc::Split(token, cache, ':');
				m_allUsers.insert(token[0],cache);
			}
		}
	}
	const User& UserManager::GetSystemUser()
	{
		if (System.getName() == "system")
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
		ReadLock lock(m_mutex);
		return m_allUsers.contains(name);
	}
	auto UserManager::userInSession(const std::string& who)
	{
		ReadLock lock(m_mutex);
		return std::find(m_took.begin(), m_took.end(), m_allUsers[who]);
	}
	bool UserManager::isUserInSession(const std::string& who)
	{
		auto iter = userInSession(who);
		ReadLock lock(m_mutex);
		return iter != m_took.end();
	}
	bool UserManager::InvalidName(std::string& name)
	{
		string cmp = name;
		ranges::transform(cmp.begin(), cmp.end(), cmp.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return name.empty() || name.length() <= 3 || name.length() > USER_NAME_LEN || cmp == "null" || cmp == "system";
	}
	bool UserManager::isSafePassword(std::string& password)
	{
		return password.length()>=8 && 
			ranges::any_of(password.begin(), password.end(), [](char c){return ispunct(static_cast<unsigned char>(c));})&& 
			ranges::any_of(password.begin(), password.end(), [](char c) { return isalnum(static_cast<unsigned char>(c));});
	}
	int UserManager::userCount() const
	{
		ReadLock readLock(m_mutex);
		return m_allUsers.size();
	}
	int UserManager::sessionCount() const
	{
		ReadLock readLock(m_mutex);
		return m_took.size();
	}
	UserPtr UserManager::create(std::string& name, std::string& password,UserLevel level)
	{
		if (exist(name)||InvalidName(name)||!isSafePassword(password))
		{
			return nullptr;
		}
		string construName = name;
		UserPtr ptr(new User(construName, password, level));
		{
			WriteLock lock(m_mutex);
			m_allUsers.emplace(name, ptr);
		}
		ReadLock lock(m_mutex);
		return m_allUsers[name];
	}
	bool UserManager::allowControl(UserLevel operatorLevel, const std::string& name)
	{
		return isUserInSession(name) && levelQualified(operatorLevel,name);
	}
	bool UserManager::levelQualified(UserLevel level, const std::string& name)
	{
		UserLevel userLevel;
		{
			ReadLock lock(m_mutex);
			userLevel = m_allUsers[name]->getLevel();
		}
		return level >= UserLevel::Manager && level > userLevel;
	}
	bool UserManager::remove(const User& executor, const std::string& who)
	{
		auto iter = userInSession(who);
		if (iter != m_took.end())
		{
			WriteLock lock(m_mutex);
			m_took.erase(iter);
		}
		if (exist(who)&&levelQualified(executor.getLevel(),who))
		{
			WriteLock lock(m_mutex);
			m_allUsers.erase(who);
			return true;
		}
		return false;
	}
	bool UserManager::allowLogin(const std::string& name, const std::string& password)
	{
		shared_ptr<User> user;
		if (!exist(name))
			return false;
		{
            ReadLock lock(m_mutex);
			user = m_allUsers[name];
		} 
		return !isUserInSession(name) && user->validate(User::PassWordHash(password));
	}
	bool UserManager::login(const std::string& name, const std::string& password)
	{
		if (allowLogin(name, password))
		{
			WriteLock lock(m_mutex);
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
			WriteLock lock(m_mutex);
			m_took.erase(iter);
			return true;
		}
		return false;
	}
	void UserManager::save()
	{
		ofstream file(Configuration::UserInforFile,ios::trunc);
		WriteLock lock(m_mutex);
		{
			for (auto& user : m_allUsers)
			{
				file << user.second->toString() << "\n";
			}
		}
		file.close();
	}
	bool UserManager::forceLogout(const User& executor, const std::string& who)
	{
		if (allowControl(executor.getLevel(),who))
		{
			WriteLock lock(m_mutex);
			m_took.erase(userInSession(who));
			return true;
		}
		return false;
	}
	UserPtr UserManager::get(const std::string& name)
	{
		ReadLock lock(m_mutex);
		return m_allUsers[name];
	}
	const UserPtr UserManager::get(const std::string& name) const
	{
        ReadLock lock(m_mutex);
		return m_allUsers.find(name)->second;
	}
}
