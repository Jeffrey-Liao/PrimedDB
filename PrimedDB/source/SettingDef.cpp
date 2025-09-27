
#include "Setting.h"
USESTD;
namespace liao::Util
{
	namespace fs = std::filesystem;
	void Setting::initialize()
	{
		fs::path programPath = fs::current_path();
		fs::path compilerPath = programPath;
        compilerPath.append(m_compilerDirectory.get());
		if (fs::exists(compilerPath)&&fs::exists("setting.cfg"))
		{
			
		}
		else
		{
			ErrorManager::getInstance().set(ErrorLevel::Fatal,)
		}
	}
	void Setting::read()
	{
		
	}
	void Setting::write()
	{
		
	}
	Setting::Setting()
	{
		initialize();
	}
	unsigned int Setting::getBlockNumber()const
	{
        return m_blockNumber;
	}
	unsigned int Setting::getBlockSize()const
	{
        return m_blockSize;
	}
	unsigned int Setting::getEnlargeSize()const
	{
        return m_enlargeSize;
	}
	unsigned int Setting::getSessionNumber()const
	{
        return m_sessionNumber;
		
	}
	unsigned int Setting::getPort()const
	{
		return m_port;
	}

	unsigned int Setting::setBlockSize(unsigned int blockSize)
	{
		return m_blockSize = blockSize;
	}
	unsigned int Setting::setBlockNumber(unsigned int blockNumber)
	{
		return m_blockNumber = blockNumber;
	}
	unsigned int Setting::setEnlargeSize(unsigned int enlargeSize)
	{
		return m_enlargeSize = enlargeSize;
	}
	unsigned int Setting::setSessionNumber(unsigned int sessionNumber)
	{
		return m_sessionNumber = sessionNumber;
	}
	unsigned int Setting::setPort(unsigned int port)
	{
		return this->m_port = port;
	}

	std::string_view Setting::getDataDirectory()const
	{
		return m_dataDirectory.get();
	}
	std::string_view Setting::getCompilerDirectory()const
	{
		return m_compilerDirectory.get();
	}
	std::string_view Setting::getGlobalDirectory()const
	{
		return m_globalDirectory.get();
	}
	std::string_view Setting::getLogDirectory()const
	{
		return m_logDirectory.get();
	}
}