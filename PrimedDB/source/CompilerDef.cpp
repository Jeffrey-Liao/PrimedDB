#include "Compiler.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <cctype>
#include <ranges>
#include <iostream>

#include "ErrorManager.h" 
#include "Log.h"

using namespace liao::PrimedDB;
using namespace std;
namespace liao::Compiler
{
	static void CallError(const string& error, const string& message, Infor::ClassInfor infor)
	{
		Util::ErrorManager::Get().set(
			Util::ErrorLevel::Error,
			error, message, infor
		);
	}
	static void CallInfo(const string& error, const string& message)
	{
		StaticFunc::WriteInfo(error, message);
	}
	static void CallWarning(const string& error, const string& message)
	{
		Util::ErrorManager::Get().set(
			Util::ErrorLevel::Info,
			error, message
		);
	}
#define ERROR_HEAD "[ERROR]:"
	Token::Token(TokenType t, std::string& v, size_t pos)
		: type(t), value(std::move(v)), position(pos)
	{}
	Result::Result(bool success)
		: m_success(success)
	{}
	Result::Result(bool success, std::string&& message, size_t pos)
		: m_success(success), m_message(std::move(message)), m_errorPosition(pos)
	{}
	Result::Result(bool success, std::string& message, size_t pos)
		:m_success(success), m_message(std::move(message)), m_errorPosition(pos)
	{}
	Entity::Entity(const Entity& copy)
		:content(copy.content), m_type(copy.m_type)
	{
		
	}
	void Entity::operator=(Entity& copy)
	{
		content = copy.content;
        m_type = copy.m_type;
	}
	Entity::Entity(DataType type, std::string& content)
		: m_type(type), content(std::move(content))
	{}
	Entity::Entity(Entity&& move)noexcept
		:m_type(move.m_type),content(std::move(move.content))
	{
		
	}
	void Entity::operator=(Entity&& move)noexcept
	{
		m_type = move.m_type;
        content = std::move(move.content);
	}
	Compiler::ExecuteBody::ExecuteBody(const std::string& message)
		: m_message(message), m_valid(false)
	{}
	SQLType Compiler::ExecuteBody::getTask(const std::string& token)
	{
		if (token == "select")
            return SQLType::Select;
        if (token == "insert")
            return SQLType::Insert;
        if (token == "update")
            return SQLType::Update;
        if (token == "create")
            return SQLType::Create;
        if (token == "delete")
            return SQLType::Delete;
        return SQLType::None;
	}
	void Compiler::ExecuteBody::selectConstructor(std::deque<Token>& tokens)
	{
		std::vector<Entity> listEntities;
		bool fromDetected = false;
		bool whereDetected = false;
		for (int tokenIter = 0; tokenIter <tokens.size();)
		{
			if (tokens[tokenIter].type == TokenType::SIZE)
			{
				int finish = tokens[tokenIter].value.size();
				++tokenIter;
				DataType type;
				for (int n = 0; n < finish; ++n, ++tokenIter)
				{
					if (fromDetected)
					{
						if (!whereDetected)
							listEntities.emplace_back(DataType::Table, tokens[tokenIter].value);
						else
						{
							if (tokens[tokenIter].type == TokenType::IDENTIFIER)
								type = DataType::Column;
							else if (tokens[tokenIter].type == TokenType::DELIMITER || tokens[tokenIter].type == TokenType::LEFTPAREN || tokens[tokenIter].type == TokenType::RIGHTPAREN)
								continue;
							else if (tokens[tokenIter].type == TokenType::NUMBER)
								type = DataType::Number;
							else if (tokens[tokenIter].type == TokenType::STRING_LITERAL)
								type = DataType::String;
							else if (tokens[tokenIter].type == TokenType::OPERATOR || tokens[tokenIter].type == TokenType::LOGICAL_OPERATOR || tokens[tokenIter].type == TokenType::NOT)
								type = DataType::Operator;
							listEntities.emplace_back(type, tokens[tokenIter].value);
						}
					}
					else
					{
						if (tokens[tokenIter].type == TokenType::IDENTIFIER)
							listEntities.emplace_back(DataType::Column, tokens[tokenIter].value);
					}
				}
				m_operations.emplace(m_operations.size(), std::move(listEntities));
				listEntities.clear();
				continue;
			}
			else
			{
				if (tokens[tokenIter].type == TokenType::END)
					break;
				if (tokens[tokenIter].type == TokenType::KEYWORD)
				{
					if (tokens[tokenIter].value == "from")
						fromDetected = true;
					if (tokens[tokenIter].value == "where")
						whereDetected = true;
				}
				else if (tokens[tokenIter].type == TokenType::MULTIPLE)
				{
					m_operations[m_operations.size()].emplace_back(DataType::All, tokens[tokenIter].value);
				}
				else if (tokens[tokenIter].type == TokenType::IDENTIFIER)
				{
					if (!fromDetected)
						m_operations[m_operations.size()].emplace_back(DataType::Column, tokens[tokenIter].value);
					else if (fromDetected)
						m_operations[m_operations.size()].emplace_back(DataType::Table, tokens[tokenIter].value);
				}
				++tokenIter;
			}
		}
	}
	void Compiler::ExecuteBody::insertConstructor(std::deque<Token>& tokens)
	{
		std::vector<Entity> listEntities;
		auto reuslt= std::ranges::find_if(tokens.begin(), tokens.end(), [](const Token& token)
			{
				return token.type == TokenType::IDENTIFIER;
			});
		m_operations[m_operations.size()].emplace_back(DataType::Table, reuslt->value);
		bool values= false;
		for (int tokenIter = 0; tokenIter <tokens.size();++tokenIter)
		{
			if (tokens[tokenIter].type == TokenType::KEYWORD)
			{
				values = tokens[tokenIter].value == "values";
			}
			if (tokens[tokenIter].type == TokenType::SIZE)
			{
				int finish = tokens[tokenIter].value.size();
				++tokenIter;
				for (int n = 0; n < finish;++n,++tokenIter)
				{
					if (values)
					{
						if (tokens[tokenIter].type == TokenType::NUMBER)
							listEntities.emplace_back(DataType::Number, tokens[tokenIter].value);
						if (tokens[tokenIter].type == TokenType::STRING_LITERAL)
                            listEntities.emplace_back(DataType::String, tokens[tokenIter].value);
					}
					else
					{
						if (tokens[tokenIter].type == TokenType::IDENTIFIER)
							listEntities.emplace_back(DataType::Column, tokens[tokenIter].value);
					}
				}
				tokenIter--;
				m_operations.emplace(m_operations.size(), std::move(listEntities));
				listEntities.clear();
			}
		}
	}
	//delete from table_name
	/*where condition;*/
	void Compiler::ExecuteBody::deleteConstructor(std::deque<Token>& tokens)
	{
		std::vector<Entity> listEntities;
		int type = 0;
		for (int tokenIter = 1; tokenIter < tokens.size(); tokenIter++)
		{
			if (tokens[tokenIter].type == TokenType::KEYWORD)
			{
				if (tokens[tokenIter].value == "user")
					type = 1;
				else if (tokens[tokenIter].value == "session")
                    type = 2;
			}
			else if (tokens[tokenIter].type == TokenType::IDENTIFIER)
			{
				if (type == 1)
					m_operations[m_operations.size()].emplace_back(DataType::User, tokens[tokenIter].value);
                else if (type == 2)
                    m_operations[m_operations.size()].emplace_back(DataType::Session, tokens[tokenIter].value);
				else
					m_operations[m_operations.size()].emplace_back(DataType::Table, tokens[tokenIter].value);
			}
			if (tokens[tokenIter].type == TokenType::SIZE)
			{
				int finish = tokens[tokenIter].value.size();
				++tokenIter;
				for (int n = 0; n < finish; ++n,++tokenIter)
				{
					if (tokens[tokenIter].type == TokenType::IDENTIFIER)
						listEntities.emplace_back(DataType::Column, tokens[tokenIter].value);
					else if (tokens[tokenIter].type == TokenType::OPERATOR)
						listEntities.emplace_back(DataType::Operator, tokens[tokenIter].value);
					else if (tokens[tokenIter].type == TokenType::NUMBER)
						listEntities.emplace_back(DataType::Number, tokens[tokenIter].value);
					else if (tokens[tokenIter].type == TokenType::STRING_LITERAL)
						listEntities.emplace_back(DataType::String, tokens[tokenIter].value);
				}
				tokenIter--;
				m_operations.emplace(m_operations.size(),std::move( listEntities));
				listEntities.clear();
			}
		}
	}
	//update table_name
	//	set column1 = value1 column2 = value2 ...
	void Compiler::ExecuteBody::updateConstructor(std::deque<Token>& tokens)
	{
		std::vector<Entity> listEntities;
		auto reuslt = std::ranges::find_if(tokens.begin(), tokens.end(), [](const Token& token)
			{
				return token.type == TokenType::IDENTIFIER;
			});
        m_operations[m_operations.size()].emplace_back(DataType::Table, reuslt->value);
		for (int tokenIter = 0; tokenIter <tokens.size(); ++tokenIter)
		{
			if (tokens[tokenIter].type == TokenType::KEYWORD)
			{
				if (tokens[tokenIter].value == "where")
					m_operations.emplace(m_operations.size(), listEntities);
				continue;
			}

			if (tokens[tokenIter].type == TokenType::SIZE)
			{
				int finish = tokens[tokenIter].value.size();
				++tokenIter;
                for (int n = 0; n < finish;++n,++tokenIter)
				{
					if (tokens[tokenIter].type == TokenType::NUMBER)
						listEntities.emplace_back(DataType::Number, tokens[tokenIter].value);
                    else if (tokens[tokenIter].type == TokenType::IDENTIFIER)
						listEntities.emplace_back(DataType::Column, tokens[tokenIter].value);
					else if (tokens[tokenIter].type == TokenType::OPERATOR)
                        listEntities.emplace_back(DataType::Operator, tokens[tokenIter].value);
                    else if (tokens[tokenIter].type == TokenType::STRING_LITERAL)
						listEntities.emplace_back(DataType::String, tokens[tokenIter].value);
				}
				m_operations.emplace(m_operations.size(), std::move(listEntities));
				listEntities.clear();
				tokenIter--;
			}
			
		}
	}

