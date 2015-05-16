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

#ifndef REFERENCECOUNTER_H_
#define REFERENCECOUNTER_H_

class QMutex;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SharedObject;

/**
 * Name:              ReferenceCounter
 * Parent:            None
 * Description:       This class is for counting references of SharedObject instances.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Final;
 */
class ReferenceCounter {
private:
  QMutex *mutex;
  int countOfReferences;

public:
  ReferenceCounter ();
  ~ReferenceCounter ();

  void acquireLock ();
  void releaseLock ();

  void ref (int count = 1);
  void unref (SharedObject *obj);

  SharedObject *detach (SharedObject *obj);

  int getCountOfReferences () const;

private:
  bool realUnref (SharedObject *obj);
};

}
}
}
}
}

#endif /* REFERENCECOUNTER_H_ */
