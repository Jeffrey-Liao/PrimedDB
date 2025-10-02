#pragma once
#include "Defs.h"

namespace liao::Util
{
	class AsyncService
	{
		std::condition_variable m_cv;
		std::mutex m_cvMutex;
		std::future<void> m_terminate;
		std::atomic<bool> m_notification;
	public:
		virtual void service() =0;
	};
}
