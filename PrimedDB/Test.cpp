#include "header/Log.h"
#include <thread>
using namespace liao::message;
using namespace std;
void t1()
{
	for (int n = 0; n < 100; ++n)
	{
		Log& ref = Log::Get();
		ClassInfor instance(THISFUNC);
		ref.printError(instance, "hello world","");
	}
}
void t2()
{
	for (int n = 0; n < 100; ++n)
	{
		Log& ref = Log::Get();
		ClassInfor instance(THISFUNC);
		ref.printError(instance, "hello world","");
	}

}
int main()
{	
	thread t1_(t1);
	thread t2_(t2);
	t1_.join();
	t2_.join();
}
