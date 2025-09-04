#pragma once
#include "Global.h"
namespace liao::PrimedDB
{
    class ErrorManager :public Singleton<ErrorManager>
    {
        std::unordered_map <std::string, std::string>m_error_errorMessage;
    public:
        std::string get(const std::string& error);
        void set(std::string& error,std::string& errorMessage);
        bool exist(const std::string& error) const;

        bool operator()(const std::string& error)const;
    };
}
