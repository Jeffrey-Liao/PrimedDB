#pragma once
#include "ErrorManager.h"
#include "json/json.h"

namespace liao::Util
{
	constexpr const char* SETTING_FILE_NAME = "setting.ini";
	class Setting:public Singleton<Setting>
	{
		unsigned int m_enlargeSize;//the enlarge number for Primize Algorithm
		unsigned int m_blockNumber;//the number of blocks
		unsigned int m_blockSize;//the size of a block
		unsigned int m_sessionNumber;//the number of sessions
		unsigned int m_port;

		UCharPtr m_dataDirectory;
		UCharPtr m_compilerDirectory;
		UCharPtr m_globalDirectory;
		UCharPtr m_logDirectory;


		mutable ShareMutex m_mutex;
	private:
		//check existence of all directories
		//check existence of critical files
		//if one of them is missing, terminate the system.
		void initialize();
		void read();
		void write();
		Setting();
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
