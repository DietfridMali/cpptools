#pragma once
#include <mutex>

// =================================================================================================

template <typename T>
class BaseSingleton
{
public:
    BaseSingleton(const BaseSingleton&) = delete;
    BaseSingleton& operator=(const BaseSingleton&) = delete;

    static T& Instance()
    {
        static std::once_flag flag;
        static T* instance = nullptr;
        std::call_once(flag, []() { instance = new T(); });
        return *instance;
    }

protected:
    BaseSingleton() = default;
    virtual ~BaseSingleton() = default;
};

// =================================================================================================

class PolymorphSingleton
{
public:
    PolymorphSingleton(const PolymorphSingleton&) = delete;
    PolymorphSingleton& operator=(const PolymorphSingleton&) = delete;
    virtual ~PolymorphSingleton() = default;

    // Zugriff auf die aktuelle Instanz
    static PolymorphSingleton& SingletonInstance()
    {
        if (!_instance) throw std::runtime_error("PolymorphSingleton::Instance() not initialized!");
        return *_instance;
    }

protected:
    PolymorphSingleton() { _instance = this; }
    static PolymorphSingleton* _instance;
};

// =================================================================================================
