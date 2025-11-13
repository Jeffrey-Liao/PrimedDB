#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <regex>
#include <future>
#include <sstream>
#include <stack>
#include <algorithm>
#include <cctype>
#include <optional>
#include "Singleton.h"
#include "User.h"
#include "Transaction.h"
#include "TableManager.h"
#include "Table.h"
#include "Column.h"

namespace liao::Compiler
{
	enum class TokenType
	{
		EXPRESSION,
		LEFTPAREN,
		RIGHTPAREN,
		KEYWORD,
		IDENTIFIER,
		IDENTIFIERS,
		IDENTIFIER_TABLE,
		IDENTIFIER_COLUMN,
		LOGICAL_OPERATOR,
		COLUMNS,
		NOT,
		DATATYPE,
		STRING_LITERAL,
		VALUE,
		NUMBER,
		ANY,
		AND,
		OR,
		OPERATOR,
		ASSIGNS,
		DELIMITER,
		MULTIPLE,
		REPEAT,
		END,
		UNKNOWN,
		SIZE
	};
	struct Token
	{
		TokenType type;
		std::string value;
		size_t position;
		
		Token(TokenType t, std::string& v, size_t pos = 0);
	};

	struct Commnad
	{

		PrimedDB::DataType m_type;
		std::string m_value;
	};

	class Result
	{
	public:
		Result() = default;
		Result(bool);
		Result(bool, std::string&, size_t);
		Result(bool success, std::string&& message, size_t pos);
		bool m_success = false;
		std::string m_message;
		size_t m_errorPosition = 0;
	};
	struct Entity
	{
		PrimedDB::DataType m_type;
        std::string content;
	public :
		Entity() = default;
		Entity(PrimedDB::DataType, std::string&);
		Entity(const Entity& copy);
		void operator=(Entity&);
		Entity(Entity&&) noexcept;

		void operator=(Entity&&)noexcept;
	};
	class Compiler : public Singleton<Compiler>
	{
		INVITESINGLETON
	private:
		class ExecuteBody
		{
			PrimedDB::UserPtr m_user;
			PrimedDB::SQLType m_task;
			std::unordered_map<unsigned, std::vector<Entity>> m_operations;
			std::string m_message;
			bool m_valid = false;
			static PrimedDB::SQLType getTask(const std::string&);
			
			void selectConstructor(std::deque<Token>&);
			void insertConstructor(std::deque<Token>&);
			void deleteConstructor(std::deque<Token>&);
			void updateConstructor(std::deque<Token>&);
			void createConstructor(std::deque<Token>&);
			void analyzer(std::deque<Token>&);

		public:
            ExecuteBody()=default;
			ExecuteBody(const std::string&);
			ExecuteBody(std::deque<Token>&);
			void construct(std::deque<Token>&);
			bool isValid()const;
			void setValid(bool);
            const std::string& getMessage()const;
			void setMessage(const std::string&);
			std::unordered_map<unsigned, std::vector<Entity>>& get();
			PrimedDB::SQLType getTask();

		};
		// Token类型检查
		static TokenType getTokenType(std::string& token);
		static bool isKeyword(std::string& token);
		static bool isDataType(std::string& token);
		static bool isIdentifier(std::string& token);
		static bool isNumber(std::string& token);
		static bool isStringLiteral(std::string& token);
		
		// 词法分析 - 生成带类型的tokens
		static int syntaxChecker(const std::vector<TokenType>&, std::deque<Token>& tokens, unsigned& start);
		static std::deque<Token> parseSQL(const std::string& sql, std::string& error);

		//Special token type checker


		//check rule like:
		// (column name - byte size,
		// column name - byte size,
		// .....)
		static bool columnsCheck(std::deque<Token>& tokens, unsigned& start);
		//check expressions like:
		// identifier = value
        // identifier = 'literal'
		static bool expressionCheck(std::deque<Token>& tokens,unsigned& start);
		// check identifier identifier identifier ... 
		static bool identifiersCheck(std::deque<Token>& tokens,unsigned& start);
		// check list like:
		//(value, value,...)
		static bool valueCheck( std::deque<Token>& tokens, unsigned& start);
		//check repetition like value, value,(the last token can be ignored)
		static int repeatCheck(const std::vector<TokenType>& types, unsigned& typeStart,std::deque<Token>& tokens, unsigned& tokenIter);
		//convert token type to string
		static std::string type2string(TokenType);

		static ExecuteBody ruleMatch(const std::vector<TokenType>&,std::deque<Token>&);
		static ExecuteBody syntaxFailed(std::string&, const std::string&);
		static ExecuteBody syntaxCheck(PrimedDB::UserPtr,std::deque<Token>& tokens,std::string&);


		static PrimedDB::Record execute(PrimedDB::UserPtr, ExecuteBody&, std::string&);


		static void toLower(std::string& str);
		static std::string trim(const std::string& str);

		static std::unordered_map<std::string, std::vector<std::vector<TokenType>>> m_rules;
		void constructRules();
		Compiler();
	public:

		// compile function
		std::future<Result> compile(PrimedDB::UserPtr user, const std::string& sql);

	};
}
