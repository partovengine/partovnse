######################################################################
# Automatically generated by qmake (2.01a) Sat Jan 25 14:21:36 2014
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . \
              src \
              src/edu/sharif/partov/exception \
              src/edu/sharif/partov/nse/emulation \
              src/edu/sharif/partov/nse/fsm \
              src/edu/sharif/partov/nse/map \
              src/edu/sharif/partov/nse/network \
              src/edu/sharif/partov/nse/plugin \
              src/edu/sharif/partov/nse/server \
              src/edu/sharif/partov/nse/usermanagement \
              src/edu/sharif/partov/nse/util \
              src/edu/sharif/partov/nse/map/builder \
              src/edu/sharif/partov/nse/map/interface \
              src/edu/sharif/partov/nse/map/link \
              src/edu/sharif/partov/nse/network/address \
              src/edu/sharif/partov/nse/network/transceiver
INCLUDEPATH += . \
               src/edu/sharif/partov/nse/server \
               src/edu/sharif/partov/nse/usermanagement \
               src/edu/sharif/partov/nse/map \
               src/edu/sharif/partov/exception \
               src/edu/sharif/partov/nse/network/transceiver \
               src/edu/sharif/partov/nse/plugin \
               src/edu/sharif/partov/nse/emulation \
               src/edu/sharif/partov/nse/network \
               src/edu/sharif/partov/nse/util \
               src/edu/sharif/partov/nse/network/address \
               src/edu/sharif/partov/nse/map/interface \
               src/edu/sharif/partov/nse/fsm \
               src/edu/sharif/partov/nse/map/builder \
               src/edu/sharif/partov/nse/map/link

