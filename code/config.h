#ifndef __BIGEULA__CONFIG__H__
#define __BIGEULA__CONFIG__H__
#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
namespace BigEula
{
    class ConfigVarBase {
public:
    using ptr= std::shared_ptr<ConfigVarBase>; 
    ConfigVarBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description) {
            //就是把m_name转变为小写
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    virtual ~ConfigVarBase() {}
    const std::string& getName() const { return m_name;}
    const std::string& getDescription() const { return m_description;}
    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
    virtual std::string getTypeName() const = 0;
protected:
    std::string m_name;
    std::string m_description;
};
}
#endif