//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *  
 * Copyright © 2009-2014 Behnam Momeni.
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

#include "User.h"

#include "Group.h"
#include "Database.h"

#include <QMetaObject>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace usermanagement {

User::User () {
}

User::User (QString _userName, QString _groupName) :
userName (_userName), groupName (_groupName) {
}

User::~User () {
}

bool User::isNull () const {
  return userName.isNull ();
}

bool User::operator== (const User &user) const {
  return userName == user.userName;
}

bool User::isInGroup (const Group &group) const {
  return groupName == group.getName ();
}

QString User::getUserName () const {
  return userName;
}

User User::authenticateUser (QString userName, QString password) {
  return Database::getInstance ()->authenticateUser (userName, password);
}

}
}
}
}
}
