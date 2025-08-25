#include "Global.h"
#include "UserManager.h"
#include "TimeStamp.h"
#include <iostream>
USELIAO;
USELIAOUTIL;
USESTD;
USELIAOMATH;
USELIAOPRIMED;
static ShareMutex mutexMain;
//void genId()
//{
//    auto id = StaticFunc::GetUniqueId(HashType::SHA256);
//    SimpleLock lock(mutexMain);
//    cout << id << "\n";
//}
int main()
{
	Util::TimeStamp timeStamp;
	auto time = Util::TimeStamp::now();
    timeStamp.reset(time);
	cout<<timeStamp.getString()<<endl;
	cout << timeStamp.get(Util::TimeType::Year) << endl;
	cout << timeStamp.get(Util::TimeType::Month) << endl;
	cout << timeStamp.get(Util::TimeType::Day) << endl;
	cout << timeStamp.get(Util::TimeType::Hour) << endl;
	cout << timeStamp.get(Util::TimeType::Minute) << endl;
	cout << timeStamp.get(Util::TimeType::Second) << endl;
	timeStamp.add(chrono::hours(2));
	cout<<timeStamp.getString()<<endl;
}