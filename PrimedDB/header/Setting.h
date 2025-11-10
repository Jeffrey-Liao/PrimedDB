#pragma once
#include "ErrorManager.h"
#include "HashContainer.h"
#include "json/json.h"

namespace liao::Math
{
	enum class HashType;
}

namespace liao::Util
{
	constexpr const char* SETTING_FILE_NAME = "setting.ini";
	class Setting:public Singleton<Setting>
	{
		INVITESINGLETON;
		using path = std::filesystem::path;
		const unsigned int m_enlargeSize = 512;//the enlarge number for Primize Algorithm
		unsigned int m_blockNumber = 256;//the number of blocks
		unsigned int m_blockSize = 16;//the size of a block
		const unsigned m_kbConvert = 1024;
		unsigned int m_sessionNumber = 10;//the number of sessions
		unsigned int m_port = 1010;
		const unsigned int m_nameSize = 50;
		const unsigned int m_enlargePower = 9;
		const Math::HashType m_userIDHashType = Math::HashType::MD5;

		path m_configurationFile = "conf.cfg";

		path m_dataDirectory = "data";
		path m_compilerDirectory = "compiler";
		path m_globalDirectory = "global";
		path m_logDirectory = "log";
		path m_tableFile = "table.def";
		path m_userFile = "users.def";
		path m_tokenFile = "tokens.file";




		mutable ShareMutex m_mutex;
		//check existence of all directories
		//check existence of critical files
		//if one of them is missing, terminate the system.
		Setting();
	public:
		std::string m_systemPassword;
		unsigned int getBlockNumber()const;
		unsigned int getBlockSize()const;
		unsigned int getEnlargeSize()const;
		unsigned int getSessionNumber()const;
		unsigned int getPort()const;
		unsigned int getNameSize() const;
		unsigned getEnlargePower() const;
		void save(const std::string& key, const std::string& value);
		Math::HashType getUserIDHashType() const;
		unsigned int setBlockSize(unsigned int blockSize);
		unsigned int setBlockNumber(unsigned int blockNumber);
		//unsigned int setEnlargeSize(unsigned int enlargeSize);
		unsigned int setSessionNumber(unsigned int sessionNumber);
		unsigned int setPort(unsigned int port);

		const std::string& getSystemPassword() const;

		const path& getDataDirectory() const;
		const path& getCompilerDirectory() const;
		const path& getGlobalDirectory() const;
		const path& getLogDirectory() const;
		const path& getUserFile()const;
		const path& getTableFile()const;
	};
}
