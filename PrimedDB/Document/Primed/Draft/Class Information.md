# Introduction
***Class information*** is a class which can automatically collect error information of this line. It will use macro provided by C++ compiler to get the class name, function call name and line number at the line which construct *{ClassInfor}*.

---
# Class declaration

```cpp title:ClassInfor.h
class ClassInfor
{
private:
	string raw2FunctionName(string& functionSig);
	
	string raw2ClassName(string& functionSig);
	
	string raw2ParameterList(string& functionSig);
	
	string raw2ReturnType(string& functionSig);
	
	string combineToComplete();
public:
	ClassInfor(std::string&& functionsig, string&& fileDirectory = "", long long lineNumber = -1);
	
	const std::string FunctionName;
	
	const std::string ClassName;
	
	const std::string ParameterList;
	
	const std::string ReturnType;
	
	const long long LineNumber;
	
	const std::string FileDirectory;
	
	//const std::string CompleteInfor();
	
	operator string();
	
	constexpr static bool  SubStrNotFound(size_t pos);
};
```

^6c5c98

---
# Specification

## Public Methods
### ``ClassInfor(std::string&& functionsig, string&& fileDirectory = "", long long lineNumber = -1)``

The *functionSig* should receive compile time macro provided by C++ compiler such as ``__FUNCSIG__`` (under Windows environment) or ``__PRETTY_FUNCTION__`` (under Linux environment). The developer can use `THISFUNC` macro defined in **ClassInfor.h** as parameter to construct {*ClassInfor*} instance.

The *fileDirectory* should receive compile time macro `__FILE__` which contains file directory information of current source/ header file. The developer can use `THISFILE`  macro defined in **ClassInfor.h** as parameter to construct {*ClassInfor*} instance.

The *lineNumber* should receive compile time macro `__LINE__` which contains the line number of the line that contains this macro. The developer can use `THISLINE` macro defined in **ClassInfor.h** as parameter to construct {*ClassInfor*} instance.

### ``const std::string CompleteInfor()``

The function return complete class information of the error place including *[[#^6c5c98|ClassName]]*, *[[#^6c5c98|FunctionName]]*, *[[#^6c5c98|ParameterList]]*, [[#^6c5c98|ReturnType]], *[[#^6c5c98|LineNumber]]*, *[[#^6c5c98|FileDirectory]]* in format 

 "{*ClassName*} \[*FunctionName*\] (*ParameterList*) at line *LineNumber* in *FileDirectory*"

### `operator string()`

Equivalent to \[*[[Class Information#``const std string CompleteInfor()``|CompleteInfor]]*\]. It can provide a quicker way to compare and operate current instance.

### `static bool  SubStrNotFound(size_t pos)`

It is a static helper function which check given position in string is equal to string::npos or not.

---
## Private Methods
### `string raw2FunctionName(string& functionSig)`
This is a helper function which can extract ==function name== from raw compiler defined macro *functionSig*. It will be called in the constructor of {*ClassInfor*}.

### `string raw2ClassName(string& functionSig)`
This is a helper function which can extract ==class name== from raw compiler defined macro *functionSig*. It will be called in the constructor of {*ClassInfor*}.

### `string raw2ParameterList(string& functionSig)`
This is a helper function which can extract ==parameter list== from raw compiler defined macro *functionSig*. It will be called in the constructor of {*ClassInfor*}.

### `string raw2ReturnType(string& functionSig)`
This is a helper function which can extract ==Return Type== from raw compiler defined macro *functionSig*. It will be called in the constructor of {*ClassInfor*}.

### `string combineToComplete()`
The is a helper function which combined all variable into  "{*ClassName*} \[*FunctionName*\] (*ParameterList*) at line *LineNumber* in *FileDirectory*" format.

---
# Members

| **visibility** | **type**        | **name**      |                                                                                                |
| -------------- | --------------- | ------------- | ---------------------------------------------------------------------------------------------- |
| public         | const string    | FunctionName  | It contains the ==function name== of the place which construct current ClassInfor instance     |
| public         | const string    | ClassName     | It contains the ==class name== of the place which construct current ClassInfor instance        |
| public         | const string    | ParameterList | It contains the ==parameter list== of the function which construct current ClassInfor instance |
| public         | const string    | ReturnType    | It contains the ==return type== of the function which construct current ClassInfor instance    |
| public         | const string    | FileDirectory | It contains the ==file name== of the file which construct current ClassInfor instance          |
| public         | const long long | LineNumber    | It contains the ==line number== of the line which construct current ClassInfor instance        |

---
# Usage

### Construct
```cpp title:ClassConstruct
void test()
{
	//Error happen here
	ClassInfor instance(THISFUNC/*MADATORY*/,
	THISFILE/*OPTIONAL*/,
	THISLINE/*OPTIONAL*/);
}

```
### Use information inside of class
```cpp  title:InstanceUseage
void test()
{
	cout << instance.ClassName << endl;
	cout << instance.FunctionName << endl;
	cout << instance.ReturnType << endl;
	cout << instance.ParameterList << endl;
	cout << instance.FileDirectory << endl;
	cout << instance.LineNumber << endl;

	cout << instance.CompleteInfor() << endl;
	if(instance.empty())
	{
		cout << "Getting error information failed" << endl;
	}
	string errorFunctionInformation = instance;
}
```