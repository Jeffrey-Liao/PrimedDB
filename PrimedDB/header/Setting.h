#pragma once
#include "ErrorManager.h"
#include "json/json.h"

namespace liao::Util
{
	constexpr const char* SETTING_FILE_NAME = "setting.ini";
	class Setting:public Singleton<Setting>
	{
		INVITESINGLETON;
		unsigned int m_enlargeSize;//the enlarge number for Primize Algorithm
		unsigned int m_blockNumber;//the number of blocks
		unsigned int m_blockSize;//the size of a block
		unsigned int m_sessionNumber;//the number of sessions
		unsigned int m_port;

		std::string m_dataDirectory;
		std::string m_compilerDirectory;
		std::string m_globalDirectory;
		std::string m_logDirectory = "/log/";


		mutable ShareMutex m_mutex;
	private:
		//check existence of all directories
		//check existence of critical files
		//if one of them is missing, terminate the system.
		Setting();
		void read();
		void write();
	public:
		unsigned int getBlockNumber()const;
		unsigned int getBlockSize()const;
		unsigned int getEnlargeSize()const;
		unsigned int getSessionNumber()const;
		unsigned int getPort()const;

		unsigned int setBlockSize(unsigned int blockSize);
		unsigned int setBlockNumber(unsigned int blockNumber);
		unsigned int setEnlargeSize(unsigned int enlargeSize);
		unsigned int setSessionNumber(unsigned int sessionNumber);
		unsigned int setPort(unsigned int port);

		std::string_view getDataDirectory()const;
		std::string_view getCompilerDirectory()const;
		std::string_view getGlobalDirectory()const;
		std::string_view getLogDirectory()const;
	};
}
