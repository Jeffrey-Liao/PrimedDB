#pragma once
#include "TimeStamp.h"
#include "HashContainer.h"
namespace liao
{
	namespace PrimedDB
	{
		enum DataType :char
		{
			Int,
			Varchar,
			Number,
			Text,
			Time,
			Date
		};
		enum ColumnType :char
		{
			Primary,
			Foreign,
			Normal
		};
		enum UserLevel :char
		{
			Visitor,
			SuperVisitor,
			User,
			SuperUser,
			Manager,
			SuperManager,
			Administrator
		};
	}
	enum ErrorCode :char
	{
		Nothing,

	};

	static int userIdGenSeed = 0;
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
	};

}
