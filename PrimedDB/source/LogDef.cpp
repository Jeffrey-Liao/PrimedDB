#include <Log.h>
#include <chrono>
#include <iostream>
#include <ctime>
#include <sstream>

#include "Setting.h"
using namespace std;
namespace liao::Infor
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
	void Log::Print(const string& message)
	{
		cout<< message;
	}
	void Log::Print(string&& message)
	{
		Print(message);
	}
	void Log::PrintLine(const string& message)
	{
		cout << format("{} - {}",GetTime(),message)<< endl;
	}
	void Log::PrintLine(string&& message)
	{
		PrintLine(message);
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
			LogEndl(temp);
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
			LogEndl(temp);
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
			LogEndl(temp);
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
	void write(std::ofstream&& file, string&& message)
	{
		if (file.is_open())
		{
			file << message << endl;
			file.close();
		}
	}
	void Log::LogEndl(LogStream& obj)
	{
		cout << obj.m_cache << endl;
		auto future = std::async(std::launch::async, write, std::move(obj.m_logFile), std::move(obj.m_cache));
		obj.m_cache.clear();
		obj.m_logFile.close();
	}

	Log::LogStream& Log::LogStream::openToFile(const string& name)
	{
		if (name != "")
		{
			auto curPath = fs::current_path();
			curPath.append(name);
			if(m_logFile.is_open())
				m_logFile.close();
			m_logFile.open(curPath, ios::app);
		}
		return *this;
	}

	string Log::LogStream::getLabel(LogType type) const
	{
		if(type == LogType::Error)
			return "Error";
		else if (type == LogType::Debug)
			return "Debug";
		else if (type == LogType::Info)
			return "Message";
		else if (type == LogType::Warning)
			return "Warning";
		else
			return "";
	}

	Log::LogStream::LogStream(LogType type)
		:TYPE(type), m_cache(format("{} - [{}]:", GetTime(), getLabel(type)))
	{}
	Log::LogStream::LogStream(const string& message)
		:TYPE(LogType::None), m_cache(message)
	{}
	Log::LogStream& Log::LogStream::operator()(ClassInfor& error)
	{
		m_cache += format(" in {} that", error.CompleteInfor());
		return *this;
	}
	Log::LogStream& Log::LogStream::append(const string& message)
	{
		m_cache += " ";
		m_cache += message;
		return *this;
	}

	Log::LogStream& Log::LogStream::append(string&& message)
	{
		return append(message);
	}
	Log::LogStream& Log::LogStream::Get()
	{
		return *this;
	}
	Log::LogStream& Log::LogStream::change(const char spliter)
	{
		size_t pos = 0;
		while ((pos=m_cache.find(m_spliter))!=m_cache.npos)
		{
			m_cache.replace(pos,1,1,spliter);
		}
		m_spliter = spliter;
		return *this;
	}

	Log::LogStream& Log::LogStream::remove(const string& message)
	{
		auto subStringPos = m_cache.find(" "+message);
		if (!ClassInfor::SubStrNotFound(subStringPos))
		{
			m_cache.erase(subStringPos, message.length() + 1);
		}
		return *this;
	}

	Log::LogStream& Log::LogStream::remove(string&& message)
	{
		return remove(message);
	}
	Log::LogStream& Log::LogStream::operator<<(const std::string& message)
	{
		return append(message);
	}
	Log::LogStream& Log::LogStream::operator()(const char* classInfor)
	{
		ClassInfor error(classInfor);
		m_cache += format("in {}that", error.CompleteInfor());
		return *this;
	}
	void Log::LogStream::operator<<(void (*p)(LogStream&))
	{
		p(*this);
	}
	Log::LogStream::~LogStream()
	{
		m_logFile.close();
	}
}