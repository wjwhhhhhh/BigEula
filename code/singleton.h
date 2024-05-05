#ifndef __BIGEULA_SINGLETON_H
#define __BIGEULA_SINGLETON_H
#include <memory>
namespace BigEula
{
    template<class T,class X=void,int N=0>
    class Singleton
    {
        public:
        static T*getSingleton()
        {
            static T v;
            return &v;
        }
    };
    template<class T,class X=void,int N=0>
    class SingletonPtr
    {
        public:
        static std::shared_ptr<T>getSingletonPtr()
        {
            static std::shared_ptr<T> v(new T);
            return v;
        }
    };
}
#endif 