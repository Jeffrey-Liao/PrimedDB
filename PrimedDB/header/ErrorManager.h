#pragma once
#include <future>
#include <queue>
#include <stack>

#include "TimeStamp.h"
#include "Singleton.h"

namespace liao::Util
{
    enum class ErrorLevel:char
    {
        Info,
        Warning,
        Error,
        Fatal,
    };
    struct Error
    {
        ErrorLevel m_level;
        std::string m_name;
        std::string m_message;
        TimeStamp m_errorTime;
        Error(ErrorLevel, std::string& , std::string&);
        Error(ErrorLevel, std::string_view, std::string_view);
        bool operator==(Error& error);
    };
    class ErrorManager final:public Singleton<ErrorManager>
    {
        friend class Singleton;
        using ErrorHandler = std::function<void(Error&)>;
        std::vector<bool> m_levelInProcess;
        std::queue<Error> m_errors;
        std::unordered_map<std::string, bool> m_inProcess;
        std::condition_variable m_conditionVar;
        std::unordered_map <std::string, std::vector<ErrorHandler>> m_serviceByName;
        std::unordered_map <ErrorLevel, std::vector<ErrorHandler>> m_serviceByLevel;
        std::atomic<bool> end = false;
        mutable ShareMutex m_mutex;
        mutable std::mutex m_cvmutex;
        std::atomic<bool> m_reported = false;
        std::future<void> m_publishFuture;
        void publish();
        static void send(std::vector<ErrorHandler>& services, Error& error);
        static void send(ErrorHandler& service, Error& error);
        bool contains(const std::string& error) const;
    protected:
        ErrorManager();
    public:
        void set(Error& error);
        void set(ErrorLevel level, std::string& error,std::string& errorMessage);
        void set(ErrorLevel level, std::string_view error, std::string_view errorMessage);
        void subscribe(ErrorLevel level,const ErrorHandler&);
        void subscribe(std::string_view error, const ErrorHandler&);
        ~ErrorManager();
    };
}
