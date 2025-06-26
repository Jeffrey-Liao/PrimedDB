#ifndef L_CLASSINFOR_H
#define L_CLASSINFOR_H
#include<string>
#include<filesystem>
#include<fstream>
#define ACTIVE_STD using namespace std
#define THISLINE __LINE__
#define THISFILE __FILE__
#ifdef _WIN32
#define THISFUNC __FUNCSIG__
#else 
#define THISFUNC __PRETTY_FUNCTION__
#endif
#define MY_API __declspec(dllexport)

namespace liao::message
{
	using std::string;
	using std::to_string;
	
	class ClassInfor
	{
		string complete;
		//Dereference function name information from compiler Macro
		string raw2FunctionName(string& functionSig);

		//Dereference class name information from compiler Macro
		string raw2ClassName(string& functionSig);

		//Dereference parameter list information from compiler Macro
		string raw2ParameterList(string& functionSig);

		//Get function return type from compiler macro
		string raw2ReturnType(string& functionSig);

		//return compelete class, function information for the line constructed this instance
		string combineToComplete();
	public:
		/*(	compiler macro which carry function and class information, 
			compiler macro which indicatte file directory information *OPTIONAL*,
			compiler macro which indicate the line which construct current instance *OPTIONAL* )*/
		ClassInfor(std::string&& functionsig, string&& fileDirectory = "", long long lineNumber = -1);
		
		//The complete information at the line which construct current instance
		const std::string CompleteInfor();

		//The name of function call at the line which construct current instance
		const std::string FunctionName;

		//The class information of current method call at the line which construct current instance
		const std::string ClassName;

		//The parameter list of function call at the line which construct current instance
		const std::string ParameterList;

		//The return type of function call at the line which construct current instance
		const std::string ReturnType;

		//The line number of the line which construct current instance
		const long long LineNumber;

		//The file path of current source file.
		const std::string FileDirectory;


		//Check the string pos is -infinity or not
		constexpr static bool  SubStrNotFound(size_t pos);
	};
}

#endif
