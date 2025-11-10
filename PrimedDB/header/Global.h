#pragma once
#include "TimeStamp.h"
#include "HashContainer.h"
#include "Singleton.h"
#include <any>

//.def file for structure definition. .inf file for informations. .dat file for data.
namespace liao
{
	static std::atomic<bool> BlockDead = false;
	static std::atomic<bool> UserDead = false;
	static std::atomic<bool> TableDead = false;
	static ShareMutex TableFileMutex;
#define COLUMN_NAME_LEN 50
#define TABLE_NAME_LEN 50
#define USER_NAME_LEN 50
	namespace PrimedDB
	{
		//for column

		enum class UserLevel :char
		{
			None,
			Visitor,
			SuperVisitor,
			User,
			SuperUser,
			Manager,
			SuperManager,
			Administrator,
			System
		};
	}
	class Global:public Singleton<Global>
	{
		std::unordered_map<std::string, int> m_values;
	public:
		void set(std::string name, int value);
        int get(std::string name);
		bool exist(std::string name);
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
		static std::string GetUniqueId(Math::HashType type = Math::HashType::MD5);
		static std::string GetHashKey(const std::string& seed,Math::HashType type = Math::HashType::MD5);
		static void Split(std::vector<std::string>& out, const std::string& s, char delimiter);
		static std::shared_ptr < std::fstream > OpenDataFile(const std::string& name);
		static unsigned ByteConvert(unsigned byte);
		static std::string vectorToString(const std::deque<std::shared_ptr<std::string>>& vec,const std::vector<bool>&);
		static unsigned MaxSizeForBlock(unsigned byteSize);
		static void WriteInfo(const std::string& title, const std::string& message);
		DYNAMICCON(std::integral)
		static long long VectorSum(std::vector<T>& vec,size_t size = vec.size())
		{
			if (size<=vec.size())
			{
				long long sum = 0;
				for (size_t n = 0; n < size; ++n)
				{
					sum+=vec[n];
				}
				return sum;
			}
			return -1;
		}
		static void StringToVectorBool(std::vector<bool>& result, const std::string& data);
		static bool ValidName(std::string& name);
		static void SplitFast(std::vector<size_t>& delimiters, const std::string& str, char delimiter);
		static void clearMemory(void* memory, size_t size);
	};

}
