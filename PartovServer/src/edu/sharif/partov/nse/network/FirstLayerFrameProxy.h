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

#ifndef FIRSTLAYERFRAMEPROXY_H_
#define FIRSTLAYERFRAMEPROXY_H_

#include "FirstLayerFrame.h"
#include "FrameProxy.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class ReferenceCounter;

/**
 * Name:              FirstLayerFrameProxy
 * Parent:            FirstLayerFrame, FrameProxy
 * Description:       This class is a proxy for first layer network frames. It delegates operations
 *                    to the actual implementation. This proxy is for providing the 'just copy on write'
 *                    feature for first layer frames (for preventing the unnecessary copy costs).
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class FirstLayerFrameProxy : public FirstLayerFrame, protected FrameProxy {
protected:
  FirstLayerFrame *imp;

public:
  FirstLayerFrameProxy (FirstLayerFrame *_imp);
  virtual ~FirstLayerFrameProxy ();

  virtual const quint8 *getSerializedData () const;
  virtual quint32 getSerializedDataLength () const;

  virtual void dumpFrame () const;

  virtual void setFrameRawDataAsInt8 (int index, quint8 data);
  virtual void setFrameRawDataAsInt16 (int index, quint16 data);
  virtual void setFrameRawDataAsInt32 (int index, quint32 data);
  virtual void setFrameRawDataAsInt64 (int index, quint64 data);
  virtual void setFrameRawDataAsStream (int index, const char *istream, const int length);

  virtual quint8 getFrameRawDataAsInt8 (int index) const;
  virtual quint16 getFrameRawDataAsInt16 (int index) const;
  virtual quint32 getFrameRawDataAsInt32 (int index) const;
  virtual quint64 getFrameRawDataAsInt64 (int index) const;
  virtual void getFrameRawDataAsStream (int index, char *ostream, const int length) const;

  virtual int getBodyLength () const;

  virtual quint16 calcIPHeaderChecksum (int offset, int length) const;

  virtual void writeFrameOnTheStream (QDataStream &stream) const throw (IOException *);

  virtual void finalize ();
  virtual FirstLayerFrameProxy *clone () const throw (NonCloneableException *);

  virtual void realFinalize ();

protected:
  virtual FrameProxy *getLowerLayerFrameProxy () const;
  virtual FirstLayerFrame *getImplementation () const;
  virtual void setNewImplementation (Frame *newImp);

public:
  virtual FrameProxy *me ();
  virtual Frame *toFrame ();

  Frame *analyze ();
};

}
}
}
}
}

#endif /* FIRSTLAYERFRAMEPROXY_H_ */
