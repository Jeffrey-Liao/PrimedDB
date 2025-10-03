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

namespace liao::Infor
{
	using std::string;
	using std::to_string;
	
	class ClassInfor
	{
		string complete;
		//Dereference function name information from compiler Macro
		string raw2FunctionName(const string& functionSig);

		//Dereference class name information from compiler Macro
		string raw2ClassName(const string& functionSig);

		//Dereference parameter list information from compiler Macro
		string raw2ParameterList(const string& functionSig);

		//Get function return type from compiler macro
		string raw2ReturnType(const string& functionSig);

		//return compelete class, function information for the line constructed this instance
		string combineToComplete() const;
	public:
		/*(	compiler macro which carry function and class information, 
			compiler macro which indicatte file directory information *OPTIONAL*,
			compiler macro which indicate the line which construct current instance *OPTIONAL* )*/
		ClassInfor()=default;
		ClassInfor(const ClassInfor&) = default;
		ClassInfor(const std::string& functionsig, const std::string& fileDirectory = "", long long lineNumber = -1);
		ClassInfor(ClassInfor&&)noexcept ;
		void init(const std::string& functionsig, const std::string& fileDirectory = "", long long lineNumber = -1);
		ClassInfor& operator=(ClassInfor&);
		//The complete information at the line which construct current instance
		const std::string& CompleteInfor();

		//The name of function call at the line which construct current instance
		std::string FunctionName;

		//The class information of current method call at the line which construct current instance
		std::string ClassName;

		//The parameter list of function call at the line which construct current instance
		std::string ParameterList;

		//The return type of function call at the line which construct current instance
		std::string ReturnType;

		//The line number of the line which construct current instance
		long long LineNumber;

		//The file path of current source file.
		std::string FileDirectory;

		operator string();

		//Check the string pos is -infinity or not
		static bool SubStrNotFound(size_t pos);
	};
}

#endif
