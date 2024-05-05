#include <boost/lexical_cast.hpp>
#include <string>
int main()
{
    double a=3.23;
    std::string b=boost::lexical_cast<std::string>(a);
}