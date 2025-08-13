#pragma once
#include <memory>
#include <unordered_map>
#include <thread>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <random>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <format> 
#include "sha.h"
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