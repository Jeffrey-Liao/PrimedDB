#pragma once
#include "Global.h"
namespace liao::PrimedDB
{
	class UserManager:public Singleton<UserManager>
	{
	public:
		UserManager() = delete;
		UserManager(const UserManager&) = delete;
		UserManager(UserManager&&) = delete;

	};
}
