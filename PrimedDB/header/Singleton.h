#pragma once
#include "Defs.h"
namespace liao
{
	DYNAMIC
	class Singleton
	{
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator=(Singleton&&) = delete;
	protected:
		Singleton() = default;
		static ShareMutex m_mutex;

	public:
		static T& Get()
		{
			static T instance;

			return instance;
		}
	};
#define	INVITESINGLETON friend class Singleton;
}