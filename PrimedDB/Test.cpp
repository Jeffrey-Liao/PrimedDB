#include "Global.h"
#include <iostream>
USELIAO;
USESTD;
USELIAOMATH;
static Mutex mutexMain;
void genId()
{
    auto id = StaticFunc::GetUniqueId(HashType::SHA256);
    SimpleLock lock(mutexMain);
    cout << id << "\n";
}
int main()
{
    thread t1(genId);
    thread t2(genId);
    t1.join();
    t2.join();
}