#include <cassert>

#include "BlockManager.h"
#include "Compiler.h"
#include "Log.h"
#include "Table.h"
#include "UserManager.h"
using namespace liao::Infor;
using namespace liao::PrimedDB;
using namespace liao::Math;
using namespace liao::Util;
using namespace liao::Compiler;
using namespace std;
namespace liao::Test
{
	/*void testLog()
	{
		Log::PrintLine("This is a test");
		Log::Print("hello");
		Log::printDebug("hello world", "debug.log");
		Log::printError("hello world", "error.log");

		Log::Get()[LogType::Info].openToFile("hello.txt") << "hello" << "world" << Log::LogEndl;
	}
	void testTable()
	{
		string columnName = "name";
		Column column(columnName, 4, "hello", DataType::Int);
		assert(column.getName(), "name");
		assert(column.getOwner(), "hello");
		assert(column.size(), 4);
		assert(column.getType(), DataType::Int);
		assert(column.primed(), column.size() * 2);
		column.resize(10);
		assert(column.size(), 10);
		column.setType(DataType::Null);
        assert(column.getType(), DataType::Null);
		deque<Column> columns = { column };
		string tableName = "hello";
		Table table("system", tableName,UserLevel::Administrator,columns);
		assert(table.getAvailable().size(), 0);
		assert(table.getOwner(), "system");
		assert(table.getName(), "hello");
        assert(table.getPermission(), UserLevel::Administrator);
        assert(table.columnSize(), 1);
		assert(table.existColumn("hello"), true);
        assert(table.existColumn("world"), false);
		assert(table.isEmpty(), true);
	}

	void testCompiler()
	{
		string hello = "hello", name = "name";
		string world = "world", password = "password";
		thread create1([&]()
			{
				UserManager::Get().create(hello, world, UserLevel::Administrator);
			});
		thread create2([&]()
		{
				UserManager::Get().create(name, password, UserLevel::Administrator);
		}
		);
		assert(UserManager::Get().userCount(), 2);
	}*/
}
