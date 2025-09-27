#pragma once
#include <string>
#include <vector>
#include <filesystem>
namespace liao::Compiler
{
	namespace fs = std::filesystem;
	class SyntaxChecker
	{
		constexpr std::string tokenList = "/compiler/token.tk";
		bool isPathExist();

	public:
		std::vector<std::string> check(std::string string);

	};
}
