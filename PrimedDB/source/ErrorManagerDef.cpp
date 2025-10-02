#include "ErrorManager.h"
#include "Log.h"
#include "Setting.h"
USESTD;
namespace liao::Util
{
    static void ErrorLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Error].openToFile(std::string("error.log"))<<"Report an error:" << error.m_name << error.m_message << error.m_errorTime.getString() << Infor::Log::LogEndl;
    }
    static void FatalLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Fatal].openToFile(std::string("fatal.log")) << "Report an error:" << error.m_name << error.m_message << error.m_errorTime.getString() << Infor::Log::LogEndl;
    }
    static void WarningLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Warning].openToFile(std::string("warning.log")) << "Report an error:" << error.m_name << " " << error.m_message << " " << error.m_errorTime.getString() << Infor::Log::LogEndl;
    }
    static void InfoLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Info].openToFile(std::string("Info.log")) << "Report an error:" << error.m_name << " " << error.m_message << " " << error.m_errorTime.getString() << Infor::Log::LogEndl;
    }
    Error::Error(ErrorLevel level, std::string& name, std::string& message)
        :m_level(level),m_name(std::move(name)),m_message(std::move(message)),m_errorTime(TimeStamp::Now())
    {
    }
    Error::Error(ErrorLevel, std::string_view name, std::string_view message)
        :m_level(ErrorLevel::Info), m_name(name), m_message(message), m_errorTime(TimeStamp::Now())
    {
    }
    Error::Error(Error&& move)
        :m_name(std::move(move.m_name)), m_level(move.m_level), m_message(std::move(move.m_message)), m_errorTime(std::move(move.m_errorTime))
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
        return *this;
    }
    void ErrorManager::publish()
    {
        unique_lock<mutex> cvLock(m_cvmutex);
        vector<ErrorHandler> levelHandlers, nameHandlers;
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
                    ReadLock lock(m_mutex);
                    levelHandlers = m_serviceByLevel[error.m_level];
                    nameHandlers = m_serviceByName[error.m_name];
                }
                send(levelHandlers, error);
                send(nameHandlers, error);
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
