#pragma once
#include <future>

#include "Global.h"
namespace liao::Util
{
    enum class ErrorLevel:char
    {
        Info = 'i',
        Warning='w',
        Error='e',
        Fatal='f',
    };
    struct Error
    {
        ErrorLevel m_level;
        std::string m_name;
        std::string m_message;
        Util::TimeStamp m_errorTime;
        std::atomic<bool> m_isHandled;
        Error(ErrorLevel, std::string& , std::string&);
    };
    class ErrorManager :public Singleton<ErrorManager>
    {
        using ErrorHandler = std::function<void(Error&)>;
        std::vector<std::shared_ptr<Error>> m_errors;
        std::unordered_map <std::string, std::shared_ptr<Error>> m_errorIndexByName;
        std::unordered_map <ErrorLevel, std::vector<std::shared_ptr<Error>>> m_errorIndexByLevel;
        std::unordered_map <std::string, std::vector<ErrorHandler>> m_serviceByName;
        std::unordered_map <ErrorLevel, std::vector<ErrorHandler>> m_serviceByLevel;
        mutable ShareMutex m_mutex;
        ErrorManager();
        void publish(Error& error);
        void send(std::vector<ErrorHandler>& services, Error& error);
        void send(ErrorHandler& service, Error& error);
    public:
        void set(Error& error);
        void set(ErrorLevel level, std::string& error,std::string& errorMessage);
        bool exist(ErrorLevel level, std::string_view error) const;
        bool exist(std::string_view error) const;
        void subscribe(ErrorLevel level,const ErrorHandler&);
        void subscribe(std::string_view error, const ErrorHandler&);
    };
}
