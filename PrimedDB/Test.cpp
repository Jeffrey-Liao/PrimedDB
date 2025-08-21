#include "Global.h"
#include "UserManager.h"
#include <iostream>
USELIAO;
USESTD;
USELIAOMATH;
USELIAOPRIMED;
static ShareMutex mutexMain;
//void genId()
//{
//    auto id = StaticFunc::GetUniqueId(HashType::SHA256);
//    SimpleLock lock(mutexMain);
//    cout << id << "\n";
//}
int main()
{
    auto& ref = UserManager::getInstance();
    string name = "hello", password ="world1.." ;
    ref.create(name, password, UserLevel::Administrator);
    ref.login(name, password);
    int n = 0;
    cin>>n;
    cout << ref.remove(UserManager::GetSystemUser(), name) << "\n";


    cout << n;
    cin >> n;
    cout << n;
}