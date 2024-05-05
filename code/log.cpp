#include <assert.h>
#include <functional>
#include <iostream>
#include <map>
#include "log.h"
namespace BigEula
{
    void LogEvent::format(const char * fmt ,...){
    va_list args;
    char *str;
    va_start(args, fmt);
    if (vasprintf(&str, fmt, args) < 0) {
        fprintf(stderr, "Error: vasprintf failed\n");
        va_end(args);
        return;
    }
    va_end(args);
    m_content<<str;
    free(str);
     }
    RaiiLogEvent::RaiiLogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char *file , 
    int32_t line,           
    uint32_t elapse,       
    uint32_t threadId,      
    uint32_t fiberId,       
    uint64_t time,              
    std::string threadName):m_logevent(std::make_shared<BigEula::LogEvent>(logger,level,file,line,elapse,threadId,fiberId,time,threadName)){}
  RaiiLogEvent::~RaiiLogEvent(){
    m_logevent->getLogger()->log(m_logevent->getLevel(),m_logevent);
  }
  std::stringstream& RaiiLogEvent::getSS(){return m_logevent->getSS();}
const char *LogLevel::toString(LogLevel::Level level)
{
        switch(level) {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}
Logger::Logger(const std::string &name) : m_name(name)
{
}
void Logger::log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level)
    {
        auto self = shared_from_this();
        for (auto &i : m_appender)
        {
            i->log(self, level, event);
        }
    }
}
void Logger::debug(LogEvent::ptr event)
{
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event)
{
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event)
{
    log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event)
{
    log(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::ptr event)
{
    log(LogLevel::FATAL, event);
}
void Logger::addAppender(LogAppender::ptr appender)
{
    m_appender.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender)
{
    for (auto it = m_appender.begin(); it != m_appender.end(); it++)
    {
        if (*it == appender)
        {
            m_appender.erase(it);
            break;
        }
    }
}
void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level&&m_formatter)
    {
        std::cout << m_formatter->format(logger, level, event);
    }
}
FileLogAppender::FileLogAppender(const std::string &filename) : m_filename(filename){reopen();};
void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level&&m_formatter)
    {
        //std::cout<<"ok"<<!!m_filestream<<std::endl;
         
        m_filestream<<m_formatter->format(logger, level, event);
    }
}
bool FileLogAppender::reopen()
{
    if (m_filestream)
    { 
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern)
{
    init();
}
std::string LogFormatter::format(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
{
    std::stringstream ss;
    for(auto& i : m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

class DateTimeFormatItem : public LogFormatter::FormatItem
{
  public:
    DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S") : m_format(format)
    {
        if (m_format.empty())
        {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }

  private:
    std::string m_format;
};
class FilenameFormatItem : public LogFormatter::FormatItem
{
  public:
    FilenameFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem
{
  public:
    LineFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem
{
  public:
    NewLineFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem
{
  public:
    StringFormatItem(const std::string &str) : m_string(str)
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << m_string;
    }

  private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem
{
  public:
    TabFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << "\t";
    }

  private:
};

class MessageFormatItem : public LogFormatter::FormatItem
{
  public:
    MessageFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem
{
  public:
    LevelFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << LogLevel::toString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem
{
  public:
    ElapseFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem
{
  public:
    NameFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << logger->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem
{
  public:
    ThreadIdFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem
{
  public:
    FiberIdFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem
{
  public:
    ThreadNameFormatItem(const std::string &str = "")
    {
    }
    void format(std::stringstream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getThreadName();
    }
};
void LogFormatter::init()
{
    std::vector<std::tuple<std::string, std::string, bool>> vec;
    for (size_t i = 0; i < m_pattern.size(); i++)
    {
        std::string first="", second="";
        if (m_pattern[i] == '%')
        {
            ++i;
            assert(i < m_pattern.size());
            first += m_pattern[i];
            if (m_pattern[i + 1] == '{')
            {
                ++i;
                ++i;
                while (m_pattern[i] != '}')
                {
                    second += m_pattern[i];
                    ++i;
                    assert(i < m_pattern.size());
                }
            }
            vec.push_back(std::make_tuple(first, second, true));
        }
        else
        {
            first += m_pattern[i];
            vec.push_back(std::make_tuple(first, second, false));
        }
    }
        static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(T, TabFormatItem),               //T:Tab
        XX(F, FiberIdFormatItem),           //F:协程id
        XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };
    //std::cout<<vec.size()<<std::endl;
    for (auto &i : vec)
    {
        // std::cout<<std::get<0>(i)<<" %2"<<std::get<1>(i)<<" %3"<<std::get<2>(i)<<std::endl;
        if (!std::get<2>(i))
        {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }
        else
        {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end())
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            }
            else
            {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
    }
}
    LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char *file , // 文件名
    int32_t line,           // 行号
    uint32_t elapse,        // 程序启动开始到现在的毫秒数
    uint32_t threadId,      // 线程id
    uint32_t fiberId,       // 携程id
    uint64_t time,              // 时间戳
    std::string threadName ):m_file(file),m_line(line),m_elapse(line),m_threadId(threadId),m_fiberId(fiberId),m_time(time),m_threadName(threadName),m_level(level),m_logger(logger){
    }

LoggerManager::LoggerManager() {
    m_root.reset(new Logger);
    auto temp=LogAppender::ptr(new StdoutLogAppender);
    temp->setFormatter(std::make_shared<LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    m_root->addAppender(temp);
    m_loggers[m_root->getName()] = m_root;

    init();
}
    void LoggerManager::init() {
}
}; // namespace BigEula