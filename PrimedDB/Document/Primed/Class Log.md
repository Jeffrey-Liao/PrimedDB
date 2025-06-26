# Introduction
**Log class** is a class which provide a **thread save** way to write log information to the console and file. It is a **singleton** class which construct the static instance at the beginning of the program. This class provide not only variety of methods but also easy to use operators to construct a log message.

{*LogStream*} is the key of sending thread save log message. It will create separate instance for each thread so that all file writing operation and console writing operation will not be affected by other threads.

---
# Class declaration
```cpp title:Log
class Log
{
	class LogStream;
private:
	static std::mutex lockMutex;
	static Log logger;
	//std::ofstream fatalFile;
	Log();
	~Log();
	Log(const Log& obj) = delete;
	Log(const Log&& obj) = delete;
public:
	static Log& Get();

	static void logEndl(LogStream& obj);

	static void print(const string& message);
	static void print(string&& message);
	static void printLine(const string& message);
	static void printLine(string&& message);

	void printError(std::string&& message, string&& fileName);
	void printDebug(std::string&& message, string&& fileName);
	void printMessage(std::string&& message,string&& fileName);

////////////////////////////////////////////////////////////

	void printError(const std::string& message,string& fileName);
	void printDebug(const std::string& message,string& fileName);
	void printMessage(const std::string& message,string& fileName);

////////////////////////////////////////////////////////////

	void printError(ClassInfor& error,const std::string& message, string& fileName);
	void printDebug(ClassInfor& error,const std::string& message, string& fileName);
	void printMessage(ClassInfor& error, const std::string& message, string& fileName);

////////////////////////////////////////////////////////////

	void printError(ClassInfor& error, std::string&& message, string&& fileName);
	void printDebug(ClassInfor& error, std::string&& message, string&& fileName);
	void printMessage(ClassInfor& error, std::string&& message, string&& fileName);
	
	LogStream operator[](LogType type);
	LogStream operator[](string&& notation);
};
```

```cpp title:LogStream
class LogStream
	{
		std::string cache;
		const LogType type;
		std::ofstream logFile;
	private:
		string getLabel(LogType) const;
	public:
		LogStream(LogType type);
		LogStream(string& message);

	//////////////////////////////////////////

		LogType getType() const;
		
	//////////////////////////////////////////
		
		LogStream& openToFile(string&& name);
		LogStream& openToFile(string& name);

	//////////////////////////////////////////

		LogStream& operator()(ClassInfor& error);
		LogStream& operator()(const char*);

	//////////////////////////////////////////

		LogStream& operator<<(const std::string&&);
		LogStream& operator<<(const std::string&);

	//////////////////////////////////////////

		friend void Log::logEndl(LogStream& obj);
		void operator<<(void(*p)(LogStream&));
		~LogStream();
	};
```
---
# Specification
## Log - Public Methods

### `static Log& Get()`
The get function for singleton instance which return the static member logger;


## Log - Private Methods


## LogStream - Public Methods



## LogStream - Private Methods




# Members

---
# Usage
