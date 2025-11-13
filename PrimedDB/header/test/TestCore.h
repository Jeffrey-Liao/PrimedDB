#pragma once
#include "Defs.h"
namespace liao::Test
{
	class TestCore
	{
	protected:
		bool m_total;
		DYNAMIC
		bool test(T expected, const std::function<T>& func())
		{
			return expected == func();
		}
		static void PrintSuccess(const std::string&);
		static void PrintFail(const std::string&);
	};
}

