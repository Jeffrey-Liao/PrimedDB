#include "ErrorManager.h"
USESTD;
namespace liao::PrimedDB
{
    std::string ErrorManager::get(const std::string& error)
    {
        string errorMessage;
        {
            WriteLock lock(m_mutex);
            errorMessage = std::move(m_error_errorMessage[error]);
            m_error_errorMessage.erase(error);
        }
        return errorMessage;
    }
    void ErrorManager::set(std::string& error, std::string& errorMessage)
    {
        WriteLock lock(m_mutex);
        m_error_errorMessage.emplace(std::move(error), std::move(errorMessage));
    }
    bool ErrorManager::exist(const std::string& error) const
    {
        ReadLock lock(m_mutex);
        return m_error_errorMessage.contains(error);
    }
    bool ErrorManager::operator()(const std::string& error)const
    {
        return exist(error);
    }
}
