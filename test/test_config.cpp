#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include "../code/config.h"
#include <memory>

int main()
{
    {
        using namespace BigEula;
        Config::Lookup<int>("port",8080,"port");
        std::cout<<Config::Lookup<int>("port")->toString();
    }
}