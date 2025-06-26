#include "header/Log.h"
#include <thread>
using namespace liao::message;
using namespace std;
void t1()
{
	for (int n = 0; n < 10000; ++n)
	{
		Log& ref = Log::Get();
		ref[LogType::Error](THISFUNC).openToFile("t1.log") << to_string(n) << "Hello, World!" << Log::logEndl;
	}
}
void t2()
{
	for (int n = 0; n < 10000; ++n)
	{
		Log& ref = Log::Get();
		ref[LogType::Debug](THISFUNC).openToFile("t2.log") <<to_string(n)<< "Hello, World!" << Log::logEndl;
	}

}
int main()
{	
	thread th1(t1);
	thread th2(t2);
	th1.join();
	th2.join();
}
