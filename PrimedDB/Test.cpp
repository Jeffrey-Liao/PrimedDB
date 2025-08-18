#include "Global.h"
#include "UserManager.h"
#include <iostream>
USELIAO;
USESTD;
USELIAOMATH;
USELIAOPRIMED;
static Mutex mutexMain;
void genId()
{
    auto id = StaticFunc::GetUniqueId(HashType::SHA256);
    SimpleLock lock(mutexMain);
    cout << id << "\n";
}
int main()
{
    auto& ref = UserManager::getInstance();
    string name = "hello", password ="world1.." ;
    ref.create(name, password, UserLevel::Administrator);
    name = "hel", password = "world";
    cout << (ref.create(name,password , UserLevel::Administrator) == nullptr);
}