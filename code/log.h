#ifndef __BigEula_LOG_H__
#define __BigEUla_lOG_H__
#include <stdint.h>
#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include "singleton.h"
#include "util.h"
#define BIGEULA_LOG_LEVEL(logger,level)\
      if(logger->getLevel()<=level)\
      BigEula::RaiiLogEvent(logger,level,__FILE__,__LINE__,0, BigEula::GetTheadId(), GetFiberId(),time(0),"111").getSS()

#define BIGEULA_LOG_DEBUG(logger)\
  BIGEULA_LOG_LEVEL(logger,BigEula::LogLevel::DEBUG)
#define BIGEULA_LOG_INFO(logger)\
BIGEULA_LOG_LEVEL(logger,BigEula::LogLevel::INfO)
#define BIGEULA_LOG_WARN(logger)\
BIGEULA_LOG_LEVEL(logger,BigEula::LogLevel::WARN)
#define BIGEULA_LOG_ERROR(logger)\
BIGEULA_LOG_LEVEL(logger,BigEula::LogLevel::ERROR)
#define BIGEULA_LOG_FATAL(logger)\
BIGEULA_LOG_LEVEL(logger,BigEula::LogLevel::FATAL)

#define BIGEULA_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel()<=level)\
      BigEula::RaiiLogEvent(logger,level,__FILE__,__LINE__,0, BigEula::GetTheadId(), GetFiberId(),time(0),"111").getLogEvent()->format(fmt, __VA_ARGS__)

#define BIGEULA_LOG_FMT_DEBUG(logger, fmt, ...) BIGEULA_LOG_FMT_LEVEL(logger, BigEula::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define BIGEULA_LOG_FMT_INFO(logger, fmt, ...) BIGEULA_LOG_FMT_LEVEL(logger, BigEula::LogLevel::INFO, fmt, __VA_ARGS__)
#define BIGEULA_LOG_FMT_WARN(logger, fmt, ...) BIGEULA_LOG_FMT_LEVEL(logger, BigEula::LogLevel::WARN, fmt, __VA_ARGS__)
#define BIGEULA_LOG_FMT_ERROR(logger, fmt, ...) BIGEULA_LOG_FMT_LEVEL(logger, BigEula::LogLevel::ERROR, fmt, __VA_ARGS__)
#define BIGEULA_LOG_FMT_FATAL(logger, fmt, ...) BIGEULA_LOG_FMT_LEVEL(logger, BigEula::LogLevel::FATAL, fmt, __VA_ARGS__)
#define BIGEULA_LOG_ROOT() BigEula::LoggerMgr::getInstance()->getRoot()
#define BIGEULA_LOG_NAME(name) sylar::LoggerMgr::getInstance()->getLogger(name)
namespace BigEula
{
  // //
  class Logger;
  class LogLevel
{
  public:
    enum Level
    {
        UNKNOW =0,
        DEBUG = 1,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    static const char* toString(LogLevel::Level level);
};
// 日志事件
class LogEvent
{
  public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent( std::shared_ptr<Logger> logger ,LogLevel::Level level,const char *file , 
    int32_t line,           
    uint32_t elapse,       
    uint32_t threadId,      
    uint32_t fiberId,       
    uint64_t time,              
    std::string threadName );
    const char*getFile()const {return m_file;};
    int32_t getLine()const {return m_line;};
     uint32_t getElapse() const{return m_elapse;};        
    uint32_t getThreadId() const {return m_threadId;};      
    uint32_t getFiberId ()const {return m_fiberId;};     
    uint64_t getTime()const {return m_time;};              
    std::string getContent()const{return m_content.str();};
    std::string getThreadName()const{return m_threadName;};
    std::stringstream& getSS(){return m_content;};
    LogLevel::Level getLevel()const{return m_level;};
    std::shared_ptr<Logger> getLogger()const{return m_logger;}
    void format(const char * fmt ,...);
  private:
    const char *m_file = "mian"; // 文件名
    int32_t m_line = 0;           // 行号
    uint32_t m_elapse = 0;        // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;      // 线程id
    uint32_t m_fiberId = 0;       // 携程id
    uint64_t m_time=0;              // 时间戳
    std::stringstream m_content;        //信息
    std::string m_threadName; //线程名字

  private:
  LogLevel::Level m_level;
  std::shared_ptr<Logger> m_logger;
};

class RaiiLogEvent
{
  public:
  RaiiLogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char *file , 
    int32_t line,           
    uint32_t elapse,       
    uint32_t threadId,      
    uint32_t fiberId,       
    uint64_t time,              
    std::string threadName);
  ~RaiiLogEvent();
  std::stringstream& getSS();
  LogEvent::ptr getLogEvent(){return m_logevent;}
  private:
  LogEvent::ptr m_logevent;
};

// 日志格式器
class LogFormatter
{
  public:
    using ptr = std::shared_ptr<LogFormatter>;

    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event);
    LogFormatter(const std::string&pattern);
  public:
  
  class FormatItem{
    public:
    using ptr = std::shared_ptr<FormatItem>;
    virtual ~FormatItem(){};
    virtual void format(std::stringstream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event)=0;
  };
  private:
  void init();
  std::string m_pattern;
  std::vector<FormatItem::ptr> m_items;
};

// 日志输出地
class LogAppender
{
  public:
    using ptr = std::shared_ptr<LogAppender>;
    virtual ~LogAppender(){};
    virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event)=0;
    void setFormatter(LogFormatter::ptr formatter){m_formatter=formatter;};
    LogFormatter::ptr getFormatter(){return m_formatter;};
    void setLevel(LogLevel::Level level){m_level=level;}
    LogLevel::Level getLevel()const{return m_level;}
  protected:
    LogLevel::Level m_level=LogLevel::UNKNOW;
    LogFormatter::ptr m_formatter;
};


// 日志器
class Logger:public std::enable_shared_from_this<Logger>
{
  public:
    using ptr = std::shared_ptr<Logger>;
    Logger(const std::string &name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    LogLevel::Level getLevel()const {return m_level;};
    void setLevel(LogLevel::Level level){m_level=level;};
    std::string getName(){return m_name;};
  private:
    std::string m_name;                    //日志名字
    LogLevel::Level m_level=LogLevel::UNKNOW;               //日志级别
    std::list<LogAppender::ptr> m_appender;//Appender集合
};

class StdoutLogAppender : public LogAppender
{
  public:
  using ptr = std::shared_ptr<StdoutLogAppender>;
  virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event)override;
};

class FileLogAppender : public LogAppender
{
  public:
  using ptr = std::shared_ptr<FileLogAppender>;
  FileLogAppender(const std::string&filename);
  ~FileLogAppender(){m_filestream.close();}
  virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event)override;
  bool reopen();
  private:
  std::string m_filename;
  std::ofstream m_filestream;
};
class LoggerManager {
public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);
    void init();
    Logger::ptr getRoot() const { return m_root;}
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

/// 日志器管理类单例模式
typedef BigEula::Singleton<LoggerManager> LoggerMgr;
} // namespace BigEula
#endif