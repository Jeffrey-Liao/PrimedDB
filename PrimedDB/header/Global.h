#pragma once
#include "TimeStamp.h"
#include "HashContainer.h"
namespace liao
{
	namespace PrimedDB
	{
		class Table;
		class User;
		class Column;

		enum class DataType :char
		{
			Int,
			Varchar,
			Number,
			Text,
			Time,
			Date
		};
		enum class ColumnType :char
		{
			Primary,
			Foreign,
			Normal
		};
		enum class UserLevel :char
		{
			None,
			Visitor,
			SuperVisitor,
			User,
			SuperUser,
			Manager,
			SuperManager,
			Administrator
		};
	}
	enum class ErrorCode :char
	{
		Nothing,
	};

	DYNAMIC
	concept Numeric = std::integral<T> || std::floating_point<T>;

	class StaticFunc
	{
		static ShareMutex m_mutex;
	public:
		template<Numeric T>
		static T GetRandom(T lower, T upper)
		{
			std::random_device rd;  // 硬件随机数生成器（用于种子）
			std::mt19937 gen(rd()); // 梅森旋转算法引擎（用随机种子初始化）
			std::uniform_int_distribution<T> dis(lower, upper); // 均匀整数分布 [0, 99]
			return dis(gen);
		}
		static std::string GetUniqueId(Math::HashType type = Math::HashType::SHA256);
		static void Split(std::vector<std::string>& out, const std::string& s, char delimiter);
	};
	class Configuration
	{
	public:
		static Math::HashType UserIDHashType;
		static std::string UserInforFile;
	};


	DYNAMIC
	concept Concept_NullRefField = requires(T obj)
	{
		std::same_as<T,PrimedDB::Column>
		|| std::same_as<T, PrimedDB::User>
		|| std::same_as<T, PrimedDB::Table>;
	};

	DYNAMICCON(Concept_NullRefField)
	class NullRefProvider
	{
	public:
		static T NullRef;
		static bool isNullObject(const T& obj)
		{
			return obj.getId() == "null";
		}
	};
	DYNAMIC
	concept Concept_Singleton = !std::is_default_constructible_v<T> &&
		!std::is_copy_constructible_v<T> &&
		!std::is_move_constructible_v<T>;
	DYNAMICCON(Concept_Singleton)
	class Singleton
	{
		static T instance;
		ShareMutex m_mutex;
	public:
		T& getInstance()
		{
			return instance;
		}
	};
}
