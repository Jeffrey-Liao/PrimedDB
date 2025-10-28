#pragma once
#include <unordered_map>
#include "Singleton.h"
#include "User.h"

namespace liao::Compiler
{
	union Token
	{
		std::string str;
        int num;
        char ch;
        bool boolean;
		double decimal;
		Token() = default;
	};
	class Result
	{
	public:
		Result() = default;
	};
	class Compiler: public Singleton<Compiler>
	{
		INVITESINGLETON;
		std::vector<std::string>& parse(std::string&);
		std::unordered_map<std::string,Token> syntaxCheck(std::vector<std::string>& tokens);
		bool semanticCheck(std::unordered_map<std::string, Token>& tokens);
	public:
		std::future<Result> compile(PrimedDB::UserPtr,std::string&);

	};
}
