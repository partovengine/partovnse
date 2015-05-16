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

#ifndef FRAME_H_
#define FRAME_H_

#include "SharedObject.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class ReferenceCounter;

/**
 * Name:              Frame
 * Parent:            SharedObject
 * Description:       Parent of all network frames.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
class Frame : public SharedObject {
private:
  ReferenceCounter *refCounter;

protected:
  int startOfBody;
  Frame *upperLayerFrame;

public:
  Frame (Frame *_lowerLayerFrame, int _startOfBody, ReferenceCounter *_refCounter);
  virtual ~Frame ();

  /**
   * Analyze this frame object and return it in its most concrete known form.
   * If we know about any more concrete form, then at least upperLayerFrame field
   * is subject to change. So this method can not be declared as constant.
   */
  virtual Frame *analyze ();

  virtual void finalize ();

  virtual void unrefStarted ();
  virtual void unrefIsExiting ();
  virtual void atomicDetachExtensionSlot ();

  ReferenceCounter *getReferenceCounter () const;
  virtual Frame *getLowerLayerFrame () const;
  virtual Frame *getUpperLayerFrame () const;

  /**
   * Each frame is created in 'populated' state and go to 'modified' state
   * by any modification. The 'populateToRawFrame' operation will apply
   * all of modifications and return frame to the 'populated' state.
   * The frame and its lower layer frames and its upper layer frames will
   * make 'chain of frames'. The chain of frames is in the populated
   * state iff all of its frames be so.
   */
  virtual void populateToRawFrame ();

  virtual int getStartOfBody () const;
  virtual int getBodyLength () const = 0;

  /**
   * Set the 'frame' as lower layer frame of this frame if possible. If this
   * frame can not have 'frame' as its lower layer frame, simply ignore it.
   *
   * Pre-Condition:
   * The 'frame' must have no upper layer frame.
   */
  virtual void setLowerLayerFrame (Frame *frame);

  /**
   * Pre-Condition:
   * The 'chain of frames' containing this frame instance, must be in
   * the 'populated' state.
   */
  virtual Frame *clone () const throw (NonCloneableException *) = 0;
};

}
}
}
}
}

#endif /* FRAME_H_ */
