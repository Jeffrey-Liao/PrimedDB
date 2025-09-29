#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <format>
#include <fstream>
#include <memory>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <random>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <format>
#include <fstream>
#include "sha.h"
#include <string_view>

namespace  liao
{
#define DYNAMIC template<class T>
#define USESTD using namespace std
#define DYNAMICCON(conce) template<conce T>
#define USECRPT using namespace CryptoPP
#define USELIAO using namespace liao
#define USELIAOUTIL USELIAO::Util
#define USELIAOPRIMED USELIAO::PrimedDB
#define USELIAOMATH USELIAO::Math
	using Lock = std::unique_lock<std::mutex>;
	using ShareMutex = std::shared_mutex;
	using Mutex = std::mutex;
	using SimpleLock = std::lock_guard<std::mutex>;
	using WriteLock = std::unique_lock<ShareMutex>;
	using ReadLock = std::shared_lock<ShareMutex>;
	using RecurMutex = std::recursive_mutex;
	using UCharPtr = std::unique_ptr<char>;
	using SCharPtr = std::shared_ptr<char>;
}