	//create table table_name(
	//	column1 datatype[constraints],
	//	column2 datatype[constraints],
	//	...
	//	[table_constraints]
	//);
	void Compiler::ExecuteBody::createConstructor(std::deque<Token>& tokens)
	{
		std::vector<Entity> listEntities;
		auto table = std::ranges::find_if(tokens, [](Token& token)
			{
				return token.type == TokenType::IDENTIFIER;
			});
		m_operations[m_operations.size()].emplace_back(DataType::Table,table->value);
		bool begin = false;
		for (int i = 0; i < tokens.size(); i++)
		{
			if (tokens[i].type == TokenType::LEFTPAREN)
			{
				begin = true;
			}
			if (tokens[i].type == TokenType::RIGHTPAREN)
				break;
			if (begin)
			{
				if (tokens[i].type == TokenType::SIZE)
				{
					int finish = tokens[i].value.size()+1;
					for (int n = 0;n<finish;++n,++i)
					{
						if (tokens[i].type == TokenType::IDENTIFIER)
							listEntities.emplace_back(DataType::Column, tokens[i].value);
						else if (tokens[i].type == TokenType::DATATYPE)
						{
							if (tokens[i].value == "int")
								listEntities.emplace_back(DataType::Int, tokens[i].value);
							else
								listEntities.emplace_back(DataType::String, tokens[i].value);
						}
						else if (tokens[i].type == TokenType::NUMBER)
							listEntities.emplace_back(DataType::Number, tokens[i].value);
						if (listEntities.size()==3)
						{
							m_operations.emplace(m_operations.size(), std::move(listEntities));
							listEntities.clear();
						}
					}
					
				}

			}
		}
	}
	void Compiler::ExecuteBody::analyzer(std::deque<Token>& tokens)
	{
		if (m_task == SQLType::Select)
            selectConstructor(tokens);
        if (m_task == SQLType::Insert)
            insertConstructor(tokens);
        if (m_task == SQLType::Delete)
            deleteConstructor(tokens);
        if (m_task == SQLType::Update)
			updateConstructor(tokens);
        if (m_task == SQLType::Create)
			createConstructor(tokens);
	}

