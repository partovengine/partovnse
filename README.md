# Partov NSE

The _Partov Network Simulation Emulation (PartovNSE)_ is an engine for discrete event simulation, extensible through plugins, joining real networks via emulation, supporting realtime scheduling, and a lot more.
Latest stable version of Partov central server is **3.4.0**.

## Installation

See the _INSTALL_ file for the manual installation guide.

## Usage

After completing installation and updating config file,
move to _PartovServer/deploy/bin/_ folder and either run

    ./launcher.bash

to start the Partov server (stop it using Ctrl+C signal) or run

    ./service_up.sh

to initiate a watcher process which starts Partov server and
restarts it after any interrupt (Ctrl+C will cause restart
instead of termination). You can use

    ./kill_service.sh

to stop it later.

## License
    Copyright © 2015 Behnam Momeni

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see {http://www.gnu.org/licenses/}.
