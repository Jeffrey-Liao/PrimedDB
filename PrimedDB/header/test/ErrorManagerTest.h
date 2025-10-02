#pragma once
#pragma once
#include "ErrorManager.h"
namespace liao::Test
{
	class ErrorManagerTest
	{
		Util::ErrorManager& m_errorManager = Util::ErrorManager::Get();
	public:
		void set_error_Test();
		void set_level_name_message_Test();
		void run();
	};
}
