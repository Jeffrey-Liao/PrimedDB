#include "./test/ErrorManagerTest.h"
using namespace liao::Util;
using namespace std;
namespace liao::Test
{
	void ErrorManagerTest::set_error_Test()
	{
		string error = "TestError";
		string testErrorMessage = "A Test Error Happened";
		Error errorObject(ErrorLevel::Info, error, testErrorMessage);
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
		m_errorManager.set(ErrorLevel::Info, error, testErrorMessage);
		m_errorManager.set(ErrorLevel::Warning, error, testErrorMessage);
		m_errorManager.set(ErrorLevel::Fatal, error, testErrorMessage);
		m_errorManager.set(ErrorLevel::Error, error, testErrorMessage);
	}
	void ErrorManagerTest::run()
	{
		cout << format("Test 1") << endl;
		set_error_Test();
        cout << format("Test 2") << endl;
        set_level_name_message_Test();
	}
}