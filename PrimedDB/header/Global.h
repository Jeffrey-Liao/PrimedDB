#pragma once
#include "TimeStamp.h"
#include "HashContainer.h"
namespace liao
{
	namespace PrimedDB
	{
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
	};
}
