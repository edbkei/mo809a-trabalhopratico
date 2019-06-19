/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Wireless Communications and Networking Group (WCNG),
 * University of Rochester, Rochester, NY, USA.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Cristiano Tapparello <cristiano.tapparello@rochester.edu>
 */

/**
 *
 * This example extends the energy model example by connecting a basic energy
 * harvester to the nodes.
 *
 * The example considers a simple communication link between a source and a
 * destination node, where the source node sends a packet to the destination
 * every 1 second. Each node is powered by a BasicEnergySource, which is recharged
 * by a BasicEnergyHarvester, and the WiFi radio consumes energy for the transmission/
 * reception of the packets.
 *
 * For the receiver node, the example prints the energy consumption of the WiFi radio,
 * the power harvested by the energy harvester and the residual energy in the
 * energy source.
 *
 * The nodes initial energy is set to 1.0 J, the transmission and reception entail a
 * current consumption of 0.0174 A and 0.0197 A, respectively (default values in
 * WifiRadioEnergyModel). The energy harvester provides an amount of power that varies
 * according to a random variable uniformly distributed in [0 0.1] W, and is updated
 * every 1 s. The energy source voltage is 3 V (default value in BasicEnergySource) and
 * the residual energy level is updated every 1 second (default value).
 *
 * The simulation start at time 0 and it is hard stopped at time 10 seconds. Given the
 * packet size and the distance between the nodes, each transmission lasts 0.0023s.
 * As a result, the destination node receives 10 messages.
 *
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/energy-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-radio-energy-model-helper.h"
//include "ns3/nqos-wifi-mac-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("trabpratb");

static inline std::string
PrintReceivedPacket (Address& from)
{
  InetSocketAddress iaddr = InetSocketAddress::ConvertFrom (from);

  std::ostringstream oss;
  oss << "--\nReceived one packet! Socket: " << iaddr.GetIpv4 ()
      << " port: " << iaddr.GetPort ()
      << " at time = " << Simulator::Now ().GetSeconds ()
      << "\n--";

  return oss.str ();
}

/**
 * \param socket Pointer to socket.
 *
 * Packet receiving sink.
 */
void
ReceivePacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () > 0)
        {
          NS_LOG_UNCOND (PrintReceivedPacket (from));
        }
    }
}

/**
 * \param socket Pointer to socket.
 * \param pktSize Packet size.
 * \param n Pointer to node.
 * \param pktCount Number of packets to generate.
 * \param pktInterval Packet sending interval.
 *
 * Traffic generator.
 */
static void
GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, Ptr<Node> n,
                 uint32_t pktCount, Time pktInterval)
{
  if (pktCount > 0)
    {
      socket->Send (Create<Packet> (pktSize));
      Simulator::Schedule (pktInterval, &GenerateTraffic, socket, pktSize, n,
                           pktCount - 1, pktInterval);
      std::cout << "Packet sent, and size = "<<pktSize<<" pktCount= "<<pktCount<<" pktInterval = "<<pktInterval << std::endl;
    }
  else
    {
      socket->Close ();
    }
}

/// Trace function for remaining energy at node.
void
RemainingEnergy (double oldValue, double remainingEnergy)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Current remaining energy = " << remainingEnergy << "J oldValue = " << oldValue);
}

/// Trace function for total energy consumption at node.
void
TotalEnergy (double oldValue, double totalEnergy)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Total energy consumed by radio = " << totalEnergy << "J oldValue =" << oldValue);
}

/// Trace function for the power harvested by the energy harvester.
void
HarvestedPower (double oldValue, double harvestedPower)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Current harvested power = " << harvestedPower << " W oldValue = " << oldValue);
}

/// Trace function for the total energy harvested by the node.
void
TotalEnergyHarvested (double oldValue, double TotalEnergyHarvested)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Total energy harvested by harvester = "
                 << TotalEnergyHarvested << " J oldValue = " << oldValue);
}