	Compiler::ExecuteBody::ExecuteBody(std::deque<Token>& tokens)
		:m_valid(true)
	{
		construct(tokens);
	}
	void Compiler::ExecuteBody::construct(std::deque<Token>& tokens)
	{
		m_task = getTask(tokens.front().value);
		analyzer(tokens);
	}
	bool Compiler::ExecuteBody::isValid()const
	{
		return m_valid;
	}
	void Compiler::ExecuteBody::setValid(bool newValue)
	{
		m_valid = newValue;
	}
	const std::string& Compiler::ExecuteBody::getMessage()const
	{
		return m_message;
	}
	void Compiler::ExecuteBody::setMessage(const std::string& newMessage)
	{
		m_message = newMessage;
	}
	std::unordered_map<unsigned, std::vector<Entity>>& Compiler::ExecuteBody::get()
	{
		return m_operations;
	}
	PrimedDB::SQLType Compiler::ExecuteBody::getTask()
	{
		return m_task;
	}
	//keywords list
	static const std::vector<std::string> keywords = {
			"select", "from", "where", "insert", "into", "values",
			"delete", "create", "table", "user",
			"update", "set", "null","database","session"
	};
	static const std::vector<std::string> dataType={"int","number","varchar","integer" };
	//map of syntax rules
	std::unordered_map<std::string, std::vector<std::vector<TokenType>>> Compiler::m_rules;

