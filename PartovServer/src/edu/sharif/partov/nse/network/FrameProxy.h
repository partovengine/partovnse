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

#ifndef FRAMEPROXY_H_
#define FRAMEPROXY_H_

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class Frame;

/**
 * Name:              FrameProxy
 * Parent:            None
 * Description:       Parent of all network frame proxy objects.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
class FrameProxy {
protected:
  FrameProxy *mostUpperLayerFrame;

protected:
  FrameProxy (FrameProxy *lowerLayerFrame);
  virtual ~FrameProxy ();

  void detach ();

  /**
   * @return true iff the frame proxy, itself and not its lower layer frames, performs the real copy.
   */
  bool realDetach ();

  virtual FrameProxy *getLowerLayerFrameProxy () const = 0;
  virtual Frame *getImplementation () const = 0;
  virtual void setNewImplementation (Frame *newImp) = 0;

public:
  virtual void realFinalize () = 0;
  virtual FrameProxy *me () = 0;
  virtual Frame *toFrame () = 0;

private:
  // FIXME: ReThink about protected inheritance of frame proxy. Shouldn't it be publicly inherited indeed?
};

}
}
}
}
}

#endif /* FRAMEPROXY_H_ */
