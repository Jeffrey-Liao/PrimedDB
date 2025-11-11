#include "Setting.h"
#include "UserManager.h"
USESTD;
namespace liao::PrimedDB
{
	UserPtr UserManager::System = nullptr;
	UserPtr UserManager::constructSystem(string& fileLine)
	{
		if (fileLine.empty())
			System = make_shared<User>(User::createUser("system", Util::Setting::Get().getSystemPassword(), UserLevel::System, std::move(User::PassWordHash("system"))));
		else
			System = make_shared<User>(fileLine);
		m_allUsers[System->getName()] = System;
		return System;
	}
	
	UserManager::UserManager()
	{
		StaticFunc::WriteInfo("UserManager", "Initializing UserManager.");
		fstream file(Util::Setting::Get().getUserFile(), ios::out | ios::in);
		string cache, name;
		UserPtr ptr = constructSystem(cache);
		if (!file.fail())
		{
			while (!file.eof())
			{
				cache = "";
				getline(file, cache);
                if (cache.empty())
                {
					break;
                }
				cache.find_first_of(":");
				name = std::move(cache.substr(0,cache.find_first_of(":")));
				StaticFunc::WriteInfo("UserManager", std::format("User [{}] loaded.",name));
				if (name == "system")
				{
					ptr = constructSystem(cache);
				}
				else
					ptr = make_shared<User>(cache);
				m_allUsers[name] = ptr;
			}
			if (m_allUsers.empty())
			{
				file.clear();
				ptr = constructSystem(cache);
				file << ptr->toString() << endl;
			}
		}
        file.close();
	}
	User& UserManager::GetSystemUser()
	{
		return *System;
	}
	bool UserManager::exist(const std::string& name)const
	{
		ReadLock lock(m_mutex);
		return m_allUsers.contains(name);
	}

	bool UserManager::InvalidName(std::string& name)
	{
		string cmp = name;
		ranges::transform(cmp.begin(), cmp.end(), cmp.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return  name.empty()|| !isalpha(name[0]) || name.length() <= 3 || name.length() > USER_NAME_LEN || cmp == "null" || cmp == "system";
	}
	bool UserManager::isSafePassword(std::string& password)
	{
		bool punctuation = ranges::any_of(password.begin(), password.end(), [](char c) { return ispunct(static_cast<unsigned char>(c)); }),
			number = ranges::any_of(password.begin(), password.end(), [](char c) { return isalnum(static_cast<unsigned char>(c)); });
		//password should longer than 8 and contain at least one punctuation and one number
		return password.length()>=8 && number && punctuation;
	}
	int UserManager::userCount() const
	{
		ReadLock readLock(m_mutex);
		return m_allUsers.size();
	}

	UserPtr UserManager::create(std::string& name, std::string& password,UserLevel level)
	{
		if (exist(name))
			return m_allUsers[name];
		if (InvalidName(name)||!isSafePassword(password))
		{
			return nullptr;
		}
		string construName = name;
		UserPtr ptr(new User(construName, password, level));
		fstream file(Util::Setting::Get().getUserFile(),ios::out | ios::app);
		if (!(file.fail()&&file.is_open()))
			file << ptr->toString() << endl;
		file.close();
		WriteLock lock(m_mutex);
		m_allUsers.emplace(name, ptr);
        StaticFunc::WriteInfo("UserManager", std::format("User [{}] created.",name));
		return m_allUsers[name];
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

	////////////////////////////////////////////////////////////////////
	bool UserManager::remove(const User& executor, const std::string& who)
	{
		if (exist(who)&&levelQualified(executor.getLevel(),who)&&m_allUsers[who].use_count() == 1)
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
		return user->validate(User::PassWordHash(password));
	}
	bool UserManager::login(const std::string& name, const std::string& password)
	{
		if (allowLogin(name, password))
		{
			StaticFunc::WriteInfo("UserManager", std::format("User [{}] logged in.",name));
			return true;
		}
		StaticFunc::WriteInfo("UserManager", std::format("User [{}] login failed.", name));
		return false;
	}
	/////////////////////////////////////////////////////
	bool UserManager::logout(const std::string& name)
	{
		return true;
	}
	void UserManager::save()
	{
		ofstream file(Util::Setting::Get().getUserFile(),ios::trunc);
		WriteLock lock(m_mutex);
		{
			for (auto& user : m_allUsers)
			{
				file << user.second->toString() << "\n";
			}
		}
		file.close();
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
