#include "Global.h"
#include "User.h"
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
    string name = "liao", password = "654321";
    User user(name,password,UserLevel::Administrator);
    user.changePassword("123456");
    password = "123456";
    password = User::PassWordHash(password);
    cout<< user.validate(password) << "\n";
    name = "test";

    cout<< user.createTable(name, user.getLevel()).toString()<<endl;
    name = "hello";
    cout << user.createTable(name, user.getLevel()).toString()<<endl;
    name = "world";
    cout << user.createTable(name, user.getLevel()).toString() << endl;
    cout<<user.toString() << "\n";
}