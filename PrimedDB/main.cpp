#include "Server.h"
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
bool isAnotherInstanceRunning(const char* mutexName = "PrimedDB")
{
	HANDLE hMutex = CreateMutexA(nullptr, TRUE, mutexName);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		return true; // 已有实例
	}
	// 注意：不要关闭 hMutex！否则锁会释放
	return false;
}

int main()
{
	if (!isAnotherInstanceRunning())
		serverStart();
	else
		cout << "Primed DB only allow one instance running at the same time. :(\n";
	system("pause");


}