//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *  
 * Copyright © 2009-2015 Behnam Momeni.
 * 
 * This file is part of the Partov.
 * 
 * Partov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Partov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Partov.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */

#include "MessageLogger.h"

#include <QDateTime>

#include <iostream>

using namespace std;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

MessageLogger *MessageLogger::me = 0;

const char *MessageLogger::NORMAL_COLOR = "\033[0m";
const char *MessageLogger::DEBUG_COLOR = "\033[0;32m"; // GREEN
const char *MessageLogger::INFO_COLOR = "\033[0;36m"; // CYAN
const char *MessageLogger::WARNING_COLOR = "\033[1;33m"; // YELLOW
const char *MessageLogger::CRITICAL_COLOR = "\033[1;31m"; // RED
const char *MessageLogger::FATAL_COLOR = "\033[1;35m"; // MAGNET
const char *MessageLogger::ADDRESS_COLOR = "\033[1;34m"; // BLUE

MessageLogger::MessageLogger () {
}

MessageLogger::~MessageLogger () {
}

MessageLogger *MessageLogger::getInstance () {
  if (!MessageLogger::me) {
    MessageLogger::me = new MessageLogger ();
  }
  return MessageLogger::me;
}

void MessageLogger::log (QtMsgType type,
        const QMessageLogContext &context, const QString &message) {
  QDateTime now = QDateTime::currentDateTime ();
  const char *color;
  const char *typestr;
  switch (type) {
  case QtDebugMsg:
    color = MessageLogger::DEBUG_COLOR;
    typestr = " [DEBUG] ";
    break;
  case QtInfoMsg:
    color = MessageLogger::INFO_COLOR;
    typestr = " [INFO] ";
    break;
  case QtWarningMsg:
    color = MessageLogger::WARNING_COLOR;
    typestr = " [WARNING] ";
    break;
  case QtCriticalMsg:
    color = MessageLogger::CRITICAL_COLOR;
    typestr = " [CRITICAL] ";
    break;
  case QtFatalMsg:
    color = MessageLogger::FATAL_COLOR;
    typestr = " [FATAL] ";
    break;
  };
  cerr << MessageLogger::NORMAL_COLOR << qPrintable (now.toString ())
      << color << typestr
      << MessageLogger::ADDRESS_COLOR << context.function
      << MessageLogger::NORMAL_COLOR << " {"
      << MessageLogger::ADDRESS_COLOR << "line " << context.line
      << MessageLogger::NORMAL_COLOR << "} "
      << color << qPrintable (message) << MessageLogger::NORMAL_COLOR << endl;
}

void myMessageHandler (QtMsgType type,
    const QMessageLogContext &context, const QString &message) {
  MessageLogger::getInstance ()->log (type, context, message);
}

}
}
}
}
}
