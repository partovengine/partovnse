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

// Uncomment if the project has compile errors, complaining about nonexisting pcap functions
//#define USE_PRE_ONE_PCAP_API

#include "EthernetDirectAccessDriver.h"

#include "NetworkAdapterInfo.h"
#include "NoActiveNetworkAdapterException.h"

#include "edu/sharif/partov/nse/network/EthernetFrame.h"
#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include <QHostAddress>
#include <QMutex>
#include <QWaitCondition>
#include <QSettings>

#include <iostream>
#include <string.h>
#include <pcap/pcap.h>

#ifdef Q_WS_WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace emulation {

EthernetDirectAccessDriver *EthernetDirectAccessDriver::me;

EthernetDirectAccessDriver::EthernetDirectAccessDriver () {
  mutex = new QMutex ();
  handlesCount = 0;
  handleReAcquired = new QWaitCondition ();
}

EthernetDirectAccessDriver::~EthernetDirectAccessDriver () {
  delete mutex;
  delete handleReAcquired;
}

void EthernetDirectAccessDriver::postInit () throw (edu::sharif::partov::nse::network::IOException *) {
  QSettings config ("../config/config.ini", QSettings::IniFormat);
  config.beginGroup ("driver");
  bool ok = true;
  libpcapReadTimeout = config.value ("libpcap-read-timeout", 1).toInt (&ok);
  if (!ok) {
    destroy ();
    throw new edu::sharif::partov::nse::network::IOException
        ("The driver/libpcap-read-timeout parameter is not configured correctly");
  }
  libpcapImmediateMode = config.value ("libpcap-immediate-mode", true).toBool ();
  config.endGroup ();
  config.beginGroup ("interfaces");
  int size = config.beginReadArray ("iface");
  std::cout << "Count of interfaces to be configured is " << size << "\n";
  for (int i = 0; i < size; ++i) {
    config.setArrayIndex (i);
    QString name = config.value ("name").toString ();
    QHostAddress gatewayIp (config.value ("gateway-ip").toString ());
    if (gatewayIp.isNull ()) {
      destroy ();
      throw new edu::sharif::partov::nse::network::IOException
          ("Gateway IP is not configured correctly");
    }
    QHostAddress routerIp (config.value ("router-ip").toString ());
    if (routerIp.isNull ()) {
      destroy ();
      throw new edu::sharif::partov::nse::network::IOException
          ("Router IP is not configured correctly");
    }
    std::cout << "A router will be installed on interface " << qPrintable (name) << ".\n";
    bool shouldFilterTraffic = config.contains ("filter");
    QString filterRule = config.value ("filter", "No rule configured.").toString ();
    std::cout << "Filter rule: " << qPrintable (filterRule) << "\n";
    NetworkAdapterInfo *nai = new NetworkAdapterInfo
        (name, gatewayIp, routerIp, shouldFilterTraffic, qPrintable (filterRule));
    adapters << nai;
    if (!nai->isValid ()) {
      // aborting....
      destroy ();
      throw new edu::sharif::partov::nse::network::IOException
          ("Can not instantiate network adapter;"
           "Check the filtering rule (if any) and the libpcap installation.");
    }
  }
  config.endArray ();
  config.endGroup ();
}

NetworkAdapterInfo *EthernetDirectAccessDriver::getNetworkAdapterInfo (
    const QString &deviceName) throw (NoActiveNetworkAdapterException *) {

  foreach (NetworkAdapterInfo *adapter, adapters) {
    if (adapter->getName () == deviceName) {
      return adapter;
    }
  }
  throw new NoActiveNetworkAdapterException (deviceName);
}

void EthernetDirectAccessDriver::close (NetworkAdapter handle) {
  QMutexLocker locker (mutex);
  pcap_close (handle);
  handlesCount--;
  handleReAcquired->wakeAll ();
}

void EthernetDirectAccessDriver::send (NetworkAdapter handle,
    edu::sharif::partov::nse::network::SecondLayerFrame *frame)
throw (edu::sharif::partov::nse::network::IOException *) {
  edu::sharif::partov::nse::network::FirstLayerFrame *l1f = frame->getLowerLayerFrame ();
  if (pcap_sendpacket (handle, l1f->getSerializedData (),
                       l1f->getSerializedDataLength ())) {
    QString mesg = "Error sending packet: %1";
    throw new edu::sharif::partov::nse::network::IOException (mesg.arg (pcap_geterr (handle)));
  }
}

edu::sharif::partov::nse::network::SecondLayerFrame *EthernetDirectAccessDriver::next (
    NetworkAdapter handle) throw (edu::sharif::partov::nse::network::IOException *) {
  struct pcap_pkthdr *header;
  const u_char *data;

  int res = pcap_next_ex (handle, &header, &data);
  if (res < 0) {
    QString mesg = "Error reading the packets: %1";
    throw new edu::sharif::partov::nse::network::IOException (mesg.arg (pcap_geterr (handle)));
  } else if (res == 0) {
    // timeout !!
    return NULL;
  }
  if (header->caplen < edu::sharif::partov::nse::network::EthernetFrame::MIN_FRAME_SIZE
      || header->caplen > edu::sharif::partov::nse::network::EthernetFrame::MAX_FRAME_SIZE) {
    return NULL;
  }
  quint8 *buffer = new quint8[header->caplen];
  memcpy (buffer, data, header->caplen);

  edu::sharif::partov::nse::network::SecondLayerFrame *sf =
      edu::sharif::partov::nse::network::FrameFactory::getInstance ()
      ->createSecondLayerFrame (buffer, header->caplen);

  return sf;
}

