#pragma once
#include "Setting.h"
#include <string>
#include <unordered_map>

namespace liao::Compiler
{
	class TokenList:Singleton<TokenList>
	{
		INVITESINGLETON;
		std::unordered_map <std::string, std::vector<std::string>> m_type_tokenList;

	public:
		TokenList();

		bool exist(const std::string& type, const std::string& token)const;

		const std::string& getToken(const std::string& type, unsigned index)const;
	};
}
