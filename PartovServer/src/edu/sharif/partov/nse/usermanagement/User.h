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

#ifndef USER_H_
#define USER_H_

#include <QString>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace usermanagement {

class Group;

/**
 * Name:              User
 * Parent:            None
 * Description:       The abstraction of the user which is used for authenticating process.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.usermanagement
 * Tags:              None
 */
class User {
private:
  QString userName;
  QString groupName;

public:
  User ();
  User (QString userName, QString groupName);
  virtual ~User ();

  bool isNull () const;
  bool operator== (const User &user) const;
  bool isInGroup (const Group &group) const;

  QString getUserName () const;

  /**
   * This method MUST RUN in the main thread.
   */
  static User authenticateUser (QString userName, QString password);
};

}
}
}
}
}

#endif /* USER_H_ */
