#pragma once
#include "Global.h"
namespace liao::PrimedDB
{
	class DiskManager:public Singleton<DiskManager>
	{
		ShareMutex m_mutex;
		
	public:

	};
}