# Input
HEADERS += src/edu/sharif/partov/exception/Exception.h \
           src/edu/sharif/partov/nse/emulation/EthernetDirectAccessDriver.h \
           src/edu/sharif/partov/nse/emulation/FrameDeliveryEvent.h \
           src/edu/sharif/partov/nse/emulation/InternetGateWayRelay.h \
           src/edu/sharif/partov/nse/emulation/NetworkAdapterInfo.h \
           src/edu/sharif/partov/nse/emulation/NoActiveNetworkAdapterException.h \
           src/edu/sharif/partov/nse/emulation/PacketObserver.h \
           src/edu/sharif/partov/nse/fsm/ActivatableTransition.h \
           src/edu/sharif/partov/nse/fsm/ExponentiallyTimedState.h \
           src/edu/sharif/partov/nse/map/AbstractScheduler.h \
           src/edu/sharif/partov/nse/map/CompacttimeScheduler.h \
           src/edu/sharif/partov/nse/map/ListManager.h \
           src/edu/sharif/partov/nse/map/Map.h \
           src/edu/sharif/partov/nse/map/MapExceptions.h \
           src/edu/sharif/partov/nse/map/MapFactory.h \
           src/edu/sharif/partov/nse/map/MapFileFormatException.h \
           src/edu/sharif/partov/nse/map/MapInstantiator.h \
           src/edu/sharif/partov/nse/map/MapThread.h \
           src/edu/sharif/partov/nse/map/Node.h \
           src/edu/sharif/partov/nse/map/RealtimeScheduler.h \
           src/edu/sharif/partov/nse/map/Scheduler.h \
           src/edu/sharif/partov/nse/map/UnBoxable.h \
           src/edu/sharif/partov/nse/map/VariableManager.h \
           src/edu/sharif/partov/nse/network/ARPPacket.h \
           src/edu/sharif/partov/nse/network/ARPPacketProxy.h \
           src/edu/sharif/partov/nse/network/Array.h \
           src/edu/sharif/partov/nse/network/ArrayBasedFrame.h \
           src/edu/sharif/partov/nse/network/EthernetFrame.h \
           src/edu/sharif/partov/nse/network/EthernetIPFrameFactory.h \
           src/edu/sharif/partov/nse/network/FirstLayerFrame.h \
           src/edu/sharif/partov/nse/network/FirstLayerFrameProxy.h \
           src/edu/sharif/partov/nse/network/Frame.h \
           src/edu/sharif/partov/nse/network/FrameFactory.h \
           src/edu/sharif/partov/nse/network/FrameProxy.h \
           src/edu/sharif/partov/nse/network/ICMPPacket.h \
           src/edu/sharif/partov/nse/network/ICMPPacketImp.h \
           src/edu/sharif/partov/nse/network/ICMPPacketProxy.h \
           src/edu/sharif/partov/nse/network/IOException.h \
           src/edu/sharif/partov/nse/network/IPv4OverEthernetARPPacket.h \
           src/edu/sharif/partov/nse/network/IPv4Packet.h \
           src/edu/sharif/partov/nse/network/ReferenceCounter.h \
           src/edu/sharif/partov/nse/network/SecondLayerFrame.h \
           src/edu/sharif/partov/nse/network/SecondLayerFrameProxy.h \
           src/edu/sharif/partov/nse/network/SharedObject.h \
           src/edu/sharif/partov/nse/network/ThirdLayerPacket.h \
           src/edu/sharif/partov/nse/network/ThirdLayerPacketProxy.h \
           src/edu/sharif/partov/nse/plugin/GeneralTCPReverseProxy.h \
           src/edu/sharif/partov/nse/plugin/InternetGateWayNode.h \
           src/edu/sharif/partov/nse/plugin/NonPresentFieldException.h \
           src/edu/sharif/partov/nse/plugin/OneToOneIcmpProxy.h \
           src/edu/sharif/partov/nse/plugin/Pinger.h \
           src/edu/sharif/partov/nse/plugin/PluginNode.h \
           src/edu/sharif/partov/nse/plugin/PluginNodeFactory.h \
           src/edu/sharif/partov/nse/plugin/PluginNodeInstantiator.h \
           src/edu/sharif/partov/nse/plugin/PluginNotInstalledException.h \
           src/edu/sharif/partov/nse/plugin/PluginScheduler.h \
           src/edu/sharif/partov/nse/plugin/Router.h \
           src/edu/sharif/partov/nse/plugin/RoutingRule.h \
           src/edu/sharif/partov/nse/plugin/SimulatedNode.h \
           src/edu/sharif/partov/nse/server/Server.h \
           src/edu/sharif/partov/nse/server/Simulator.h \
           src/edu/sharif/partov/nse/usermanagement/Database.h \
           src/edu/sharif/partov/nse/usermanagement/Group.h \
           src/edu/sharif/partov/nse/usermanagement/User.h \
           src/edu/sharif/partov/nse/usermanagement/UserAuthenticator.h \
           src/edu/sharif/partov/nse/util/PacketLogger.h \
           src/edu/sharif/partov/nse/util/PacketLoggerFactory.h \
           src/edu/sharif/partov/nse/util/ScalarLogger.h \
           src/edu/sharif/partov/nse/util/SignalHandler.h \
           src/edu/sharif/partov/nse/map/builder/ElementVisitor.h \
           src/edu/sharif/partov/nse/map/builder/EthernetInterfaceVisitor.h \
           src/edu/sharif/partov/nse/map/builder/Formula.h \
           src/edu/sharif/partov/nse/map/builder/FsmVisitor.h \
           src/edu/sharif/partov/nse/map/builder/InterfaceVisitor.h \
           src/edu/sharif/partov/nse/map/builder/ItemVisitor.h \
           src/edu/sharif/partov/nse/map/builder/LinkVisitor.h \
           src/edu/sharif/partov/nse/map/builder/ListVisitor.h \
           src/edu/sharif/partov/nse/map/builder/MapReader.h \
           src/edu/sharif/partov/nse/map/builder/NamedElementTwoPassVisitor.h \
           src/edu/sharif/partov/nse/map/builder/NamedElementVisitor.h \
           src/edu/sharif/partov/nse/map/builder/NodeVisitor.h \
           src/edu/sharif/partov/nse/map/builder/PhysicalEthernetInterfaceVisitor.h \
           src/edu/sharif/partov/nse/map/builder/PluginNodeVisitor.h \
           src/edu/sharif/partov/nse/map/builder/SimpleNodeVisitor.h \
           src/edu/sharif/partov/nse/map/builder/TransitionVisitor.h \
           src/edu/sharif/partov/nse/map/builder/UnnamedFieldRefVisitor.h \
           src/edu/sharif/partov/nse/map/builder/UnnamedFieldVisitor.h \
           src/edu/sharif/partov/nse/map/builder/Visitor.h \
           src/edu/sharif/partov/nse/map/interface/EthernetInterface.h \
           src/edu/sharif/partov/nse/map/interface/EthernetPhysicalInterface.h \
           src/edu/sharif/partov/nse/map/interface/Interface.h \
           src/edu/sharif/partov/nse/map/link/Link.h \
           src/edu/sharif/partov/nse/map/link/LinkCsmaCd.h \
           src/edu/sharif/partov/nse/map/link/LinkPointToPoint.h \
           src/edu/sharif/partov/nse/network/address/EthernetMACAddress.h \
           src/edu/sharif/partov/nse/network/address/MACAddress.h \
           src/edu/sharif/partov/nse/network/transceiver/ARPEntry.h \
           src/edu/sharif/partov/nse/network/transceiver/Ethernet2ndLayerTransceiver.h \
           src/edu/sharif/partov/nse/network/transceiver/EthernetARPEntry.h \
           src/edu/sharif/partov/nse/network/transceiver/EthernetProxyARPTransceiver.h \
           src/edu/sharif/partov/nse/network/transceiver/IPBlockRule.h \
           src/edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.h \
           src/edu/sharif/partov/nse/network/transceiver/StaticARPEntry.h \
           src/edu/sharif/partov/nse/network/transceiver/TransceiverFactory.h \
           src/edu/sharif/partov/nse/network/transceiver/WaitingFrameEntry.h
