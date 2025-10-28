#include <execution>

#include "Parser.h"

namespace liao::Compiler
{
	Parser::Parser(const std::string& command)
	{
		parse(command);
	}
	void Parser::parse(const std::string& command)
	{
		StaticFunc::Split(m_tokens, command, ' ');
		for (auto& token:m_tokens)
		{
			
		}
	}

	std::vector<std::string>& Parser::get()
	{
		return m_tokens;
	}
	bool Parser::valid() const
	{
		return m_valid;
	}
}
