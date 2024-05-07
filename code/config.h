#ifndef __BIGEULA__CONFIG__H__
#define __BIGEULA__CONFIG__H__
#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <map>
#include "util.h"
#include "log.h"
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
template<class T>
class ConfigVar:public ConfigVarBase
{
    public:
    using ptr=std::shared_ptr<ConfigVar>;
    ConfigVar(const std::string &name,const T& value,const std::string &description):ConfigVarBase(name,description),m_value(value){}
    std::string toString()override{
        try{
            return boost::lexical_cast<std::string>(m_value);
        }catch(std::exception &e){
            BIGEULA_LOG_ERROR(BIGEULA_LOG_ROOT())<<"ConfigVar::toString exception"<<e.what()<<"convert:"<<TypeToName<T>()<<"to string";
        }
        return "";
    }
    bool fromString(const std::string &val)override{
        try{
            m_value=boost::lexical_cast<T>(val);
            return true;
        }catch(std::exception &e){
            BIGEULA_LOG_ERROR(BIGEULA_LOG_ROOT())<<"ConfigVar::formString exception"<<e.what()<<"convert:"<<TypeToName<T>()<<" form string ";
        }
        return false;
    }
    std::string getTypeName()const override{
        return TypeToName<T>();
    }
    T&getValue(){return m_value;}
    void setValue(const T&v){m_value=v;}
    private:
    T m_value;
};
class Config{
    public:
    using ConfigVarMap=std::map<std::string,ConfigVarBase::ptr>;
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = "") {
        auto it = GetDatas().find(name);
        if(it != GetDatas().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if(tmp) {
                BIGEULA_LOG_INFO(BIGEULA_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            } else {
                BIGEULA_LOG_INFO(BIGEULA_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
                        << TypeToName<T>() << " real_type=" << it->second->getTypeName()
                        << " " << it->second->toString();
                return nullptr;
            }
        }

        if(name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
                != std::string::npos) {
            BIGEULA_LOG_ERROR(BIGEULA_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        GetDatas()[name] = v;
        return v;
    }
        template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }
    private:
    static ConfigVarMap& GetDatas() {
    static ConfigVarMap s_datas;
    return s_datas;
    }
};
}
#endif