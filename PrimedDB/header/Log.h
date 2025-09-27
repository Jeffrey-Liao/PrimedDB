#include "ClassInfor.h"
#include <mutex>
#include <memory>
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
		Error,
		Debug,
		Message
	};
	class Log
	{
		class LogStream;
	public:
		static void LogEndl(LogStream& obj);
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

			LogStream(const string& message);

			LogStream& append(const string& message);

			LogStream& append(string&& message);

			LogStream& remove(const string& message);

			LogStream& remove(string&& message);

			LogType getType() const;

			LogStream& openToFile(string&& name);

			LogStream& openToFile(const string& name);

			LogStream& openToFile(std::string_view name);

			LogStream& operator()(ClassInfor& error);

			LogStream& operator()(const char*);

			LogStream& operator<<(std::string&&);

			LogStream& operator<<(const std::string&);

			friend void Log::LogEndl(LogStream& obj);

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

		static void Print(const string& message);
		static void Print(string&& message);
		static void PrintLine(const string& message);
		static void PrintLine(string&& message);

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
	};
}