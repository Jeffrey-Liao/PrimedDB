#include "ClassInfor.h"
#include <mutex>
#include <memory>
namespace liao::message
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
		Error,
		Debug,
		Message
	};
	class Log
	{
		class LogStream;
	public:
		static void logEndl(LogStream& obj);
	private:
		class LogStream
		{
			std::string cache;
			const LogType TYPE;
			std::ofstream logFile;
			static std::mutex LogStreamMutex;
		private:
			string getLabel(LogType) const;
		public:
			LogStream(LogType type);

			LogStream(string& message);

			LogType getType() const;

			LogStream& openToFile(string&& name);

			LogStream& openToFile(string& name);

			LogStream& operator()(ClassInfor& error);

			LogStream& operator()(const char*);

			LogStream& operator<<(const std::string&&);

			LogStream& operator<<(const std::string&);

			friend void Log::logEndl(LogStream& obj);

			void operator<<(void(*p)(LogStream&));

			~LogStream();
		};

		static std::mutex LogMutex;

		static Log Logger;

	private:
		Log();

		~Log();

		Log(const Log& obj) = delete;

		Log(const Log&& obj) = delete;

	public:
		static Log& Get();

		static void print(const string& message);
		static void print(string&& message);
		static void printLine(const string& message);
		static void printLine(string&& message);

		////////////////////////////////////////////////////////////////

		void printError(std::string&& message, string&& fileName);
		void printDebug(std::string&& message, string&& fileName);
		void printMessage(std::string&& message,string&& fileName);

		////////////////////////////////////////////////////////////////

		void printError(const std::string& message,string& fileName);
		void printDebug(const std::string& message,string& fileName);
		void printMessage(const std::string& message,string& fileName);

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
		LogStream operator[](string&& notation);
	};
}