#include"util.h"
namespace BigEula
{
    pthread_t GetTheadId()
    {
         return pthread_self();
    }
    __uint32_t GetFiberId()
    {
        return 0;
    }
}