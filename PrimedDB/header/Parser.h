#pragma once
#include <string>
#include <vector>
#include "Global.h"

namespace liao::Compiler
{
	union Token
	{
		std::string m_text;
		char m_operator;
		int m_integer;
		double m_double;
		bool m_boolean;
	};
	class Parser
	{
		std::vector<std::string> m_tokens;
		bool m_valid;
	public:
		Parser()=default;
		Parser(const std::string& command);
		void parse(const std::string& command);

		std::vector<std::string>& get();
		bool valid() const;
		~Parser() = default;
	};


}
