#include <ClassInfor.h>
#include <format>
ACTIVE_STD;
namespace liao::Infor
{
	string ClassInfor::raw2FunctionName(string& functionSig)
	{
		//find the first string after '::'(restricted by class) and before '(' (parameter list)
		auto pos4DoubleColon = functionSig.find_last_of("::"),
			pos4Space = functionSig.find_last_of(" "),
			pos4LBracket = functionSig.find_first_of("(");
		//if this function is no a member of some class
		if (SubStrNotFound(pos4DoubleColon))
			return functionSig.substr(pos4Space + 1
				, (pos4LBracket) - (pos4Space + 1));
		else
			return functionSig.substr(pos4DoubleColon + 1
				, (pos4LBracket) - (pos4DoubleColon + 1));
	}
	string ClassInfor::raw2ClassName(string& functionSig)
	{
		auto pos4Space = functionSig.find_last_of(" "),
			pos4DoubleColon = functionSig.find_last_of("::");
		//if name of class was not found.
		if (SubStrNotFound(pos4Space)
			|| SubStrNotFound(pos4DoubleColon))
			return "";
		else
		{
			string tempStr = functionSig.substr(pos4Space + 1,pos4DoubleColon - pos4Space - 2);
			pos4DoubleColon = tempStr.find_last_of("::");
			if (!SubStrNotFound(pos4DoubleColon))
				tempStr = tempStr.substr(pos4DoubleColon);
			return tempStr;
		}
			
	}
	string ClassInfor::raw2ParameterList(string& functionSig)
	{
		auto pos4LBracket = functionSig.find_first_of("("),
			pos4RBracket = functionSig.find_last_of(")");
		//if no () pair was found, then return a empty string back to constructor
		if (SubStrNotFound(pos4LBracket) || SubStrNotFound(pos4RBracket))
			return "";
		else
			return functionSig.substr(pos4LBracket+1
				, pos4RBracket - pos4LBracket-1);
	}
	string ClassInfor::raw2ReturnType(string& functionSig) 
	{
		auto pos4Space = functionSig.find_first_of(" ");
		if (SubStrNotFound(pos4Space))
			return "";
		else
			return functionSig.substr(0
				, pos4Space);
	}
	string ClassInfor::combineToComplete()
	{
		string cache;
		if (!ClassName.empty())
			cache += "{" + ClassName + "} ";
		if (!FunctionName.empty())
			cache += "[" + FunctionName + "] ";
		if(!ParameterList.empty())
			cache += "(" + ParameterList + ") ";
		if(LineNumber != -1)
			cache +="at line " + to_string(LineNumber)+" ";
		if(!FileDirectory.empty())
			cache += "in " + FileDirectory+" ";
		return cache;
		//{className} [functionName] (parameterList) at line * in D://
	}
	ClassInfor::ClassInfor(std::string&& functionSig, string&& fileDirectory, long long lineNumber)
		:ReturnType(raw2ReturnType(functionSig)),

		ParameterList(raw2ParameterList(functionSig)),

		FunctionName(raw2FunctionName(functionSig)),

		ClassName(raw2ClassName(functionSig)),
		LineNumber(lineNumber),
		FileDirectory(fileDirectory),
		complete("")
	{}
	const string ClassInfor::CompleteInfor()
	{
		if(complete.empty())
			complete = combineToComplete();
		return complete;
	}
	//ClassInfor::operator string()
	//{
	//	return this->CompleteInfor();
	//}
	bool ClassInfor::SubStrNotFound(size_t pos)
	{
		return pos == string::npos;
	}
}