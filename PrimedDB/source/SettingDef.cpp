
#include "Setting.h"
USESTD;
namespace liao::Util
{
	namespace fs = std::filesystem;
	Setting::Setting()
	{
		fs::path programPath = fs::current_path();
		fs::path compilerPath = programPath/m_compilerDirectory;
		fs::path dataPath = programPath/m_dataDirectory;
        fs::path globalPath = programPath/m_globalDirectory;
		fs::path logPath = programPath/m_logDirectory;

		if (!fs::exists(compilerPath))
			fs::create_directory(compilerPath);
		if (!fs::exists(dataPath))
            fs::create_directory(dataPath);
        if (!fs::exists(globalPath))
            fs::create_directory(globalPath);
        if (!fs::exists(logPath))
            fs::create_directory(logPath);
	}
	unsigned int Setting::getBlockNumber()const
	{
        return m_blockNumber;
	}
	unsigned int Setting::getBlockSize()const
	{
        return m_blockSize * m_kbConvert;
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
	unsigned int Setting::getNameSize() const
	{
		return m_nameSize;
	}
	unsigned Setting::getEnlargePower() const
	{
		return m_enlargePower;
	}
	Math::HashType Setting::getUserIDHashType() const
	{
		return m_userIDHashType;
	}

	unsigned int Setting::setBlockSize(unsigned int blockSize)
	{
		return m_blockSize = blockSize;
	}
	unsigned int Setting::setBlockNumber(unsigned int blockNumber)
	{
		return m_blockNumber = blockNumber;
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
		return m_dataDirectory.data();
	}
	std::string_view Setting::getCompilerDirectory()const
	{
		return m_compilerDirectory.data();
	}
	std::string_view Setting::getGlobalDirectory()const
	{
		return m_globalDirectory.data();
	}
	std::string_view Setting::getLogDirectory()const
	{
		return m_logDirectory.data();
	}
}