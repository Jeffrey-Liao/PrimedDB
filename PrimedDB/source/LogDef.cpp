#include "Log.h"
#include <chrono>
#include <iostream>
#include <ctime>
#include <sstream>
using namespace std;
namespace liao::message
{
	Log Log::Logger;
	mutex Log::LogMutex;	
	mutex Log::LogStream::LogStreamMutex;
	static mutex timeMutex;
	static string GetTime()
	{
		lock_guard<mutex> lock(timeMutex);
		time_t now = time(nullptr);
		tm local_time;
		localtime_s(&local_time, &now);
		ostringstream oss;
		oss << put_time(&local_time, "[%F %T]");  // 格式示例: [2023-04-05 15:30:45]
		return oss.str();
	}
	Log::Log()
	{
		if (!fs::exists(LOG_FOLDER))
			fs::create_directories(LOG_FOLDER);
		//fatalFile.open(LOG_FOLDER.append(FATAL_LOG_FILE), std::ios::app);
	}
	Log::~Log()
	{
	}
	Log& Log::Get()
	{
		return Logger;
	}
	void Log::print(const string& message)
	{
		cout<< message;
	}
	void Log::print(string&& message)
	{
		print(message);
	}
	void Log::printLine(const string& message)
	{
		cout << format("{} - {}",GetTime(),message)<< endl;
	}
	void Log::printLine(string&& message)
	{
		printLine(message);
	}

	void Log::printError(string&& message,string&& fileName)
	{
		printError(message,fileName);
	}

	void Log::printDebug(string&& message, string&& fileName)
	{
		printDebug(message,fileName);
	}

	void Log::printMessage(string&& message, string&& fileName)
	{
		printMessage(message, fileName);
	}

	void Log::printError(const std::string& message, string& fileName)
	{
		string cache;
		{
			std::lock_guard<std::mutex> lock(LogMutex);
			cache = format("{} - [{}]: {}", GetTime(), "Error", message);
			LogStream temp = LogStream(cache);
			temp.openToFile(fileName);
			logEndl(temp);
		}

	}
	void Log::printDebug(const std::string& message, string& fileName)
	{
		string cache;
		{
			std::lock_guard<std::mutex> lock(LogMutex);
			cache = format("{} - [{}]: {}", GetTime(), "Debug", message);
			LogStream temp = LogStream(cache);
			temp.openToFile(fileName);
			logEndl(temp);
		}

	}
	void Log::printMessage(const std::string& message, string& fileName)
	{
		string cache;
		{
			std::lock_guard<std::mutex> lock(LogMutex);
			cache = format("{} - [{}]: {}", GetTime(), "Message", message);
			LogStream temp = LogStream(cache);
			temp.openToFile(fileName);
			logEndl(temp);
		}
	}

	void Log::printError(ClassInfor& error, const std::string& message, string& fileName)
	{
		printError(format("in {}that {}",error.CompleteInfor(), message),fileName);
	}
	void Log::printDebug(ClassInfor& error, const std::string& message, string& fileName)
	{
		printDebug(format("in {}that {}", error.CompleteInfor(), message), fileName);
	}
	void Log::printMessage(ClassInfor& error, const std::string& message, string& fileName)
	{
		printMessage(format("in {}that {}", error.CompleteInfor(), message),fileName);
	}

	void Log::printError(ClassInfor& error, std::string&& message, string&& fileName)
	{
		printError(format("in {}that {}", error.CompleteInfor(), message),fileName);
	}
	void Log::printDebug(ClassInfor& error, std::string&& message, string&& fileName)
	{
		printDebug(format("in {}that {}", error.CompleteInfor(), message),fileName);
	}
	void Log::printMessage(ClassInfor& error, std::string&& message,string&& fileName)
	{
		printMessage(format("in {}that {}", error.CompleteInfor(), message),fileName);
	}

	Log::LogStream Log::operator[](LogType type)
	{
		return LogStream(type);
	}
	Log::LogStream Log::operator[](string&& notation)
	{
		return LogStream(notation);
	}

	void Log::logEndl(LogStream& obj)
	{
		cout << obj.cache << endl;
		if (obj.logFile.is_open())
		{
			std::lock_guard<std::mutex> lock(obj.LogStreamMutex);
			obj.logFile << obj.cache << endl;
		}
	}
	Log::LogStream& Log::LogStream::openToFile(string&& name)
	{
		return openToFile(name);
	}
	Log::LogStream& Log::LogStream::openToFile(string& name)
	{
		if (name != "")
		{
			auto curPath = fs::current_path() / LOG_FOLDER;
			curPath.append(name);
			logFile.open(curPath, ios::app);
		}
		return *this;
	}


	string Log::LogStream::getLabel(LogType type) const
	{
		if(type == LogType::Error)
			return "Error";
		else if (type == LogType::Debug)
			return "Debug";
		else if (type == LogType::Message)
			return "Message";
		else
			return "";
	}

	Log::LogStream::LogStream(LogType type)
		:TYPE(type), cache(format("{} - [{}]:", GetTime(), getLabel(type)))
	{}
	Log::LogStream::LogStream(string& message)
		:TYPE(LogType::None), cache(message)
	{}
	Log::LogStream& Log::LogStream::operator()(ClassInfor& error)
	{
		cache += format(" in {} that", error.CompleteInfor());
		return *this;
	}
	Log::LogStream& Log::LogStream::operator<<(const std::string&& message)
	{
		cache += " ";
		cache += message;
		return *this;
	}
	Log::LogStream& Log::LogStream::operator<<(const std::string& message)
	{
		cache += " ";
		cache += message;
		return *this;
	}
	Log::LogStream& Log::LogStream::operator()(const char* classInfor)
	{
		ClassInfor error(classInfor);
		cache += format("in {}that", error.CompleteInfor());
		return *this;
	}
	LogType Log::LogStream::getType() const
	{
		return TYPE;
	}
	void Log::LogStream::operator<<(void (*p)(LogStream&))
	{
		p(*this);
	}
	Log::LogStream::~LogStream()
	{
		logFile.close();
	}
}