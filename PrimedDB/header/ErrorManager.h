#pragma once
#include <future>
#include <deque>
#include <stack>

#include "TimeStamp.h"
#include "ClassInfor.h"
#include "Singleton.h"

namespace liao::Util
{
    struct Error;

    enum class ErrorLevel:char
    {
        Info,
        Warning,
        Error,
        Fatal,
    };
    class ErrorHandlers
    {
    public:
        static void FileNotExistHandler(Error& error);
    };
    struct Error
    {
        ErrorLevel m_level;
        std::string m_name;
        std::string m_message;
        TimeStamp m_errorTime;
        Infor::ClassInfor m_info;
        Error() = default;
        Error(const Error&) = default;
        Error(Error&&) noexcept;
        Error(ErrorLevel, std::string& , std::string&, const Infor::ClassInfor&);
        Error(ErrorLevel, std::string_view, std::string_view,const Infor::ClassInfor&);
        bool operator==(Error& error);
        Error& operator=(Error&& error);
    };
    class ErrorManager final:public Singleton<ErrorManager>
    {
        friend class Singleton;
        using ErrorHandler = std::function<void(Error&)>;
        std::deque<Error> m_errors;
        std::condition_variable m_conditionVar;
        std::unordered_map <std::string, std::vector<ErrorHandler>> m_serviceByName;
        std::unordered_map <ErrorLevel, std::vector<ErrorHandler>> m_serviceByLevel;
        std::atomic<bool> end = false;
        mutable ShareMutex m_mutex;
        mutable ShareMutex m_handleMutex;
        mutable std::mutex m_cvmutex;
        std::atomic<bool> m_reported = false;
        std::future<void> m_asyncTerminate;
        void publish();
        static void send(std::vector<ErrorHandler>& services, Error& error);
        static void send(ErrorHandler& service, Error& error);
    protected:
        ErrorManager();
    public:
        void set(Error& error);
        void set(ErrorLevel level, std::string& error,std::string& m_message,const Infor::ClassInfor& info);
        void set(ErrorLevel level, std::string_view error, std::string_view m_message,const Infor::ClassInfor& info);
        void set(ErrorLevel level, std::string_view error, std::string_view m_message);
        template<class F, class... Args>
        void subscribe(ErrorLevel level, F&& func, Args&&... args)
        {
            WriteLock lock(m_handleMutex);
            m_serviceByLevel[level].emplace_back(
                [func = std::forward<F>(func), args_tuple = std::make_tuple(std::forward<Args>(args)...)](Error& error) mutable
                {
                    if constexpr (sizeof...(args) == 0) {
                        std::invoke(func, error);
                    }
                    else {
                        apply_custom(func, args_tuple, error);
                    }
                });
        }
        template<class F, class... Args>
        void subscribe(std::string_view error, F&& func, Args&&... args)
        {
            WriteLock lock(m_handleMutex);
            m_serviceByName[error.data()].emplace_back(
                [func = std::forward<F>(func), args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable
                {
                    std::apply(func, args_tuple);
                });
        }
        ~ErrorManager();
    };
}
