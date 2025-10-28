#include "./test/ErrorManagerTest.h"
using namespace liao::Util;
using namespace std;
namespace liao::Test
{
	void ErrorManagerTest::set_error_Test()
	{
		string error = "TestError";
		string testErrorMessage = "A Test Error Happened";
		Error errorObject(ErrorLevel::Info, error, testErrorMessage,Infor::ClassInfor(THISFUNC,THISFILE));
		m_errorManager.set(errorObject);
		errorObject.m_level = ErrorLevel::Warning;
		m_errorManager.set(errorObject);
		errorObject.m_level = ErrorLevel::Error;
		m_errorManager.set(errorObject);
		errorObject.m_level = ErrorLevel::Fatal;
		m_errorManager.set(errorObject);
	}
	void ErrorManagerTest::set_level_name_message_Test()
	{
		string error = "TestError";
		string testErrorMessage = "A Test Error Happened";
		Error errorObject(ErrorLevel::Info, error, testErrorMessage, Infor::ClassInfor(THISFUNC, THISFILE));
		m_errorManager.set(errorObject);
		errorObject.m_level = ErrorLevel::Warning;
		m_errorManager.set(errorObject);
		errorObject.m_level = ErrorLevel::Fatal;
		m_errorManager.set(errorObject);
		errorObject.m_level = ErrorLevel::Error;
		m_errorManager.set(errorObject);
	}
	void ErrorManagerTest::run()
	{
		cout << format("Test 1") << endl;
		for (int n = 0;n<100000;++n)
		{
			set_error_Test();
			set_level_name_message_Test();
		}

	}
}