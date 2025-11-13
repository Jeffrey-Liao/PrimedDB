#include "test/TransactionTest.h"

#include "Transaction.h"
using namespace liao;
namespace  liao::Test
{
	void TransactionTest::getterTest()
	{
		std::string user = "system";
		PrimedDB::Transaction transaction(user,"test",PrimedDB::SQLType::Create,1,1,1);
	}
	void TransactionTest::setterTest()
	{
		
	}
	void TransactionTest::run()
	{
		getterTest();
        setterTest();
		if (m_total)
		{
			PrintSuccess("Transaction Test");
			return;
		}
		PrintFail("Transaction Test");
	}
}