int
main (int argc, char *argv[])
{
  std::string phyMode ("OfdmRate12Mbps");
  double Prss = -80;            // dBm
  uint32_t PacketSize = 200;   // bytes
  bool verbose = false;

  // simulation parameters
  uint32_t numPackets = 10000;  // number of packets to send
  //uint32_t numPackets = echoClient.SetAttribute("MaxPackets",UintegerValue(nPackets);  // number of packets to send
  //echoClient.SetAttribute("MaxPackets",UintegerValue(numPackets);
  double interval = 1;          // seconds
  double startTime = 0.0;       // seconds
  double distanceToRx = 300.0;  // meters
  double simTime=10.0;   // seconds

  /*
   * This is a magic number used to set the transmit power, based on other
   * configuration.
   */
  double offset = 81;
  
  // Energy Harvester variables --Comment if to remove harveser
  double harvestingUpdateInterval = 1;  // seconds

  // Add or Not Add Harvester
  //double addHarvester = 0; // default not add

  CommandLine cmd;
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("Prss", "Intended primary RSS (dBm)", Prss);
  cmd.AddValue ("PacketSize", "size of application packet sent", PacketSize);
  cmd.AddValue ("numPackets", "Total number of packets to send", numPackets);
  cmd.AddValue ("startTime", "Simulation start time", startTime);
  cmd.AddValue ("simTime", "Simulation duration time", simTime);
  //cmd.AddValue ("addHarvester", "Add Harvester to Energy Model", addHarvester);
  cmd.AddValue ("distanceToRx", "X-Axis distance between nodes", distanceToRx);
  cmd.AddValue ("verbose", "Turn on all device log components", verbose);
  cmd.Parse (argc, argv);
  std::cout << "distanceToRx = "<<distanceToRx<< std::endl;
  std::cout << "simTime = "<<simTime<< std::endl;
  //std::cout << "addHarvester = "<<addHarvester<< std::endl;

  // Convert to time object
  Time interPacketInterval = Seconds (interval);

  // disable fragmentation for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold",
                      StringValue ("2200"));
  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold",
                      StringValue ("2200"));
  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",
                      StringValue (phyMode));

  NodeContainer c;
  c.Create (2);     // create 2 nodes
  NodeContainer networkNodes;
  networkNodes.Add (c.Get (0));
  networkNodes.Add (c.Get (1));

  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  if (verbose)
    {
      wifi.EnableLogComponents ();
    }
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);

  /** Wifi PHY **/
  /***************************************************************************/
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  wifiPhy.Set ("RxGain", DoubleValue (-10));
  wifiPhy.Set ("TxGain", DoubleValue (offset + Prss)); // offset + Prss
  //wifiPhy.Set ("CcaMode1Threshold", DoubleValue (0.0));
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  /***************************************************************************/

  /** wifi channel **/
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel",
                                   "Exponent", DoubleValue (3.0));
  // create wifi channel
  Ptr<YansWifiChannel> wifiChannelPtr = wifiChannel.Create ();
  wifiPhy.SetChannel (wifiChannelPtr);

  // Add a non-QoS upper mac, and disable rate control (i.e. ConstantRateWifiManager)
  //NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));


  /** MAC layer **/
  // Add a MAC and disable rate control
  WifiMacHelper wifiMac;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
                                StringValue (phyMode), "ControlMode",
                                StringValue (phyMode));
  // Set it to ad-hoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");

  /** install PHY + MAC **/
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, networkNodes);

  /** mobility **/
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (distanceToRx, 0.0, 0.0)); // 2* originally * distanceToRx
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);

  /** Energy Model **/
  /***************************************************************************/
  /* energy source */
  BasicEnergySourceHelper basicSourceHelper;
  // configure energy source
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (100.0)); // 1.0 J
  // install source
  EnergySourceContainer sources = basicSourceHelper.Install (c);
  /* device energy model */
  WifiRadioEnergyModelHelper radioEnergyHelper;
  // configure radio energy model
  radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174));
  radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0.0197));
  // install device model
  DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (devices, sources);
  //if(addHarvester==1) {
     //std::cout << "addHarvester 1 ";
  /* energy harvester -- comment to remove harvester */
    BasicEnergyHarvesterHelper basicHarvesterHelper;
  /* configure energy harvester */
    basicHarvesterHelper.Set ("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (harvestingUpdateInterval)));
    basicHarvesterHelper.Set ("HarvestablePower", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=0.1]"));
  /* install harvester on all energy sources */
    EnergyHarvesterContainer harvesters = basicHarvesterHelper.Install (sources);
  //}
  /***************************************************************************/

  /** Internet stack **/
  InternetStackHelper internet;
  internet.Install (networkNodes);

  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("172.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket (networkNodes.Get (1), tid);  // node 1, Destination
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

  Ptr<Socket> source = Socket::CreateSocket (networkNodes.Get (0), tid);    // node 0, Source
  InetSocketAddress remote = InetSocketAddress (Ipv4Address::GetBroadcast (), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);

  /** connect trace sources **/
  /***************************************************************************/
  // all traces are connected to node 1 (Destination)
  // energy source
  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (1));
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));
  // device energy model
  Ptr<DeviceEnergyModel> basicRadioModelPtr =
    basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
  NS_ASSERT (basicRadioModelPtr != 0);
  basicRadioModelPtr->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));
  // energy harvester -- comment to remove harvester
    Ptr<BasicEnergyHarvester> basicHarvesterPtr = DynamicCast<BasicEnergyHarvester> (harvesters.Get (1));
    basicHarvesterPtr->TraceConnectWithoutContext ("HarvestedPower", MakeCallback (&HarvestedPower));
    basicHarvesterPtr->TraceConnectWithoutContext ("TotalEnergyHarvested", MakeCallback (&TotalEnergyHarvested));
  /***************************************************************************/


  /** simulation setup **/
  // start traffic
  Simulator::Schedule (Seconds (startTime), &GenerateTraffic, source, PacketSize,
                       networkNodes.Get (0), numPackets, interPacketInterval);

  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();

  for (DeviceEnergyModelContainer::Iterator iter = deviceModels.Begin (); iter != deviceModels.End (); iter ++)
    {
      double energyConsumed = (*iter)->GetTotalEnergyConsumption ();
      NS_LOG_UNCOND ("End of simulation (" << Simulator::Now ().GetSeconds ()
                     << "s) Total energy consumed by radio = " << energyConsumed << "J");
      NS_ASSERT (energyConsumed <= 100.0);
    }

  Simulator::Destroy ();

  return 0;
}
