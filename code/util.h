#ifndef BIGEULA_UTIL_H
#define BIGEULA_UTIL_H
#include <pthread.h>
namespace BigEula
{
    pthread_t GetTheadId();
    __uint32_t GetFiberId();
    template<class T>
    const char* TypeToName() {
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}
};
#endif