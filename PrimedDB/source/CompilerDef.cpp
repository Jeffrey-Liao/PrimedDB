#include "Compiler.h"
namespace liao::Compiler
{
	std::future<Result> Compiler::compile(PrimedDB::UserPtr ptr, std::string& command)
	{
		return std::async(std::launch::async, [&]() -> Result
			{
				if (ptr != nullptr)
				{

				}
				return Result();
			});
		
	}
}