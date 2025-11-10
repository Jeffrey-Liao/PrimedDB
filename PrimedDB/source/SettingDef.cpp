
#include "Global.h"
#include "Setting.h"
USESTD;
namespace liao::Util
{
	namespace fs = std::filesystem;
	Setting::Setting()
	{
		fs::path programPath = fs::current_path();
		m_compilerDirectory = programPath/m_compilerDirectory;
		m_dataDirectory = programPath/m_dataDirectory;
		m_globalDirectory = programPath/m_globalDirectory;
		m_logDirectory = programPath/m_logDirectory;

		if (!fs::exists(m_compilerDirectory))
			fs::create_directory(m_compilerDirectory);
		if (!fs::exists(m_dataDirectory))
            fs::create_directory(m_dataDirectory);
        if (!fs::exists(m_globalDirectory))
            fs::create_directory(m_globalDirectory);
        if (!fs::exists(m_logDirectory))
            fs::create_directory(m_logDirectory);
		m_configurationFile = m_globalDirectory / m_configurationFile;
        m_userFile = m_globalDirectory / m_userFile;
        m_tableFile = m_globalDirectory /m_tableFile;
		m_tokenFile = m_compilerDirectory / m_tokenFile;
		fstream conf(m_systemPassword, ios::in);
		string buffer;
		if (!(conf.fail()&&conf.is_open()))
		{
			getline(conf, buffer);
			if (buffer.empty())
				m_systemPassword = StaticFunc::GetHashKey("system");
			else
			{
				vector<std::string> token;
				StaticFunc::Split(token, buffer, ':');
				m_systemPassword = std::move(token[1]);
			}

		}
		conf.close();
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
	void Setting::save(const std::string& key, const std::string& value)
	{
		fstream conf(m_configurationFile, ios::out | ios::app);
        conf << key << ":" << value << endl;
        conf.close();
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

	const std::string& Setting::getSystemPassword() const
	{
		return m_systemPassword;
	}

	const std::filesystem::path& Setting::getDataDirectory() const
	{
		return m_dataDirectory;
	}

	const std::filesystem::path& Setting::getCompilerDirectory() const
	{
		return m_compilerDirectory;
	}

	const std::filesystem::path& Setting::getGlobalDirectory() const
	{
		return m_globalDirectory;
	}

	const std::filesystem::path& Setting::getLogDirectory() const
	{
		return m_logDirectory;
	}

	const std::filesystem::path& Setting::getUserFile() const
	{
		return m_userFile;
	}

	const std::filesystem::path& Setting::getTableFile()const
	{
		return m_tableFile;
	}
}