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
		const unsigned int m_enlargeSize = 512;//the enlarge number for Primize Algorithm
		unsigned int m_blockNumber = 256;//the number of blocks
		unsigned int m_blockSize = 4;//the size of a block
		const unsigned m_kbConvert = 1024;
		unsigned int m_sessionNumber = 10;//the number of sessions
		unsigned int m_port = 1010;
		const unsigned int m_nameSize = 50;
		const unsigned int m_enlargePower = 9;
		const Math::HashType m_userIDHashType = Math::HashType::MD5;

		std::string m_dataDirectory = "data";
		std::string m_compilerDirectory = "compiler";
		std::string m_globalDirectory = "global";
		std::string m_logDirectory = "log";



		mutable ShareMutex m_mutex;
	private:
		//check existence of all directories
		//check existence of critical files
		//if one of them is missing, terminate the system.
		Setting();
	public:
		unsigned int getBlockNumber()const;
		unsigned int getBlockSize()const;
		unsigned int getEnlargeSize()const;
		unsigned int getSessionNumber()const;
		unsigned int getPort()const;
		unsigned int getNameSize() const;
		unsigned getEnlargePower() const;

		Math::HashType getUserIDHashType() const;
		unsigned int setBlockSize(unsigned int blockSize);
		unsigned int setBlockNumber(unsigned int blockNumber);
		//unsigned int setEnlargeSize(unsigned int enlargeSize);
		unsigned int setSessionNumber(unsigned int sessionNumber);
		unsigned int setPort(unsigned int port);

		std::string_view getDataDirectory()const;
		std::string_view getCompilerDirectory()const;
		std::string_view getGlobalDirectory()const;
		std::string_view getLogDirectory()const;
	};
}