SOURCES += src/main.cpp \
           src/edu/sharif/partov/exception/Exception.cpp \
           src/edu/sharif/partov/nse/emulation/EthernetDirectAccessDriver.cpp \
           src/edu/sharif/partov/nse/emulation/FrameDeliveryEvent.cpp \
           src/edu/sharif/partov/nse/emulation/InternetGateWayRelay.cpp \
           src/edu/sharif/partov/nse/emulation/NetworkAdapterInfo.cpp \
           src/edu/sharif/partov/nse/emulation/PacketObserver.cpp \
           src/edu/sharif/partov/nse/fsm/ActivatableTransition.cpp \
           src/edu/sharif/partov/nse/fsm/ExponentiallyTimedState.cpp \
           src/edu/sharif/partov/nse/map/AbstractScheduler.cpp \
           src/edu/sharif/partov/nse/map/CompacttimeScheduler.cpp \
           src/edu/sharif/partov/nse/map/ListManager.cpp \
           src/edu/sharif/partov/nse/map/Map.cpp \
           src/edu/sharif/partov/nse/map/MapFactory.cpp \
           src/edu/sharif/partov/nse/map/MapFileFormatException.cpp \
           src/edu/sharif/partov/nse/map/MapInstantiator.cpp \
           src/edu/sharif/partov/nse/map/MapThread.cpp \
           src/edu/sharif/partov/nse/map/Node.cpp \
           src/edu/sharif/partov/nse/map/RealtimeScheduler.cpp \
           src/edu/sharif/partov/nse/map/VariableManager.cpp \
           src/edu/sharif/partov/nse/network/ArrayBasedFrame.cpp \
           src/edu/sharif/partov/nse/network/EthernetFrame.cpp \
           src/edu/sharif/partov/nse/network/EthernetIPFrameFactory.cpp \
           src/edu/sharif/partov/nse/network/FirstLayerFrame.cpp \
           src/edu/sharif/partov/nse/network/FirstLayerFrameProxy.cpp \
           src/edu/sharif/partov/nse/network/Frame.cpp \
           src/edu/sharif/partov/nse/network/FrameFactory.cpp \
           src/edu/sharif/partov/nse/network/FrameProxy.cpp \
           src/edu/sharif/partov/nse/network/ICMPPacket.cpp \
           src/edu/sharif/partov/nse/network/ICMPPacketImp.cpp \
           src/edu/sharif/partov/nse/network/ICMPPacketProxy.cpp \
           src/edu/sharif/partov/nse/network/IPv4OverEthernetARPPacket.cpp \
           src/edu/sharif/partov/nse/network/IPv4Packet.cpp \
           src/edu/sharif/partov/nse/network/ReferenceCounter.cpp \
           src/edu/sharif/partov/nse/network/SecondLayerFrame.cpp \
           src/edu/sharif/partov/nse/network/SecondLayerFrameProxy.cpp \
           src/edu/sharif/partov/nse/network/SharedObject.cpp \
           src/edu/sharif/partov/nse/network/ThirdLayerPacketProxy.cpp \
           src/edu/sharif/partov/nse/plugin/GeneralTCPReverseProxy.cpp \
           src/edu/sharif/partov/nse/plugin/InternetGateWayNode.cpp \
           src/edu/sharif/partov/nse/plugin/OneToOneIcmpProxy.cpp \
           src/edu/sharif/partov/nse/plugin/Pinger.cpp \
           src/edu/sharif/partov/nse/plugin/PluginNode.cpp \
           src/edu/sharif/partov/nse/plugin/PluginNodeFactory.cpp \
           src/edu/sharif/partov/nse/plugin/PluginScheduler.cpp \
           src/edu/sharif/partov/nse/plugin/Router.cpp \
           src/edu/sharif/partov/nse/plugin/RoutingRule.cpp \
           src/edu/sharif/partov/nse/plugin/SimulatedNode.cpp \
           src/edu/sharif/partov/nse/server/Server.cpp \
           src/edu/sharif/partov/nse/server/Simulator.cpp \
           src/edu/sharif/partov/nse/usermanagement/Database.cpp \
           src/edu/sharif/partov/nse/usermanagement/Group.cpp \
           src/edu/sharif/partov/nse/usermanagement/User.cpp \
           src/edu/sharif/partov/nse/usermanagement/UserAuthenticator.cpp \
           src/edu/sharif/partov/nse/util/PacketLogger.cpp \
           src/edu/sharif/partov/nse/util/PacketLoggerFactory.cpp \
           src/edu/sharif/partov/nse/util/ScalarLogger.cpp \
           src/edu/sharif/partov/nse/util/SignalHandler.cpp \
           src/edu/sharif/partov/nse/map/builder/ElementVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/EthernetInterfaceVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/Formula.cpp \
           src/edu/sharif/partov/nse/map/builder/FsmVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/InterfaceVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/ItemVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/LinkVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/ListVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/MapReader.cpp \
           src/edu/sharif/partov/nse/map/builder/NamedElementTwoPassVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/NamedElementVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/NodeVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/PhysicalEthernetInterfaceVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/PluginNodeVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/SimpleNodeVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/TransitionVisitor.cpp \
           src/edu/sharif/partov/nse/map/builder/Visitor.cpp \
           src/edu/sharif/partov/nse/map/interface/EthernetInterface.cpp \
           src/edu/sharif/partov/nse/map/interface/EthernetPhysicalInterface.cpp \
           src/edu/sharif/partov/nse/map/interface/Interface.cpp \
           src/edu/sharif/partov/nse/map/link/Link.cpp \
           src/edu/sharif/partov/nse/map/link/LinkCsmaCd.cpp \
           src/edu/sharif/partov/nse/map/link/LinkPointToPoint.cpp \
           src/edu/sharif/partov/nse/network/address/EthernetMACAddress.cpp \
           src/edu/sharif/partov/nse/network/transceiver/ARPEntry.cpp \
           src/edu/sharif/partov/nse/network/transceiver/Ethernet2ndLayerTransceiver.cpp \
           src/edu/sharif/partov/nse/network/transceiver/EthernetARPEntry.cpp \
           src/edu/sharif/partov/nse/network/transceiver/EthernetProxyARPTransceiver.cpp \
           src/edu/sharif/partov/nse/network/transceiver/IPBlockRule.cpp \
           src/edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.cpp \
           src/edu/sharif/partov/nse/network/transceiver/StaticARPEntry.cpp \
           src/edu/sharif/partov/nse/network/transceiver/TransceiverFactory.cpp \
           src/edu/sharif/partov/nse/network/transceiver/WaitingFrameEntry.cpp

include(Partov_custom.pro)