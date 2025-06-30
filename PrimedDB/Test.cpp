#include "header/Log.h"
#include <thread>

#include <chrono>

using namespace liao::message;
using namespace std;

void primed(string& message)
{

    hash_obj.Update((const CryptoPP::byte*)message.data(), message.size());
    hash_obj.Final(result);
    // 将字节流转换为大整数
    CryptoPP::Integer x(result, CryptoPP::SHA256::DIGESTSIZE);
    
    //std::cout << result <<"\n";
    //std::cout << "SHA-256 as BigInteger: " << x << "\n";
    //cout << endl;
}
void compare(string& message)
{
    int n = message.size();
    //cout << message << endl;
}
int main()
{	

    string m;
    for (int n = 0; n < EXTIME; ++n)
    {
        m = to_string(n);
        primed(m);
    }
}
