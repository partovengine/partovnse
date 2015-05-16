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

#ifndef MESSAGE_LOGGER_H_
#define MESSAGE_LOGGER_H_

#include <Qt>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

/**
 * Name:              MessageLogger
 * Parent:            None
 * Description:       The logger class which can log messages written by qDebug and so on.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.util
 * Tags:              None
 */
class MessageLogger {
private:
  static MessageLogger *me;

  static const char *NORMAL_COLOR;
  static const char *DEBUG_COLOR;
  static const char *WARNING_COLOR;
  static const char *CRITICAL_COLOR;
  static const char *FATAL_COLOR;
  static const char *ADDRESS_COLOR;

  MessageLogger ();

public:
  ~MessageLogger ();

  static MessageLogger *getInstance ();

  void log (QtMsgType, const QMessageLogContext &, const QString &);
};

void myMessageHandler (QtMsgType type,
    const QMessageLogContext &context, const QString &message);

}
}
}
}
}

#endif /* MESSAGE_LOGGER_H_ */
