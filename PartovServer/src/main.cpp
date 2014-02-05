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

#include <QCoreApplication>
#include <QTimer>

#include <signal.h>
#include <iostream>

#include "edu/sharif/partov/nse/server/Server.h"

#include "edu/sharif/partov/nse/map/Map.h"
#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/network/transceiver/TransceiverFactory.h"

#include "edu/sharif/partov/nse/plugin/PluginNodeFactory.h"

#include "edu/sharif/partov/nse/emulation/EthernetDirectAccessDriver.h"
#include "edu/sharif/partov/nse/network/EthernetIPFrameFactory.h"

#include "edu/sharif/partov/nse/usermanagement/Database.h"
#include "edu/sharif/partov/nse/usermanagement/User.h"

#include "edu/sharif/partov/nse/util/PacketLoggerFactory.h"
#include "edu/sharif/partov/nse/util/SignalHandler.h"

using namespace std;

int startPartovServer (QCoreApplication &app);
void printShortIntro ();

int main (int argc, char *argv[]) {
  QCoreApplication app (argc, argv);

  printShortIntro ();

  edu::sharif::partov::nse::server::Server::loadGlobalSettings ();

  if (!edu::sharif::partov::nse::usermanagement::Database::init ()) {
    return -10;
  }
  edu::sharif::partov::nse::network::FrameFactory::init
      (new edu::sharif::partov::nse::network::EthernetIPFrameFactory ());
  edu::sharif::partov::nse::plugin::PluginNodeFactory::init ();
  if (!edu::sharif::partov::nse::emulation::EthernetDirectAccessDriver::init ()) {
    return -20;
  }
  if (!edu::sharif::partov::nse::network::transceiver::TransceiverFactory::init ()) {
    return -30;
  }
  if (!edu::sharif::partov::nse::util::PacketLoggerFactory::init ()) {
    return -40;
  }
  if (!edu::sharif::partov::nse::map::MapFactory::init ()) {
    return -50;
  }
  if (!edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->setupAdapterRoutersMap ()) {
    return -60;
  }
  if (!edu::sharif::partov::nse::util::SignalHandler::init ()) {
    return -70;
  }
  if (!QObject::connect (edu::sharif::partov::nse::util::SignalHandler::getInstance (),
                         SIGNAL (interruptSignalTriggered ()), &app, SLOT (quit ()))) {
    return -80;
  }

  cout << flush;
  int res = startPartovServer (app);

  edu::sharif::partov::nse::util::SignalHandler::destroy ();
  // destroy both the map factory and the routers map (internally) in mirror
  // of setupAdapterRoutersMap using the shutdownAdapterRoutersMap method.
  edu::sharif::partov::nse::map::MapFactory::getInstance ()->destroy ();
  edu::sharif::partov::nse::util::PacketLoggerFactory::destroy ();
  edu::sharif::partov::nse::network::transceiver::TransceiverFactory::destroy ();
  edu::sharif::partov::nse::emulation::EthernetDirectAccessDriver::getInstance ()->destroy ();
  edu::sharif::partov::nse::plugin::PluginNodeFactory::destroy ();
  edu::sharif::partov::nse::network::FrameFactory::destroy ();
  edu::sharif::partov::nse::usermanagement::Database::destroy ();

  return res;
}

int startPartovServer (QCoreApplication &app) {
  edu::sharif::partov::nse::server::Server server;
  QTimer::singleShot (0, &server, SLOT (listenForConnections ()));

  return app.exec ();
}

void printShortIntro () {
  cout << "________              _____                  ________                               " << endl;
  cout << "___  __ \\_____ _________  /_________   __    __  ___/______________   ______________" << endl;
  cout << "__  /_/ /  __ `/_  ___/  __/  __ \\_ | / /    _____ \\_  _ \\_  ___/_ | / /  _ \\_  ___/" << endl;
  cout << "_  ____// /_/ /_  /   / /_ / /_/ /_ |/ /     ____/ //  __/  /   __ |/ //  __/  /    " << endl;
  cout << "/_/     \\__,_/ /_/    \\__/ \\____/_____/      /____/ \\___//_/    _____/ \\___//_/     " << endl;
  cout << "                                                                                    " << endl;
  cout << "Partov Project - Version " << edu::sharif::partov::nse::map::Map::MAP_VERSION << " -- Central Server" << endl;
  cout << "Code-name: PARTOV (Portable And Reliable Tool fOr Virtualization)" << endl;
  cout << "Copyright © 2009-2014 Behnam Momeni." << endl;
  cout << "For any bugs, report to <b_momeni@ce.sharif.edu>" << endl;
  cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>." << endl;
  cout << "This is free software: you are free to change and redistribute it." << endl;
  cout << "There is NO WARRANTY, to the extent permitted by law." << endl;
  cout << endl;
}
