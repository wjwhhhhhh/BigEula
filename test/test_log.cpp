#include<iostream>
#include <memory>
#include "../code/log.h"
#include "../code/util.h"
using namespace BigEula;
int main()
{
 LogFormatter::ptr formatt=std::make_shared<LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
  StdoutLogAppender::ptr LogAppender=std::make_shared<StdoutLogAppender>();
  FileLogAppender::ptr fileappender=std::make_shared<FileLogAppender>("./log.txt");
  //Logger::ptr log=std::make_shared<Logger>();
  Logger::ptr log=LoggerMgr::getSingleton()->getRoot();
  //#define log LoggerMgr::getSingletonPtr()->getRoot()
  fileappender->setLevel(LogLevel::ERROR);
    log->addAppender(LogAppender);
   log->addAppender(fileappender);
   fileappender->setFormatter(formatt);
   LogAppender->setFormatter(formatt);
   BIGEULA_LOG_FMT_DEBUG(log,"%s","hello world");
   BIGEULA_LOG_FMT_ERROR(log,"%s","hello world");
   BIGEULA_LOG_FMT_DEBUG(log,"%s","hello world");
   BIGEULA_LOG_FMT_ERROR(log,"%s","hello world");
   //#undef log
}