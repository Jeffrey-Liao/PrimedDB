# Usage
Class information is class which can automatically collect error information of this line. It will use macro provided by C++ compiler to get the class name, function call name and line number at the line which construct *Class information*.

# Class declaration
```cpp
class MY_API ClassInfor
{
private:
	string raw2FunctionName(string& functionSig);
	
	string raw2ClassName(string& functionSig);
	
	string raw2ParameterList(string& functionSig);
	
	string raw2ReturnType(string& functionSig);
	
	string combineToComplete();
public:
	ClassInfor(std::string&& functionsig, string&& fileDirectory = "", long long lineNumber = -1);
	
	const std::string CompleteInfor;
	
	const std::string FunctionName;
	
	const std::string ClassName;
	
	const std::string ParameterList;
	
	const std::string ReturnType;
	
	const long long LineNumber;
	
	const std::string FileDirectory;
	
	operator string();
	
	constexpr static bool  SubStrNotFound(size_t pos);
};
```
