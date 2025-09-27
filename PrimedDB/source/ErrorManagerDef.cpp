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
    static void MessageLog(Error& error)
    {
        Infor::Log::Get()[Infor::LogType::Message].openToFile(std::string("message.log")) << error.m_name << " " << error.m_message << " " << error.m_errorTime.getString() << Infor::Log::LogEndl;
    }
    Error::Error(ErrorLevel level, std::string& name, std::string& message)
        :m_level(level),m_name(std::move(name)),m_message(std::move(message)),m_errorTime(TimeStamp::Now()), m_isHandled(false)
    {
    }
    ErrorManager::ErrorManager()
    {
        subscribe(ErrorLevel::Info, MessageLog);
        subscribe(ErrorLevel::Error, ErrorLog);
        subscribe(ErrorLevel::Fatal, ErrorLog);
        subscribe(ErrorLevel::Warning, WarningLog);
    }
    void ErrorManager::send(std::vector<ErrorHandler>& services, Error& error)
    {
        for (auto& service : services)
            service(error);
        error.m_isHandled = true;
    }
    void ErrorManager::send(ErrorHandler& service, Error& error)
    {
        service(error);
        error.m_isHandled = true;
    }
    void ErrorManager::publish(Error& error)
    {
        const bool isLevel = m_serviceByLevel.contains(error.m_level),
        isName = m_serviceByName.contains(error.m_name);
        if ((isLevel||isName)&&!error.m_isHandled)
        {
            if (isLevel)
                send(m_serviceByLevel[error.m_level], error);
            else
                send(m_serviceByName[error.m_name],error);
            m_errors.erase(std::find(m_errors.begin(), m_errors.end(), error));
        }
    }
    void ErrorManager::set(ErrorLevel level, std::string& error, std::string& errorMessage)
    {
        shared_ptr<Error> ptr = make_shared<Error>(level, error, errorMessage);
        m_errors.emplace_back(ptr);
        m_errorIndexByLevel[ptr->m_level].emplace_back(ptr);
        m_errorIndexByName[ptr->m_name] = ptr;
        publish(*ptr);
    }
    void ErrorManager::set(Error& error)
    {
        shared_ptr<Error> ptr = shared_ptr<Error>(new Error(error.m_level, error.m_name, error.m_message));
        m_errors.emplace_back(ptr);
        m_errorIndexByLevel[ptr->m_level].emplace_back(ptr);
        m_errorIndexByName[ptr->m_name] = ptr;
        publish(*ptr);
    }
    bool ErrorManager::exist(ErrorLevel level, std::string_view error) const
    {
        const string errorName = error.data();
        return m_errorIndexByLevel.contains(level)&&m_errorIndexByName.contains(errorName)&& !m_errorIndexByName.at(errorName)->m_isHandled;
    }
    bool ErrorManager::exist(std::string_view error) const
    {
        const string errorName = error.data();
        return m_errorIndexByName.contains(errorName) && !m_errorIndexByName.at(errorName)->m_isHandled;
    }
    void ErrorManager::subscribe(ErrorLevel level, const ErrorHandler& handler)
    {
        m_serviceByLevel[level].emplace_back(handler);
    }
    void ErrorManager::subscribe(std::string_view error, const ErrorHandler& handler)
    {
        m_serviceByName[string(error.data())].emplace_back(handler);
    }
   
}
