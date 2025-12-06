#include "ErrorManager.h"
#include "Log.h"
#include "Setting.h"
USESTD;
namespace liao::Util
{
    static TimeStamp NOW = TimeStamp::Now();
    static void ErrorLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Error].openToFile("error"+ NOW.getDate()).split('-') << error.m_name << error.m_message << error.m_info << Infor::Log::LogEndl;
    }
    static void FatalLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Fatal].openToFile(std::string("fatal"+ NOW.getDate())).split('-') << error.m_name << error.m_message << error.m_info << Infor::Log::LogEndl;
    }
    static void WarningLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Warning].openToFile(std::string("warning"+ NOW.getDate())).split('-') << error.m_name << error.m_message << Infor::Log::LogEndl;
    }
    static void InfoLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Info].split('-') << error.m_name  << error.m_message << Infor::Log::LogEndl;
    }
    Error::Error(ErrorLevel level, std::string& name, std::string& message,const Infor::ClassInfor& info)
        :m_level(level),m_name(std::move(name)),m_message(std::move(message)),m_errorTime(TimeStamp::Now()),m_info(info)
    {
    }
    Error::Error(ErrorLevel, std::string_view name, std::string_view message,const Infor::ClassInfor& info)
        :m_level(ErrorLevel::Info), m_name(name), m_message(message), m_errorTime(TimeStamp::Now()),m_info(info)
    {
    }
    Error::Error(Error&& move) noexcept
        :m_name(std::move(move.m_name)), m_level(move.m_level), m_message(std::move(move.m_message)), m_errorTime(std::move(move.m_errorTime)),m_info(move.m_info)
    {}
    bool Error::operator==(Error& error)
    {
        return error.m_level == m_level&&error.m_name== m_name;
    }
    Error& Error::operator=(Error&& error)
    {
        m_errorTime = std::move(error.m_errorTime);
        m_level = error.m_level;
        m_message = std::move(error.m_message);
        m_name = std::move(error.m_name);
        m_info = error.m_info;
        return *this;
    }
    void ErrorManager::publish()
    {
        unique_lock<mutex> cvLock(m_cvmutex);
        while (1)
        {
            m_conditionVar.wait(cvLock, [&]()
                {
                    return m_reported || end;
                });
            if (end && m_errors.empty())
                break;
            
            while (1)
            {
                Error error;
                {
                    WriteLock lock(m_mutex);
                    if (m_errors.empty())
                        break;
                    error = std::move(m_errors.front());
                    m_errors.pop_front();
                }
                {
                    ReadLock lock(m_handleMutex);
                    send(m_serviceByLevel[error.m_level], error);
                    send(m_serviceByName[error.m_name], error);
                }
                
                //pair<ErrorLevel, string> cmpPair = make_pair(level,name);
            }
            m_reported = false;
        }
    }
    ErrorManager::ErrorManager()
    {
        m_asyncTerminate = std::async(std::launch::async, [this]() 
            { this->publish(); });
        subscribe(ErrorLevel::Info, InfoLog);
        subscribe(ErrorLevel::Error, ErrorLog);
        subscribe(ErrorLevel::Fatal, FatalLog);
        subscribe(ErrorLevel::Warning, WarningLog);
    }
    void ErrorManager::send(std::vector<ErrorHandler>& services, Error& error)
    {
        for (ErrorHandler& service : services)
            send(service,error);
    }
    void ErrorManager::send(ErrorHandler& service, Error& error)
    {
        service(error);
    }

   
    void ErrorManager::set(ErrorLevel level, std::string& error, std::string& m_message, const Infor::ClassInfor& info)
    {
        Error errorObject(level, error, m_message, info);
        set(errorObject);
    }
    void ErrorManager::set(ErrorLevel level, std::string_view error, std::string_view m_message,const Infor::ClassInfor& infor)
    {
        Error errorObject(level, error, m_message,infor);
        set(errorObject);
    }
    void ErrorManager::set(ErrorLevel level, std::string_view error, std::string_view m_message)
    {
        Error errorObject(level, error, m_message, Infor::ClassInfor());
        WriteLock lock(m_mutex);
        m_errors.emplace_back(errorObject);
        m_reported = true;
        m_conditionVar.notify_one();
    }
   
    void ErrorManager::set(Error& error)
    {
        //if (!contains(error.m_name))
        {
            WriteLock lock(m_mutex);
            m_errors.emplace_back(error);
            m_reported = true;
        }
        m_conditionVar.notify_one();
    }
    ErrorManager::~ErrorManager()
    {
        end = true;
        m_conditionVar.notify_one();
        m_asyncTerminate.get();
    }
   
}
