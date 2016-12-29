#ifndef HL_LOGGER_H
#define HL_LOGGER_H

#include <string>
#include <list>

#include "defines.h"
#include "logevent.h"
#include "loglevel.h"
#include "util/mutex.h"
#include "util/spinlock.h"

HL_NAMESPACE_BEGIN

class Appender;

class HUMBLE_EXPORT_API Logger
{
public:
  Logger(const std::string &name, int level = LogLevel::Off);
  ~Logger();

  Logger& setName(const std::string &name);
  const std::string& getName() const;

  Logger& setLogLevel(int level);
  int getLogLevel() const;

  Logger& addAppender(Appender *appender);
  std::list<Appender*> getAppenders() const;
  bool hasAppender(Appender *appender);

  bool wouldLog(int level) const;
  Logger& log(const LogEvent &logEvent);

private:
  mutable SpinLock _mutex;

  /*
    Name of this logger
  */
  std::string _name;

  /*
    Log level of this logger
  */
  int _level;

  /*
    List of all appenders, which are used by the current Logger object.
  */
  std::list<Appender*> _appenders;
};

HL_NAMESPACE_END
#endif
