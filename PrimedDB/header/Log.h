#include "ClassInfor.h"
#include "Singleton.h"
#include "Defs.h"

namespace liao::Infor
{
	namespace fs = std::filesystem;
	constexpr const char* LOG_FOLDER = "logFiles";
	constexpr const char* ERROR_LOG_FILE = "errorLog.log";
	constexpr const char* DEBUG_LOG_FILE = "debugLog.log";
	//constexpr const char* FATAL_LOG_FILE = "fatalLog.log";
	static string GetTime();
	enum class LogType
	{
		None,
		Warning,
		Fatal,
		Error,
		Debug,
		Info
	};
	class Log:public Singleton<Log>
	{
		INVITESINGLETON;
		class LogStream;
		friend class LogStream;
		static ShareMutex m_staticMutex;
		Mutex m_cvMutex;

		static std::condition_variable m_cv;
		std::atomic<bool> m_end=false;
		std::future<void> m_asyncTerminate;
		static std::queue<LogStream> m_messages;
	public:
		static void LogEndl(LogStream& obj);
	private:
		static string LogFilePath;
		class LogStream
		{
			std::string m_cache;
			const LogType TYPE;
			string m_split = "\t";
			string m_logFile;
			static std::mutex LogStreamMutex;
		private:
			static string getLabel(LogType);
		public:
			LogStream(LogType type);

			LogStream(LogStream&& mObject) noexcept;

			LogStream(const string& message);

			LogStream& append(const string& message);

			LogStream& append(string&& message);

			LogStream& split(const char split);

			LogStream& remove(const string& message);

			LogStream& remove(string&& message);

			std::string& getFile();

			std::string& getMessage();

			LogStream& openToFile(const string& name);

			LogStream& operator()(ClassInfor& error);

			LogStream& operator()(const char*);

			LogStream& operator<<(const std::string&);

			string& getString();

			friend void Log::LogEndl(LogStream& obj);

			void operator<<(void(*p)(LogStream&));

			LogStream& Get();

			~LogStream();
		};

	private:
		Log();
		void asyncFileHandler();
	public:
		static void Print(const string& message);
		static void Print(string&& message);
		static void PrintLine(const string& message);
		static void PrintLine(string&& message);

		////////////////////////////////////////////////////////////////

		void printError(std::string&& message, string&& fileName);
		void printDebug(std::string&& message, string&& fileName);
		void printMessage(std::string&& message,string&& fileName);

		////////////////////////////////////////////////////////////////

		void printError(const std::string& message,const string& fileName);
		void printDebug(const std::string& message, const string& fileName);
		void printMessage(const std::string& message, const string& fileName);

		////////////////////////////////////////////////////////////////
		
		void printError(ClassInfor& error,const std::string& message, string& fileName);
		void printDebug(ClassInfor& error,const std::string& message, string& fileName);
		void printMessage(ClassInfor& error, const std::string& message, string& fileName);

		////////////////////////////////////////////////////////////////

		void printError(ClassInfor& error, std::string&& message, string&& fileName);
		void printDebug(ClassInfor& error, std::string&& message, string&& fileName);
		void printMessage(ClassInfor& error, std::string&& message, string&& fileName);


		////////////////////////////////////////////////////////////////
		
		LogStream operator[](LogType type);
		~Log();
	};
}