#include <Log.h>
#include <chrono>
#include <iostream>
#include <ctime>
#include <sstream>
#include "Timestamp.h"

#include "Setting.h"
using namespace std;
using namespace liao::Util;
namespace liao::Infor
{
	mutex Log::LogStream::LogStreamMutex;
	static mutex timeMutex;
	condition_variable Log::m_cv;
	ShareMutex Log::m_staticMutex;
	queue<Log::LogStream> Log::m_messages;
	static string GetTime()
	{
		lock_guard<mutex> lock(timeMutex);
		auto now = TimeStamp::Now();
		return now.getString();
	}
	void Log::asyncFileHandler()
	{
		ofstream file;
		while (1)
		{
			Lock lock(m_cvMutex);
			m_cv.wait(lock, [this]()
				{
					return !m_messages.empty() || m_end;
				});
			bool run;
			{
				ReadLock lock(m_staticMutex);
				run = !m_messages.empty();
			}
			while (run)
			{
				file.close();
				{
					ReadLock lock(m_staticMutex);
					auto& object = m_messages.front();
					file.open(object.getFile(), ios::app);
					file << object.getMessage() << endl;
				}
				{
					WriteLock lock(m_staticMutex);
					m_messages.pop();
					run = !m_messages.empty();
				}
			}
			file.close();
			if (m_end)
				break;
		}
	}
	Log::Log()
	{
		m_asyncTerminate = std::async(std::launch::async, &Log::asyncFileHandler,this);
		if (!fs::exists(LogFilePath))
			fs::create_directories(LogFilePath);
		//fatalFile.open(LOG_FOLDER.append(FATAL_LOG_FILE), std::ios::app);
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

	void Log::printError(const std::string& message, const string& fileName)
	{
		string cache;
		{
			cache = format("{} - [{}]: {}", GetTime(), "Error", message);
			LogStream temp = LogStream(cache);
			temp.openToFile(fileName);
			LogEndl(temp);
		}

	}
	void Log::printDebug(const std::string& message, const string& fileName)
	{
		string cache;
		{
			cache = format("{} - [{}]: {}", GetTime(), "Debug", message);
			LogStream temp = LogStream(cache);
			temp.openToFile(fileName);
			LogEndl(temp);
		}

	}
	void Log::printMessage(const std::string& message, const string& fileName)
	{
		string cache;
		{
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
	void Log::LogEndl(LogStream& obj)
	{
		{
			WriteLock lock(m_staticMutex);
			cout << obj.m_cache << endl;
			m_messages.emplace(std::move(obj));
			m_cv.notify_one();
		}
	}
	Log::~Log()
	{
		m_end = true;
        m_cv.notify_one();
        m_asyncTerminate.get();
	}
	Log::LogStream& Log::LogStream::openToFile(const string& name)
	{
		if (name != "")
		{
			auto curPath = fs::current_path()/ LogFilePath/(name+".log");
            m_logFile = curPath.string();
		}
		return *this;
	}
	std::string& Log::LogStream::getFile()
	{
		return m_logFile;
	}

	std::string& Log::LogStream::getMessage()
	{
		return m_cache;
	}
	string Log::LogStream::getLabel(LogType type)
	{
		if(type == LogType::Error)
			return "Error";
		else if (type == LogType::Debug)
			return "Debug";
		else if (type == LogType::Info)
			return "Info";
		else if (type == LogType::Warning)
			return "Warning";
		else if (type == LogType::Fatal)
			return "Fatal";
		else
			return "";
	}
	string Log::LogFilePath = "log";
	Log::LogStream::LogStream(LogType type)
		:TYPE(type), m_cache(format("{} - [{}]:",GetTime(), getLabel(type)))
	{}
	Log::LogStream::LogStream(Log::LogStream && mObject) noexcept
		: TYPE(mObject.TYPE), m_cache(std::move(mObject.m_cache)),m_logFile(std::move(mObject.m_logFile)),m_split(mObject.m_split)
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
		if(!m_cache.empty()&&m_cache.back()!=':')
			m_cache += m_split;
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
	Log::LogStream& Log::LogStream::split(const char split)
	{
		size_t pos = 0;
		while ((pos=m_cache.find(m_split))!=m_cache.npos)
		{
			m_cache.replace(pos,1,1,split);
		}
		m_split = split;
		return *this;
	}
	string& Log::LogStream::getString()
	{
		return m_cache;
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
		Log::m_cv.notify_one();
	}
}