struct PacketProcessingInfo {

  void *object;
  EthernetDirectAccessDriver::CallbackFunction function;
};

void process_packet (u_char *aux, const struct pcap_pkthdr *header,
    const u_char *data);

void EthernetDirectAccessDriver::startProcessingPackets (NetworkAdapter handle,
    void *object, CallbackFunction function) {
  PacketProcessingInfo info = {object, function};
  int ret = pcap_loop
      (handle,
       -1, // indicates count of packets | -1 means read packets indefinitely
       process_packet,
       (u_char *) & info);
  if (ret == -1) {
    QString mesg = "Error reading the packets: %1";
    throw new edu::sharif::partov::nse::network::IOException (mesg.arg (pcap_geterr (handle)));
  } else if (ret == -2) {
    // pcap_breakloop is called
    return;
  }
  throw new edu::sharif::partov::nse::network::IOException
      ("Unreachable code is executed! Check for possible changes in libpcap behavior");
}

void process_packet (u_char *aux, const struct pcap_pkthdr *header,
    const u_char *data) {
  PacketProcessingInfo *info = (PacketProcessingInfo *) aux;

  if (header->caplen < edu::sharif::partov::nse::network::EthernetFrame::MIN_FRAME_SIZE
      || header->caplen > edu::sharif::partov::nse::network::EthernetFrame::MAX_FRAME_SIZE) {
    return;
  }
  quint8 *buffer = new quint8[header->caplen];
  memcpy (buffer, data, header->caplen);

  edu::sharif::partov::nse::network::SecondLayerFrame *sf =
      edu::sharif::partov::nse::network::FrameFactory::getInstance ()
      ->createSecondLayerFrame (buffer, header->caplen);
  info->function (info->object, sf);
}

void EthernetDirectAccessDriver::stopProcessingPackets (NetworkAdapter handle) {
  pcap_breakloop (handle);
}

NetworkAdapter EthernetDirectAccessDriver::open (const QString &deviceName)
throw (edu::sharif::partov::nse::network::IOException *) {
  return open (qPrintable (deviceName));
}

NetworkAdapter EthernetDirectAccessDriver::open (const char *deviceName)
throw (edu::sharif::partov::nse::network::IOException *) {
  QMutexLocker locker (mutex);
  char errorBuffer[1000];
  NetworkAdapter handle;
#ifdef USE_PRE_ONE_PCAP_API
  // Old code for versions before 1.0 (when immediate mode was on by default)
  if ((handle = pcap_open_live (deviceName, // name of the device
                                65536, // portion of the packet to capture.
                                1, // promiscuous mode (nonzero means promiscuous)
                                libpcapReadTimeout, // read timeout
                                errorBuffer // error buffer
                                )) == NULL) {
    QString mesg = "Unable to open the adapter: %1 is not supported by this driver: %2";
    throw new edu::sharif::partov::nse::network::IOException
        (mesg.arg (deviceName).arg (errorBuffer));
  }
  handlesCount++;
  return handle;
#else
  handle = pcap_create (deviceName, errorBuffer);
  if (handle == NULL) {
    QString mesg = "Unable to open the adapter: %1 is not supported by this driver: %2";
    throw new edu::sharif::partov::nse::network::IOException
        (mesg.arg (deviceName).arg (errorBuffer));
  }
  if (// The 65536 guarantees full packet capture on all MACs.
      (pcap_set_snaplen (handle, 65536) == 0)
      && (pcap_set_timeout (handle, libpcapReadTimeout) == 0)
      // enables promiscuous mode
      && (pcap_set_promisc (handle, 1) == 0)
      // immediate mode causes received packets to be processes immediately
      && (pcap_set_immediate_mode (handle, (libpcapImmediateMode ? 1 : 0)) == 0)
      && (pcap_activate (handle) == 0)) {
    handlesCount++;
    return handle;
  }
  QString mesg = "Unable to activate the adapter: %1 is not supported by this driver: %2";
  throw new edu::sharif::partov::nse::network::IOException
      (mesg.arg (deviceName).arg (pcap_geterr (handle)));
#endif
}

bool EthernetDirectAccessDriver::init () {
  try {
    EthernetDirectAccessDriver::me = new EthernetDirectAccessDriver ();
    EthernetDirectAccessDriver::me->postInit ();

  } catch (edu::sharif::partov::nse::network::IOException *e) {
    e->printExceptionDescription ();
    return false;
  }
  return true;
}

EthernetDirectAccessDriver *EthernetDirectAccessDriver::getInstance () {
  return EthernetDirectAccessDriver::me;
}

void EthernetDirectAccessDriver::destroy () {
  while (!adapters.isEmpty ()) {
    delete adapters.takeFirst ();
  }
  {
    QMutexLocker locker (mutex);
    while (handlesCount > 0) {
      // we must wait for the released handle to be returned to us (acquired)
      handleReAcquired->wait (mutex);
    }
  }
  delete this;
  EthernetDirectAccessDriver::me = NULL;
}

}
}
}
}
}
