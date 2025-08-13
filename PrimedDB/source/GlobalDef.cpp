#include "Global.h"

#include <random>
USESTD;
USECRPT;
namespace liao {
	std::string StaticFunc::GetUniqueId(Math::HashType type)
	{
		int random = GetRandom(0, 100);
		Util::TimeStamp timeStamp(Util::TimeStamp::SystemClock::now());
		Math::HashContainer container;
		container.generate(timeStamp.getString() + to_string(random),type);
		return container.getHashHex();
	}
}