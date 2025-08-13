//#include "header/Log.h"
//#include <thread>
//#include <HashContainer.h>
//#include<PrimeNumberConvert.h>
//#include <vector>
//#include <random>
//#include<numeric>
//#include <gmp.h>
//#include <unordered_map>
//#include<iostream>
//using namespace liao::message;
//using namespace liao::primed::hash;
//using namespace liao::primed::prime;
//using namespace std;
//
//#define EXTIME 20000000
//std::random_device rd;
//std::uniform_int_distribution<int> dist(1, numeric_limits<int>::max());
//static unordered_map<string, int> primeMap;
//static mutex mapLock;
//static int total = 0;
//static int conflicts = 0;
//static Log& logRef = Log::Get();
//static std::mt19937 gen(rd());
//static mpz_class maxDistance = 0;
//static const int digits = 8;
//static const string fileName = to_string(digits) + "_" + "conflicts.txt";
//void test(int start, int end)
//{
//	for (int n = start; n < end; ++n)
//	{
//		int number = dist(gen);
//		mpz_class result(number);
//		PrimeNumberConvert::generate_big(result, digits);
//		{
//			std::lock_guard<std::mutex> lock(mapLock);
//			if (primeMap.contains(result.get_str(10)) && primeMap[result.get_str(10)] != number)
//			{
//				logRef[LogType::Error].openToFile(fileName) << to_string(total)
//					<< result.get_str(10) << to_string(primeMap[result.get_str(10)]) << to_string(number) << Log::LogEndl;
//				++conflicts;
//			}
//			//cout << "Number: " << number << " Result: " << result.get_str(10) << endl;
//			primeMap[result.get_str(10)] = number;
//			total++;
//		}
//	}
//}
//void c_distance(unsigned int lowerLimit, unsigned int upperLimit)
//{
//	mpz_class lastPrime = 0;
//	mpz_class result = lowerLimit;
//	for (int n = lowerLimit; n <= upperLimit; n = result.get_si())
//	{
//		result = n;
//		mpz_nextprime(result.get_mpz_t(), result.get_mpz_t());
//		if (lastPrime == 0)
//		{
//			lastPrime = result.get_si();
//			continue;
//		}
//		{
//			std::lock_guard<std::mutex> lock(mapLock);
//			if (maxDistance < result - lastPrime)
//			{
//				maxDistance = result - lastPrime;
//			}
//		}
//		lastPrime = result.get_si();
//	}
//}
//const int va = 100000000;
//int main()
//{
//	liao::primed::hash::HashContainer hashGen;
//	thread t1(c_distance, 0, va);
//	thread t2(c_distance, va, 2 * va);
//	thread t3(c_distance, 2 * va, 3 * va);
//	thread t4(c_distance, 3 * va, 4 * va);
//	thread t5(c_distance, 4 * va, 5 * va);
//	thread t6(c_distance, 5 * va, 6 * va);
//	thread t7(c_distance, 6 * va, 7 * va);
//	thread t8(c_distance, 7 * va, 8 * va);
//	thread t9(c_distance, 8 * va, 9 * va);
//	thread t10(c_distance, 9 * va, 10 * va);
//	t1.join();
//	t2.join();
//	t3.join();
//	t4.join();
//	t5.join();
//	t6.join();
//	t7.join();
//	t8.join();
//	t9.join();
//	t10.join();
//	cout << maxDistance;
//	//thread t1(test, 0, EXTIME);
//	//thread t2(test, 0, EXTIME);
//	//thread t3(test, 0, EXTIME);
//	//thread t4(test, 0, EXTIME);
//	//thread t5(test, 0, EXTIME);
//	//thread t6(test, 0, EXTIME);
//	//thread t7(test, 0, EXTIME);
//	//t1.join();
//	//t2.join();
//	//t3.join();
//	//t4.join();
//	//t5.join();
//	//t6.join();
//	//t7.join();
//	/*hashGen.generate("Liao Runkang");
//	vector<unsigned int> value = hashGen.getHashNumbers();
//	PrimeNumberConvert::generate(value, digits);
//	unsigned int total = 1;
//	for(auto& v : value)
//	{
//		total *= v;
//	}
//	cout << total<<endl;
//	hashGen.generate("4.5");
//	vector<unsigned int> value2 = hashGen.getHashNumbers();
//	total = 1;
//	for(auto& v : value2)
//	{
//		if(v == 0)
//			continue;
//		total *= v;
//	}
//	cout << total << endl;
//	PrimeNumberConvert::generate(value2, digits);
//	cout<<PrimeNumberConvert::generate(123456, digits) << endl;*/
//	//for (int n = 0; n < 100; ++n)
//	//{
//	//	long long number = dist(gen);
//	//	mpz_class result(to_string(number));
//	//	PrimeNumberConvert::generate_big(result, digits);
//	//	mpz_class candidate,temp(to_string(number));
//	//	mpz_ui_pow_ui(candidate.get_mpz_t(), 3, digits);
//	//	candidate = result / candidate;
//	//	cout << "Number: " << number << " Hash: " << result.get_str(10) << " Decode: " << candidate.get_str(10)<<"  " <<  (candidate == temp) << endl;
//	//}
//}
//