	void Compiler::constructRules()
	{
		//select columns from table where condition;
		std::vector<TokenType> rule = { TokenType::KEYWORD,TokenType::MULTIPLE, TokenType::KEYWORD, TokenType::IDENTIFIERS,TokenType::IDENTIFIERS,TokenType::KEYWORD,TokenType::EXPRESSION,TokenType::END };
		m_rules["select"].emplace_back(std::move(rule));
		rule = { TokenType::KEYWORD,TokenType::MULTIPLE, TokenType::KEYWORD, TokenType::IDENTIFIERS,TokenType::END };
		m_rules["select"].emplace_back(std::move(rule));
		rule = { TokenType::KEYWORD,TokenType::IDENTIFIERS, TokenType::KEYWORD, TokenType::IDENTIFIERS,TokenType::END };
		m_rules["select"].emplace_back(std::move(rule));
		//select columns from table;
		rule = { TokenType::KEYWORD,TokenType::IDENTIFIERS, TokenType::KEYWORD, TokenType::IDENTIFIERS,TokenType::KEYWORD,TokenType::EXPRESSION,TokenType::END };
		m_rules["select"].emplace_back(std::move(rule));
		//select * from table;

		//select * from table where condition;



		//insert into table (columns) values (values);
		rule = { TokenType::KEYWORD,TokenType::KEYWORD, TokenType::IDENTIFIER,TokenType::LEFTPAREN,TokenType::IDENTIFIERS,TokenType::RIGHTPAREN,TokenType::KEYWORD,TokenType::LEFTPAREN,TokenType::VALUE,TokenType::RIGHTPAREN,TokenType::END };
		m_rules["insert"].emplace_back(std::move(rule));

		//update
		rule = { TokenType::KEYWORD , TokenType::IDENTIFIER,TokenType::KEYWORD,TokenType::REPEAT, TokenType::EXPRESSION, TokenType::KEYWORD,TokenType::EXPRESSION,TokenType::END };
		m_rules["update"].emplace_back(std::move(rule));

		//delete
		rule = { TokenType::KEYWORD , TokenType::KEYWORD, TokenType::IDENTIFIER,TokenType::KEYWORD,TokenType::EXPRESSION,TokenType::END };
		m_rules["delete"].emplace_back(std::move(rule));
		rule = { TokenType::KEYWORD , TokenType::KEYWORD, TokenType::IDENTIFIER,TokenType::END };
		m_rules["delete"].emplace_back(std::move(rule));

		////EXPRESSION
		//rule = { TokenType::NOT, TokenType::EXPRESSION };
		//m_rules["expression"].emplace_back(std::move(rule));
		rule = { TokenType::LEFTPAREN, TokenType::EXPRESSION,TokenType::RIGHTPAREN };
		m_rules["expression"].emplace_back(std::move(rule));
		rule = { TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::STRING_LITERAL, TokenType::LOGICAL_OPERATOR,TokenType::EXPRESSION };
		m_rules["expression"].emplace_back(std::move(rule));
		rule = { TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::NUMBER, TokenType::LOGICAL_OPERATOR,TokenType::EXPRESSION };
		m_rules["expression"].emplace_back(std::move(rule));
		rule = { TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::STRING_LITERAL};
		m_rules["expression"].emplace_back(std::move(rule));
		rule = { TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::NUMBER };
		m_rules["expression"].emplace_back(std::move(rule));

		rule = { TokenType::EXPRESSION, TokenType::LOGICAL_OPERATOR,TokenType::EXPRESSION };
		m_rules["expression"].emplace_back(std::move(rule));

		//create
		rule = { TokenType::KEYWORD, TokenType::KEYWORD, TokenType::IDENTIFIER ,TokenType::LEFTPAREN,TokenType::REPEAT, TokenType::IDENTIFIER, TokenType::AND, TokenType::DATATYPE,TokenType::AND,TokenType::NUMBER,TokenType::AND,TokenType::DELIMITER,TokenType::RIGHTPAREN, TokenType::END };
		m_rules["create"].emplace_back(std::move(rule));

		rule = { TokenType::LEFTPAREN,TokenType::REPEAT,TokenType::IDENTIFIER, TokenType::AND, TokenType::DELIMITER, TokenType::RIGHTPAREN };
		m_rules["columns"].emplace_back(std::move(rule));

		rule = { TokenType::REPEAT ,TokenType::NUMBER,TokenType::AND, TokenType::DELIMITER,TokenType::OR,TokenType::STRING_LITERAL,TokenType::AND, TokenType::DELIMITER};
		m_rules["value"].emplace_back(std::move(rule));


		rule = { TokenType::REPEAT , TokenType::IDENTIFIER, TokenType::AND, TokenType::DELIMITER };
		m_rules["identifiers"].emplace_back(std::move(rule));


		rule = { TokenType::REPEAT, TokenType::EXPRESSION , TokenType::AND, TokenType::DELIMITER };
		m_rules["assign"].emplace_back(std::move(rule));

	}
	static void ReportInfo(const std::string& name,const std::string& message)
	{
		StaticFunc::WriteInfo(name, message);
	}
	// 辅助函数实现
	void Compiler::toLower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](char c)
		{
				if (c !='\n')
				{
					c = std::tolower(c);
				}
				else
					c = ' ';
				return c;
		});
	}

	std::string Compiler::trim(const std::string& str)
	{
		size_t first = str.find_first_not_of(static_cast<char>(' '));
		if (std::string::npos == first) return str;
		size_t last = str.find_last_not_of(static_cast<char>(' '));
		return str.substr(first, (last - first + 1));
	}

	// Token类型检查函数
	bool Compiler::isKeyword(std::string& token)
	{
		return std::find(keywords.begin(), keywords.end(), token) != keywords.end();
	}
	bool Compiler::isDataType(std::string& token)
	{
		return std::find(dataType.begin(), dataType.end(), token) != dataType.end();
	}
	bool Compiler::isIdentifier(std::string& token)
	{
		if (token.empty()) return false;
		// 标识符必须以字母开头，可包含字母、数字和下划线
		if (!std::isalpha(token[0]) && token[0] != '_') return false;
		if (std::ranges::any_of(token, [](char c)
			{
				return !std::isalnum(c);
			}))
			return false;

		return !isKeyword(token);
	}

	bool Compiler::isNumber(std::string& token)
	{
		if (token.empty()||(token.size()==1&&!isdigit(token[0]))) return false;
		auto notNumber = token | std::views::filter([](char c)
			{
				return !isdigit(c);
			});
		for (auto& ch: notNumber)
		{
			if (ch != '-')
				return false;
		}
		return true;
	}

	bool Compiler::isStringLiteral(std::string& token)
	{
		if (token.length() < 2) return false;
		char left = token.front();
		char right = token.back();
		if (left == right && left == '\'')
		{
			token.pop_back();
			token.erase(0, 1);
			return true;
		}
		return false;
	}

	TokenType Compiler::getTokenType(std::string& token)
	{
		if (token == "and" || token == "or")
			return TokenType::LOGICAL_OPERATOR;
		else if (token == "not")
			return TokenType::NOT;
		if (isKeyword(token)) return TokenType::KEYWORD;
		if (isDataType(token)) return TokenType::DATATYPE;
		if (isIdentifier(token)) return TokenType::IDENTIFIER;
		if (isNumber(token)) return TokenType::NUMBER;
		if (isStringLiteral(token)) return TokenType::STRING_LITERAL;
		
		// 检查操作符
		if (token == "=" || token == "!=" || token == "<" || token == ">" || 
			token == "<=" || token == ">=" ) {
			return TokenType::OPERATOR;
		}

		else if (token == "*")
			return TokenType::MULTIPLE;
		// 检查分隔符
		else if (token == "," ) {
			return TokenType::DELIMITER;
		}
		else if (token == "(")
			return TokenType::LEFTPAREN;
		else if (token == ";")
            return TokenType::END;
		else if (token == ")")
			return TokenType::RIGHTPAREN;
		return TokenType::UNKNOWN;
	}
	int Compiler::syntaxChecker(const std::vector<TokenType>& rule, std::deque<Token>& tokens, unsigned& tokenIter)
	{
		bool skip = false;
		for (unsigned ruleIter = 0; ruleIter < rule.size()&& tokenIter<tokens.size(); ++ruleIter)
		{
			if (rule[ruleIter] == TokenType::REPEAT)
			{
				auto begin = tokenIter;
				auto value = repeatCheck(rule, ruleIter, tokens, tokenIter);

				if (value == -1)
				{
					ReportInfo("InvalidRepeat", "The format of repetition is incorrect.");
					return tokenIter;
				}
				else if (value == 0)
				{
					;
				}
				else
				{
					std::string size(tokenIter-begin, '.');
					tokens.insert(tokens.begin() + begin,Token(TokenType::SIZE,size));

					tokenIter++;
				}
				continue;
			}
			else if (rule[ruleIter] == TokenType::EXPRESSION)
			{
				if (expressionCheck(tokens, tokenIter))
					continue;
				ReportInfo("InvalidExpression", "Given expression is invalid.");
				return tokenIter;
			}
			else if (rule[ruleIter] == TokenType::VALUE)
			{
				if (valueCheck(tokens, tokenIter))
					continue;
				ReportInfo("InvalidValueList", "Given value list is invalid.");
				return tokenIter;
			}
			else if (rule[ruleIter] == TokenType::COLUMNS)
			{
				if (columnsCheck(tokens, tokenIter))
					continue;
				ReportInfo("InvalidColumnList", "Given column list is invalid.");
				return tokenIter;
			}
			else if (rule[ruleIter] == TokenType::IDENTIFIERS)
			{
				if (identifiersCheck(tokens, tokenIter))
					continue;
				ReportInfo("InvalidIdentifierList", "Given IdentifierList is invalid.");
				return tokenIter;
			}
			else if (rule[ruleIter] == TokenType::END && tokens.size()-tokenIter == 1)
			{
				ReportInfo("SyntaxCorrect", "Given command passed the Syntax check");
				return -1;
			}
			else if (rule[ruleIter] == TokenType::OR)
				continue;
			if (tokenIter >= tokens.size()&& rule.size() - ruleIter <= 2)
				return -1;
			if (tokens[tokenIter].type != rule[ruleIter])
			{
				if (skip|| rule.size() - ruleIter == 1)
				{
					ReportInfo("SyntaxError", std::format("{} is type {} but {} required",
						tokens[tokenIter].value,
						type2string(tokens[tokenIter].type), type2string(rule[ruleIter])));
					return tokenIter;
				}
				skip = true;
			}
			else
			{
				if (skip)
					skip = false;
				++tokenIter;
			}
		}
		return -1;
	}
	std::deque<Token> Compiler::parseSQL(const std::string& sql, std::string& error)
	{
		std::deque<Token> tokens;
		std::string currentToken;
		std::string internal = "Error:[ParsingError]";
		bool inQuotes = false;
		char quoteChar = '"';
		size_t position = 0;
		if (sql.empty())
		{
			internal += "Given command is empty (Parsing Error)";
			error = std::move(internal);
		}
		else
		{
			for (size_t i = 0; i < sql.length(); ++i) {
				char c = sql[i];
				position = i;
				if (!inQuotes) {
					if (c == '"' || c == '\'') {
						inQuotes = true;
						quoteChar = c;
						currentToken += c;
					}
					else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
						if (!currentToken.empty()) {
							TokenType type = getTokenType(currentToken);
							if (type == TokenType::UNKNOWN)
							{
								internal += "Invalid token detected(Parsing Error)";
								error = std::move(internal);
								return tokens;
							}
							tokens.emplace_back(type, currentToken, position - currentToken.length());
							currentToken.clear();
						}
					}
					else if (c == ',' || c == '(' || c == ')' || c == ';' || c == '=' || c == '<' || c == '>' || c == '!') {
						if (!currentToken.empty()) {
							TokenType type = getTokenType(currentToken);
							if (type == TokenType::UNKNOWN)
							{
								internal += "Invalid token detected (Parsing Error)";
								error = std::move(internal);
								return tokens;
							}
							tokens.emplace_back(type, currentToken, position - currentToken.length());
							currentToken.clear();
						}
						if (c != ';') {
							std::string delim(1, c);
							TokenType type = getTokenType(delim);
							if (type == TokenType::UNKNOWN)
							{
								internal += "Invalid token detected (Parsing Error)";
								error = std::move(internal);
								return tokens;
							}
							tokens.emplace_back(type, delim, position);
						}
					}
					else {
						currentToken += c;
					}
				}
				else {
					currentToken += c;
					if (c == quoteChar && (i == 0 || sql[i - 1] != '\\')) {
						inQuotes = false;
					}
				}
			}
			if (!currentToken.empty()) {
				TokenType type = getTokenType(currentToken);
				tokens.emplace_back(type, currentToken, position - currentToken.length());
			}
		}
		return std::move(tokens);
	}
	std::string Compiler::type2string(TokenType type)
	{
		if (type == TokenType::EXPRESSION)
		{
			return "expression";
		}
		else if (type == TokenType::LEFTPAREN)
		{
			return "LEFTPAREN";
		}
		else if (type == TokenType::RIGHTPAREN)
		{
			return "RIGHTPAREN";
		}
		else if (type == TokenType::KEYWORD)
		{
			return "KEYWORD";
		}
		else if (type == TokenType::IDENTIFIER)
		{
			return "IDENTIFIER";
		}
		else if (type == TokenType::IDENTIFIERS)
		{
			return "identifiers";
		}
		else if (type == TokenType::IDENTIFIER_TABLE)
		{
			return "IDENTIFIER_TABLE";
		}
		else if (type == TokenType::IDENTIFIER_COLUMN)
		{
			return "IDENTIFIER_COLUMN";
		}
		else if (type == TokenType::LOGICAL_OPERATOR)
		{
			return "LOGICAL_OPERATOR";
		}
		else if (type == TokenType::COLUMNS)
		{
			return "columns";
		}
		else if (type == TokenType::NOT)
		{
			return "not";
		}
		else if (type == TokenType::STRING_LITERAL)
		{
			return "STRING_LITERAL";
		}
		else if (type == TokenType::VALUE)
		{
			return "value";
		}
		else if (type == TokenType::NUMBER)
		{
			return "number";
		}
		else if (type == TokenType::OR)
		{
			return "or";
		}
		else if (type == TokenType::OPERATOR)
		{
			return "OPERATOR";
		}
		else if (type == TokenType::DELIMITER)
		{
			return "DELIMITER";
		}
		else if (type == TokenType::MULTIPLE)
		{
			return "MULTIPLE";
		}
		else if (type == TokenType::REPEAT)
		{
			return "REPEAT";
		}
		else if (type == TokenType::END)
		{
			return "END";
		}
		else
		{
			return "UNKNOWN";
		}
	}
	bool Compiler::columnsCheck( std::deque<Token>& tokens, unsigned& start)
	{
		auto& rules = m_rules["columns"];
		unsigned begin = start;
		for (auto& rule : rules)
		{
			if (syntaxChecker(rule, tokens, start) == -1)
			{
				std::string size(start - begin, '.');
				tokens.insert(tokens.begin() + begin, Token(TokenType::SIZE, size));
				start++;
				return true;
			}

		}
		return false;
	}
	bool Compiler::identifiersCheck( std::deque<Token>& tokens, unsigned& start)
	{
		auto& rules = m_rules["identifiers"];
		for (auto& rule : rules)
		{
			unsigned begin = start;
			unsigned typeStart = 0;
			if (repeatCheck(rule, typeStart, tokens, begin))
			{
				std::string size(begin - start, '.');
				tokens.insert(tokens.begin() + start, Token(TokenType::SIZE, size));
				start = begin;
				start++;
				return true;
			}

		}
		return false;
	}
	bool Compiler::valueCheck( std::deque<Token>& tokens, unsigned& start)
	{
		auto& rules = m_rules["value"];
		unsigned begin = start;
		unsigned typeStart = 0;
		for (auto& rule : rules)
		{
			if (repeatCheck(rule, typeStart, tokens, begin))
			{
				std::string size(begin - start, '.');
				tokens.insert(tokens.begin() + start, Token(TokenType::SIZE, size));
				start = begin;
				start++;
				return true;
			}

		}
		return false;
	}
	int Compiler::repeatCheck(const std::vector<TokenType>& types, unsigned& typeStart, std::deque<Token>& tokens, unsigned& tokenIter)
	{
		typeStart++;
		TokenType mainType = types[typeStart];
		if (types[typeStart] == TokenType::EXPRESSION)
		{
			while (expressionCheck(tokens, tokenIter))
			{
				if (tokens[tokenIter].type == TokenType::DELIMITER)
					tokenIter++;
				else
					break;
			}
			if (tokens.size() - tokenIter == 1 
				|| tokens[tokenIter].type == TokenType::END
				|| tokens[tokenIter].type == TokenType::KEYWORD)
				return 0;
			return -1;
		}
		else
		{
			std::unordered_map<TokenType, std::vector<TokenType>> repetition;
			bool finish = false;
			bool change = false;
			unsigned times = 0;
			for (int sourceIter = typeStart; sourceIter < types.size(); sourceIter++)
			{
				//if finish
				if (finish)
				{
					if (change)
					{
						mainType = types[sourceIter];
						change = false;
						finish = false;
						sourceIter--;
						continue;
					}

					//check any repetition token left
					if (types[sourceIter] == TokenType::OR)
					{
						//move one step forward to receive new token
						change = true;
						continue;
					}
					else if (types[sourceIter] == TokenType::AND)
					{
						//move one step forward to receive new token
						finish = false;
						continue;
					}
					//no repeat token left
					else
						break;
				}
				repetition[mainType].push_back(types[sourceIter]);
				//put into finish state
				finish = true;
			}
			change = true;
			std::vector<TokenType> currentList;
			for (int typeIter = 0; tokenIter < tokens.size(); )
			{
				if (change)
				{
					if (repetition.contains(tokens[tokenIter].type))
						currentList = repetition[tokens[tokenIter].type];
					else
					{
						return ++times;
					}

					change = false;
					continue;
				}
				else
				{
					if (currentList[typeIter] != tokens[tokenIter].type)
					{
						if (currentList.size() - typeIter == 1 || typeIter == 0)
						{
							return ++times;
						}
						break;
					}
					typeStart++;
					typeIter++;
					if (typeIter == currentList.size())
					{
						times++;
						change = true;
						typeIter = 0;
					}
				}
				tokenIter++;
			}
			return -1;
		}
	}
	bool Compiler::expressionCheck( std::deque<Token>& tokens, unsigned& start)
	{
		auto& rules = m_rules["expression"];

		bool skip = false;
		for (auto&rule:rules)
		{
			unsigned begin = start;
			if (tokens[start].type != rule.front())
				continue;
			if (syntaxChecker(rule, tokens, begin) == -1)
			{
                std::string size(begin - start, '.');
                tokens.insert(tokens.begin() + start, Token(TokenType::SIZE, size));
				start = begin;
				start++;
				return true;
			}
		}
		return false;
	}
	Compiler::ExecuteBody Compiler::ruleMatch(const std::vector<TokenType>& rules,std::deque<Token>& tokens)
	{
		
		ExecuteBody executeBody;
		unsigned counter = 0;
		if (syntaxChecker(rules, tokens, counter) == -1)
			return ExecuteBody(tokens);
		return ExecuteBody("Syntax check failed");
	}
	Compiler::ExecuteBody Compiler::syntaxFailed(std::string& error, const std::string& message)
	{
		error = "Error:[SyntaxError]";
		error += message;
		return ExecuteBody(message);
	}
	Compiler::ExecuteBody Compiler::syntaxCheck(UserPtr user, std::deque<Token>& tokens, std::string& error)
	{
		if (tokens.empty())
			return syntaxFailed(error, "No tokens found");
		auto& first = tokens.front();
		if (first.type != TokenType::KEYWORD)
			return syntaxFailed(error, std::format("{} is not a sql keyword.",first.value));
		if (!m_rules.contains(first.value))
			return syntaxFailed(error, std::format("{} is either not supported by current version of PrimedDB or this keyword cannot be use at the beginning of the sql statement.",first.value));

		auto& ruleSet = m_rules[first.value];
		std::vector<std::future<ExecuteBody>> ruleMatches;
		for (auto& rule : ruleSet)
		{
			auto temp = tokens;
			auto execute = ruleMatch(rule, temp);
			if (execute.isValid())
			{
				tokens = std::move(temp);
				return execute;
			}
		}
		return syntaxFailed(error, "Given command does not match any pre-defined rule in compiler.");
	}
	Record Compiler::execute(UserPtr user, ExecuteBody& body,std::string& error)
	{
		auto task = body.getTask();
		auto& operations = body.get();
		std::string internal = "Error:[RuntimeError]";
		if (task == SQLType::Delete||task == SQLType::Insert||task == SQLType::Update)
		{
			try
			{
				std::string& tableName = operations[0].at(0).content;
				auto ptr = TableManager::Get().get_noLock(tableName);
				std::unordered_map<std::string, std::string> where;
				bool exit = false;
				if (ptr == nullptr)
				{
					internal += std::format("Table [{}] do not exists", tableName);
					return Record();
				}
				else
				{
					auto& columnInfo = ptr->getColumns();
					if (task == SQLType::Delete)
					{
						if (operations.size() - 1 == 0)
							;
						for (int n = 1; n < operations.size(); ++n)
						{
							auto& operation = operations[n];
							where[operation[0].content] = operation[2].content;
						}
						auto result = ptr->where(where);
						where.clear();
						for (int n = 0;n<result.size();++n)
						{
							if (result[n])
								ptr->remove(user->getName(), n);
						}
					}
					else if (task == SQLType::Insert)
					{

						auto& columnList = operations[1];
						auto& valueList = operations[2];
						if (columnList.size() != ptr->columnSize() || columnList.size() != valueList.size())
						{
							internal += std::format("Column number do not match with definition of [{}]", ptr->getName());
							error = std::move(internal);
							return Record();
						}

						if (std::any_of(columnList.begin(), columnList.end(), [&](Entity& column) {
							return !ptr->existColumn(column.content);
						}))
						{
							exit = true;
                            internal += std::format("Column name not found in [{}]", ptr->getName());
							error = std::move(internal);
							return Record();
						};
						if (!std::ranges::equal(columnList, columnInfo, [&](const Entity& column, const Column& info)
							{
								return column.content == info.getName();
							}))
						{
							exit = true;
							internal += std::format("Given order of column is incorrect");
							error = std::move(internal);
							return Record();
						}

						if (!exit)
						{
							UCharPtr data = std::make_unique<char[]>(ptr->byteSize());
							StaticFunc::clearMemory(data.get(), ptr->byteSize());
							int offset = 0;
							for (int n = 0; n < columnList.size(); ++n)
							{
								if (valueList[n].m_type == DataType::Number)
								{
									if (columnInfo[n].getType() != PrimedDB::DataType::Int)
									{
										internal += std::format("Given type not match the defined type with {}", ptr->getColumns()[n].getName());
										exit = true;
										break;
									}
									unsigned value = std::stoi(valueList[n].content);
									memcpy_s(data.get()+offset, columnInfo[n].size(), &value, columnInfo[n].size());
								}
								else if (valueList[n].m_type == DataType::String)
								{
									if (columnInfo[n].getType() != PrimedDB::DataType::Varchar)
									{
										internal += std::format("Given type does not match the defined type [{}]", ptr->getColumns()[n].getName());

										error = std::move(internal);
										return Record();
									}
									auto cpySize = valueList[n].content.size() >  columnInfo[n].size() ? columnInfo[n].size() : valueList[n].content.size();
									memcpy_s(data.get() + offset, columnInfo[n].size(), valueList[n].content.data(), cpySize);
								}
								offset += columnInfo[n].size();
							}
							if (!exit)
								ptr->insert(user->getName(), std::move(data));
							internal = "Insert success";
							error = std::move(internal);
							return Record(true);
						}

					}
					else
					{
						int n = 1;
						std::vector<std::string> columns;
						std::vector<std::string> values;
						for (; n < operations.size(); ++n)
						{
							columns.emplace_back(operations[n][0].content);
							values.emplace_back(operations[n][2].content);
							if (operations[n].empty())
							{
								internal += "Unexpected compiler error";
								error = std::move(internal);
								return Record();
							}
						}
						if (!std::ranges::equal(columns, columnInfo, [&](const std::string& column, const Column& info)
							{
								return column == info.getName();
							}))
						{
							internal += std::format("Given order of column is incorrect ");
							error = std::move(internal);
							return Record();
						}
						else
						{
							auto iter = std::find_if(columns.begin(), columns.end(), [&](const std::string& column)
								{
									return !ptr->existColumn(column);
								});
							if (iter != columns.end())
							{
								internal += std::format("Given column {} do not exist ", *iter);
								error = std::move(internal);
								return Record();
							}
							if (columns.size() != columnInfo.size())
								internal += "For current version only fully update will be accepted by PrimedDB Compiler ";
							else
							{

								for (; n < operations.size(); ++n)
								{
									where[operations[n][0].content] = operations[n][2].content;
								}
								auto result = ptr->where(where);
								UCharPtr data;
								int sum = 0;
								for (int pos; pos < result.size(); ++pos)
								{
									if (result[pos])
									{
										data = std::make_unique<char[]>(ptr->byteSize());
										StaticFunc::clearMemory(data.get(), ptr->byteSize());
										int sum = 0;
										for (int n = 0; n < columns.size(); ++n)
										{
											if (columnInfo[n].getType() == DataType::String)
												memcpy_s(data.get() + sum, columnInfo[n].size(), values[n].c_str(), columnInfo[n].size());
											else if (columnInfo[n].getType() == DataType::Int)
											{
												unsigned temp = std::stoi(values[n]);
												memcpy_s(data.get() + sum, columnInfo[n].size(), &temp, columnInfo[n].size());
											}
											sum += columnInfo[n].size();
										}
										ptr->update(user->getName(), pos, std::move(data));
									}
								}
							}
						}
					}
				}
			}
			catch (std::out_of_range& e)
			{
				Util::ErrorManager::Get().set(Util::ErrorLevel::Error, "Error", e.what(),Infor::ClassInfor(THISFUNC,THISFILE));
			}
			return Record();
		}
		else if (task == SQLType::Select)
		{
			auto& columns = operations[0];
			auto& tables = operations[1];
			TablePtr ptr = TableManager::Get().get_noLock(tables[0].content);
			if (ptr == nullptr)
			{
				internal += std::format("Table {} do not exist", tables[0].content);
                error = std::move(internal);
				return Record();
			}
			Record record;
			if (columns.size() == 1 && columns[0].m_type == DataType::All)
			{
				record = ptr->select();
				record.all();
			}
			else
			{
				record = ptr->select();
				for (auto& column : columns)
				{
					record.include(column.content);
				}
			}
			if (operations.size() >= 3)
			{
				std::unordered_map<std::string, std::string> where;
				for (int n = 2;n < operations.size();++n)
				{
					if (operations[n].size() !=  3)
						continue;
					where[operations[n][0].content] = operations[n][2].content;
					auto available = std::move(ptr->where(where));
					record.where(available);
				}
			}
			return record;
		}
		else if (task == SQLType::Create)
		{
			std::string tableName = std::move(operations[0][0].content);
			std::deque<Column> columns;
			for (int n = 1;n<operations.size();++n)
			{
				auto type = operations[n][1].m_type == DataType::String ? DataType::Varchar : DataType::Int;
				columns.emplace_back(operations[n][0].content,
					stoi(operations[n][2].content),
					tableName,
					type);
			}
			if (TableManager::Get().exist(tableName))
			{
				internal = std::format("Table {} exists\n", tableName);
				return Record(true);
			}
			auto ptr = TableManager::Get().add(*user,tableName, columns);
			internal = std::format("Table {} created\n", ptr->getName());
			error = std::move(internal);
			return Record(true);
		}
		return Record();
	}


	Compiler::Compiler()
	{
		constructRules();
	}
	// 主编译函数
	std::future<Result> Compiler::compile(UserPtr user,const std::string& sql)
	{
		return std::async(std::launch::async, [user, sql]()
			{
				std::string inner = sql;
				// 验证SQL语句
				Transaction transection;
				Record record;
				toLower(inner);
				if (inner == "commit"|| inner == "commit;")
				{
					user->commit();
					CallInfo("CompilerMessage", "Commit executed");
					return Result(true, "Commit success", 0);
				}
				std::string error;
				auto tokens = parseSQL(inner, error);

				if (error.empty()) {
					CallInfo("CompilerMessage", "Parsing Success");
					auto value = syntaxCheck(user, tokens, error);
					if (value.isValid())
					{
						CallInfo("CompilerMessage", "Syntax check pass");
						auto result = std::move(execute(user, value, error));
						if (result.isValid())
						{
							if (error.empty())
								return Result(result.isValid(), result.format(), 0);
							else
								return Result (result.isValid(), error, 0);
						}

					}
				}
				// 词法分析
				return Result(false, error, 0);

			});
	}
}
