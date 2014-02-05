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

#ifndef PLUGIN_NOT_INSTALLED_EXCEPTION_H_
#define PLUGIN_NOT_INSTALLED_EXCEPTION_H_

#include "edu/sharif/partov/exception/Exception.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

/**
 * Name:              PluginNotInstalledException
 * Parent:            Exception
 * Description:       Indicate that no plugin with given name is installed.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              Exception Class;
 */
class PluginNotInstalledException : public edu::sharif::partov::exception::Exception {
Q_OBJECT

public:
  inline PluginNotInstalledException (const QString & mesg) :
      Exception (mesg) {
  }
};

}
}
}
}
}

#endif /* PLUGIN_NOT_INSTALLED_EXCEPTION_H_ */
