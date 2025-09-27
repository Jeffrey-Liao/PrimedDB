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

	static void LogEndl(LogStream& obj);

	static void Print(const string& message);
	static void Print(string&& message);
	static void PrintLine(const string& message);
	static void PrintLine(string&& message);

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
};
```

```cpp title:LogStream
class LogStream
	{
		std::string cache;
		const LogType TYPE;
		std::ofstream logFile;
	private:
		string getLabel(LogType) const;
	public:
		LogStream(LogType type);
		LogStream(string& message);

		LogStream& append(string& message);

		LogStream& append(string&& message);

		LogStream& remove(string& message);

		LogStream& remove(string&& message);

	//////////////////////////////////////////

		LogType getType() const;
		
	//////////////////////////////////////////
		
		LogStream& openToFile(string&& name);
		LogStream& openToFile(string& name);

	//////////////////////////////////////////

		LogStream& operator()(ClassInfor& error);
		LogStream& operator()(const char*);

	//////////////////////////////////////////

		LogStream& operator<<(std::string&&);
		LogStream& operator<<(const std::string&);

	//////////////////////////////////////////

		friend void Log::LogEndl(LogStream& obj);
		void operator<<(void(*p)(LogStream&));
		~LogStream();
	};
```
---
# Specification
## Log - Public Methods

### `static Log& Get()`
The get function for singleton instance which return the static member logger.

### `static void LogEndl(LogStream& obj)`
This function can end up a log message operation. It will send message to console and file (if user specify a valid file path).

### `static void Print(const string& message)/Print(string&& message)`
This method will print given message to console. It is equivalent to `cout`.

### `static void PrintLine(const string& message)/PrintLine(string&& message)`
This method will print given message and current time.

### `void printError(const std::string& message,string& fileName)/printError(std::string&& message, string&& fileName)`
Print log message with ==error mode== and write into the file specify by user. This method will call \[*[[#`LogStream& openToFile(string& name)/openToFile(string&& name)`|openToFile]]*\] and \[*[[#`static void LogEndl(LogStream& obj)`|LogEndl]]*\]

### `void printDebug(const std::string& message,string& fileName)/printDebug(std::string&& message, string&& fileName)`
Print log message with ==debug mode== and write into the file specify by user. This method will call \[*[[#`LogStream& openToFile(string& name)/openToFile(string&& name)`|openToFile]]*\] and \[*[[#`static void LogEndl(LogStream& obj)`|LogEndl]]*\]

### `void printMessage(const std::string& message,string& fileName)/printMessage(std::string&& message,string&& fileName)`
Print log message with ==message mode== and write into the file specify by user. This method will call \[*[[#`LogStream& openToFile(string& name)/openToFile(string&& name)`|openToFile]]*\] and \[*[[#`static void LogEndl(LogStream& obj)`|LogEndl]]*\]

### `void printError(ClassInfor& error,const std::string& message, string& fileName)/printError(ClassInfor& error, std::string&& message, string&& fileName)`
Print log message and class information at error place with ==error mode== and write into the file specify by user. This method will call \[*[[#`LogStream& openToFile(string& name)/openToFile(string&& name)`|openToFile]]*\] and \[*[[#`static void LogEndl(LogStream& obj)`|LogEndl]]*\]

### `void printDebug(ClassInfor& error,const std::string& message, string& fileName)/printDebug(ClassInfor& error, std::string&& message, string&& fileName)`
Print log message and class information at error place with ==debug mode== and write into the file specify by user. This method will call \[*[[#`LogStream& openToFile(string& name)/openToFile(string&& name)`|openToFile]]*\] and \[*[[#`static void LogEndl(LogStream& obj)`|LogEndl]]*\]

### `void printMessage(ClassInfor& error, const std::string& message, string& fileName)/printMessage(ClassInfor& error, std::string&& message, string&& fileName)`
Print log message and class information at error place with ==message mode== and write into the file specify by user. This method will call \[*[[#`LogStream& openToFile(string& name)/openToFile(string&& name)`|openToFile]]*\] and \[*[[#`static void LogEndl(LogStream& obj)`|LogEndl]]*\]

### `LogStream operator[](LogType type)`
Generate a LogStream instance with type tag, which can provide message write to file and write console feature thread safe.

## Log - Private Methods
None

## LogStream - Public Methods
### `LogStream& openToFile(string& name)/openToFile(string&& name)`
This method will try to open or create a file under `current program path/logFiles`. This function will do nothing if given file name is invalid. This function only open or create a file without write anything into it.
*This function will automatically close the file if that file still opening*.
The write procedure will not be done until *\[[[#`static void LogEndl(LogStream& obj)`|LogEndl]]\]* was called.

### `LogStream(LogType type)`
Construct {*LogStream*} with type. So that the mode of message can be correctly detected when new message append by operator or function.

### `LogStream& append(string& message)/append(string&& message)``
Append new message after current message body.

### `LogStream& remove(string& message)/remove(string&& message)``
Remove a message part if given message exists in message body.

### `LogStream(string& message)`
Construct {*LogStream*} by message body already built.

### `LogType getType() const`
Return log type bind with current {*LogStream*} instance.

### `LogStream& operator()(ClassInfor& error)/operator()(const char*)`
Bind a error class information with current {*LogStream*} instance.

### `LogStream& operator<<(const std::string&)/operator<<(const std::string&&)`
=\[*[[#`LogStream& append(string& message)/append(string&& message)``|append]]*\]

### `void operator<<(void(*p)(LogStream&))`
A open interface for \[*[[#`static void LogEndl(LogStream& obj)`|LogEndl]]*\] which can finish current {*LogStream*}

### `~LogStream()`
This de-constructor will automatically close the file handler.

## LogStream - Private Methods
### `string getLabel(LogType) const;`
A helper function return the string label corresponding to type tag which will be write into message body. 

# Members

| **Log**        |               |           |                                                                                                                                           |
| -------------- | ------------- | --------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| **visibility** | **type**      | **name**  |                                                                                                                                           |
| private<br>    | static mutex  | lockMutex | This is a resource lock for all access to static resource in {*Log*}.                                                                     |
| private        | static Log    | logger    | This is the static instance for singleton class {*Log*}                                                                                   |
|                |               |           |                                                                                                                                           |
| **LogStream**  |               |           |                                                                                                                                           |
| **visibility** | **type**      | **name**  |                                                                                                                                           |
| private        | string        | cache     | This is a cache for message which is not printed yet.                                                                                     |
| private        | const LogType | TYPE      | This store type message type of current log message. The types of message are: Debug, Error, Message                                      |
| private        | ofstream      | logFile   | This is the file specified by instance user, which can be specify by method \[*[[#`LogStream& openToFile(string&& name)`\|openToFile]]*\] |

---
# Usage
