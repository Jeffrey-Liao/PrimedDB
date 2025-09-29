#include "ErrorManager.h"
#include "Log.h"
#include "Setting.h"
USESTD;
namespace liao::Util
{
    static void ErrorLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Error].openToFile(std::string("error.log")) << error.m_name << " " << error.m_message << " " << error.m_errorTime.getString() << Infor::Log::LogEndl;
    }
    static void WarningLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Warning].openToFile(std::string("warning.log")) << error.m_name << " " << error.m_message << " " << error.m_errorTime.getString() << Infor::Log::LogEndl;
    }
    static void InfoLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Info].openToFile(std::string("Info.log")) << error.m_name << " " << error.m_message << " " << error.m_errorTime.getString() << Infor::Log::LogEndl;
    }
    Error::Error(ErrorLevel level, std::string& name, std::string& message)
        :m_level(level),m_name(std::move(name)),m_message(std::move(message)),m_errorTime(TimeStamp::Now())
    {
    }
    Error::Error(ErrorLevel, std::string_view name, std::string_view message)
        :m_level(ErrorLevel::Info), m_name(name), m_message(message), m_errorTime(TimeStamp::Now())
    {
    }
    bool Error::operator==(Error& error)
    {
        return error.m_level == m_level&&error.m_name== m_name;
    }
    void ErrorManager::publish()
    {
        unique_lock<mutex> lock(m_cvmutex);
        while (1)
        {
            m_conditionVar.wait(lock, [&]()
                {
                    return m_reported||end;
                });
            while (!m_errors.empty())
            {
                {
                    ReadLock lock(m_mutex);
                    Error& error = m_errors.front();
                    send(m_serviceByLevel[error.m_level], error);
                    send(m_serviceByName[error.m_name], error);
                    
                }
                {
                    WriteLock lock(m_mutex);
                    Error& error = m_errors.front();
                    m_inProcess[error.m_name] = false;
                    m_errors.pop();
                }
            }
            m_reported = false;
            if (end&&m_errors.empty())
                break;
        }
    }
    ErrorManager::ErrorManager()
        :m_levelInProcess(4)
    {
        m_publishFuture = std::async(std::launch::async, [this]() 
            { this->publish(); });
        subscribe(ErrorLevel::Info, InfoLog);
        subscribe(ErrorLevel::Error, ErrorLog);
        subscribe(ErrorLevel::Fatal, ErrorLog);
        subscribe(ErrorLevel::Warning, WarningLog);
        m_levelInProcess.resize(4);
    }
    void ErrorManager::send(std::vector<ErrorHandler>& services, Error& error)
    {
         vector<std::future<void>> handlerThreads;
         for (auto& service : services)
            handlerThreads.emplace_back(std::async(std::launch::async, [&service,&error]()
                {
                    service(error);
                }));
        for (auto& handlerThread : handlerThreads)
            handlerThread.get();
    }
    void ErrorManager::send(ErrorHandler& service, Error& error)
    {
        service(error);
    }

   
    void ErrorManager::set(ErrorLevel level, std::string& error, std::string& errorMessage)
    {
        Error errorObject(level, error, errorMessage);
        set(errorObject);
    }
    void ErrorManager::set(ErrorLevel level, std::string_view error, std::string_view errorMessage)
    {
        Error errorObject(level, error, errorMessage);
        set(errorObject);
    }
   
    void ErrorManager::set(Error& error)
    {
        if (contains(error.m_name))
            return;
        {
            WriteLock lock(m_mutex);
            m_errors.push(error);
            m_inProcess[error.m_name] = true;
            m_reported = true;
        }
        m_conditionVar.notify_one();
    }
    bool ErrorManager::contains(const std::string& error)const
    {
        ReadLock lock(m_mutex);
        return m_inProcess.contains(error) && m_inProcess.at(error);
    }
    void ErrorManager::subscribe(ErrorLevel level, const ErrorHandler& handler)
    {
        WriteLock lock(m_mutex);
        m_serviceByLevel[level].emplace_back(handler);
    }
    void ErrorManager::subscribe(std::string_view error, const ErrorHandler& handler)
    {
        WriteLock lock(m_mutex);
        m_serviceByName[string(error.data())].emplace_back(handler);
    }
    ErrorManager::~ErrorManager()
    {
        end = true;
        m_publishFuture.get();
    }
   
}
