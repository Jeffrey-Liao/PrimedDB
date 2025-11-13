#include "test/TestCore.h"
#include "Log.h"
using namespace liao::Infor;
namespace liao::Test
{
	void TestCore::PrintSuccess(const std::string& message)
	{
		Log::Get()[LogType::Info]<<"LiaoTest:  "<<message << "PASSED"<<Log::LogEndl;
	}
	void TestCore::PrintFail(const std::string& message)
	{
		Log::Get()[LogType::Info] << "LiaoTest:  " << message << "FAILED" << Log::LogEndl;
	}
}
