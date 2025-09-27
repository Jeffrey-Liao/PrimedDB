This is a **developer document** of *Primed Database*.

So that this document will tell the reader about *how did those interface in Prime DB written* elaborately. 

There will be another folder call *design* which also under this documentation folder as well. 
It will record all mind change of mine and idea of some interface but in Chinese mainly.

---
# Rules
1. Both code and documentation will use camel naming rule for ordinary variables and functions.
```cpp title::Cppcode
int varName;
void functionName();
```

2. The constant variable will be in capital letter form.
```cpp title::Cppcode
int CONSTVARNAME
```

3. The variables of map or key, value pair will be named with key usage + under line + value usage.
```cpp title::Cppcode
unordered_map<int,string> id_nameMap;
pair<int,string> id_namePair;
```

4. The name of static variable and function will begin with capital letter.
 ```cpp title::Cppcode
 static int VariableName;
 static void FunctionName();
 ```

5. In document, the variable will be written in *italic* way. The class name will be around by *{ClassName}*. The function name will be around by *\[functionName\]*. The specific file name will be written in **Bold** form.

| identifier | documentation representation |
| ---------- | ---------------------------- |
| class      | *{class name}*               |
| function   | *[function name]*            |
| variable   | *variable name*              |
