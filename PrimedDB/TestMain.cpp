//#include "Global.h"
//#include "UserManager.h"
//#include "TimeStamp.h"
//#include <iostream>
//USELIAO;
//USELIAOUTIL;
//USESTD;
//USELIAOMATH;
//USELIAOPRIMED;
//static ShareMutex mutexMain;
////void genId()
////{
////    auto id = StaticFunc::GetUniqueId(HashType::SHA256);
////    SimpleLock lock(mutexMain);
////    cout << id << "\n";
////}
//int main()
//{
//	Util::TimeStamp timeStamp;
//	auto time = Util::TimeStamp::SystemTime();
//    timeStamp.reset(time);
//	cout<<timeStamp.getString()<<endl;
//	cout << timeStamp.get_noLock(Util::TimeType::Year) << endl;
//	cout << timeStamp.get_noLock(Util::TimeType::Month) << endl;
//	cout << timeStamp.get_noLock(Util::TimeType::Day) << endl;
//	cout << timeStamp.get_noLock(Util::TimeType::Hour) << endl;
//	cout << timeStamp.get_noLock(Util::TimeType::Minute) << endl;
//	cout << timeStamp.get_noLock(Util::TimeType::Second) << endl;
//	timeStamp.add(chrono::hours(2));
//	cout<<timeStamp.getString()<<endl;
//}
#include "Server.h"
#include "Log.h"
#include "Timer.h"
#include "TableManager.h"
#include "UserManager.h"
#include <chrono>
#include <iostream>
USESTD;
USELIAO;
void serverStart()
{
	asio::io_context io_context;
	Net::Server server(io_context, "localhost", 313);
	string input;
	server.start();
	std::thread io_thread([&io_context]()
		{
			asio::executor_work_guard<asio::io_context::executor_type> work_guard =
				asio::make_work_guard(io_context);
			io_context.run();
		});
	while (std::getline(std::cin, input)) {
		if (input == "exit") {
			io_context.stop();
			break;
		}
	}
	io_thread.join();
}
void sqlTest()
{
	auto system = PrimedDB::UserManager::Get().get("system");

	if (system != nullptr)
	{
		string sql = "select * from JEFF where id = 123";
		sql = "CREATE TABLE JEFF (id INT 4, name VARCHAR 10, gender INT 4)";
		future<Compiler::Result> result;
		Compiler::Result reply;
		
		auto& tableM = PrimedDB::TableManager::Get();
		for (int n = 0;n<100;++n)
		{
			sql = format("INSERT INTO LIAO (id, name, gender) VALUES ({},'Hello', 1)", 99);
			result = Compiler::Compiler::Get().compile(system, sql);
			reply = result.get();
		}
		system->commit();
		sql = "select * from LIAO";
		result = Compiler::Compiler::Get().compile(system, std::move(sql));
		reply = result.get();
		cout << reply.m_message;
		this_thread::sleep_for(chrono::seconds(3));
	}
}
int main()
{
	serverStart();
	//sqlTest();
}