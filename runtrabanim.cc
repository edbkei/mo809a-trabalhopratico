/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
 /*
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
  * Author: Vikas Pushkar (Adapted from third.cc)
  */
 
 #include <iostream>
 #include <sstream>
 #include <string>
 #include "ns3/core-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/csma-module.h"
 #include "ns3/network-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/mobility-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/netanim-module.h"
 #include "ns3/basic-energy-source.h"
 #include "ns3/simple-device-energy-model.h"
 #include "ns3/yans-wifi-helper.h"
 #include "ns3/ssid.h"
 #include "ns3/wifi-radio-energy-model.h"
 
 using namespace ns3;
 
 NS_LOG_COMPONENT_DEFINE ("runtrabanim");
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
          //std::cout<<"Packets received = "<<PrintReceivedPacket (from)<<std::endl;

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
      std::cout<<"Packet sent, and size = "<<pktSize<<" pktCount= "<<pktCount
                 <<" pktInterval ="<<pktInterval<<std::endl;
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
                 << "s Current remaining energy = " << remainingEnergy << "J");
}

/// Trace function for total energy consumption at node.
void
TotalEnergy (double oldValue, double totalEnergy)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Total energy consumed by radio = " << totalEnergy << "J");
}

/// Trace function for the power harvested by the energy harvester.
void
HarvestedPower (double oldValue, double harvestedPower)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Current harvested power = " << harvestedPower << " W");
}

/// Trace function for the total energy harvested by the node.
void
TotalEnergyHarvested (double oldValue, double TotalEnergyHarvested)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Total energy harvested by harvester = "
                 << TotalEnergyHarvested << " J");
}

 int 
 main (int argc, char *argv[])
 {
   uint32_t nWifi = 1;
   double distanceToRx = 100.0;  // meters
   double simTime=15.0;    // seconds
   double interval = 1;          // seconds
  // Convert to time object
   Time interPacketInterval = Seconds (interval);
   uint32_t PacketSize = 200;   // bytes
  // simulation parameters
   uint32_t numPackets = 10000;  // number of packets to send
   CommandLine cmd;
   std::stringstream stream;
   cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
   cmd.AddValue ("simTime", "Simulation time in seconds", simTime);
   cmd.AddValue ("distanceToRx", "Distance to Rx in meters", distanceToRx);
   cmd.Parse (argc,argv);
   std::cout<<"distanceToRx = "<<distanceToRx<<std::endl;
   std::cout<<"simTime = "<<simTime<<std::endl;
 
   //NodeContainer allNodes;
   NodeContainer wifiStaNodes;
   wifiStaNodes.Create (nWifi);
   //allNodes.Add (wifiStaNodes);
   NodeContainer wifiApNode ;
   wifiApNode.Create (1);
   //allNodes.Add (wifiApNode);
 
   YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
   YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
   phy.SetChannel (channel.Create ());
 
   WifiHelper wifi;
   wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
 
   WifiMacHelper mac;
   Ssid ssid = Ssid ("ns-3-ssid");
   mac.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid),
                "ActiveProbing", BooleanValue (false));
 
   NetDeviceContainer staDevices;
   staDevices = wifi.Install (phy, mac, wifiStaNodes);
   

   mac.SetType ("ns3::ApWifiMac",
                "Ssid", SsidValue (ssid)); 
   NetDeviceContainer apDevices;
   apDevices = wifi.Install (phy, mac, wifiApNode);
 
 
   //NodeContainer p2pNodes;
   //p2pNodes.Add (wifiApNode);
   //p2pNodes.Create (1);
   //allNodes.Add (p2pNodes.Get (1));
 
   //PointToPointHelper pointToPoint;
   //pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   //pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
 
   //NetDeviceContainer p2pDevices;
   //p2pDevices = pointToPoint.Install (p2pNodes);
 
   NodeContainer csmaNodes;
   //csmaNodes.Add (p2pNodes.Get (1));
   csmaNodes.Add(wifiApNode.Get(0)); //here
   csmaNodes.Create (1);
   //allNodes.Add (csmaNodes.Get (1));
 
   CsmaHelper csma;
   csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
   csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
 
   NetDeviceContainer csmaDevices;
   csmaDevices = csma.Install (csmaNodes);
 
   // Mobility
 
   MobilityHelper mobility;
   Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
   positionAlloc->Add (Vector (distanceToRx, 0.0, 0.0));
   mobility.SetPositionAllocator (positionAlloc);
   //mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
   //                               "MinX", DoubleValue (10.0),
   //                               "MinY", DoubleValue (10.0),
   //                               "DeltaX", DoubleValue (5.0),
   //                               "DeltaY", DoubleValue (2.0),
   //                               "GridWidth", UintegerValue (5),
   //                               "LayoutType", StringValue ("RowFirst"));
   //mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
   //                           "Bounds", RectangleValue (Rectangle (-50, 50, -25, 50)));
   mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobility.Install (wifiStaNodes);

   MobilityHelper mobilityAp;
   Ptr<ListPositionAllocator> positionAllocAp = CreateObject<ListPositionAllocator> ();
   positionAllocAp->Add (Vector (0, 0.0, 0.0));
   mobilityAp.SetPositionAllocator (positionAllocAp);
   mobilityAp.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityAp.Install (wifiApNode);

   Ptr<ListPositionAllocator> positionAllocCsma = CreateObject<ListPositionAllocator> ();
   positionAllocCsma->Add (Vector (-50, 0.0, 0.0));
   MobilityHelper mobilityCsma;
   mobilityCsma.SetPositionAllocator (positionAllocCsma);
   mobilityCsma.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityCsma.Install (csmaNodes.Get(1));

   //AnimationInterface::SetConstantPosition (p2pNodes.Get (1), 10, 30); 
   AnimationInterface::SetConstantPosition (csmaNodes.Get (1), -80, 30);  // before (10, 33)
 
   Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
   Ptr<WifiRadioEnergyModel> energyModel = CreateObject<WifiRadioEnergyModel>();
 
   energySource->SetInitialEnergy (100);
   energyModel->SetEnergySource (energySource);
   energySource->AppendDeviceEnergyModel (energyModel);
 
   // aggregate energy source to node
   //wifiApNode.Get (0)->AggregateObject (energySource);
   wifiStaNodes.Get (0)->AggregateObject (energySource);

 
   // Install internet stack
 
   InternetStackHelper stack;
   stack.Install (csmaNodes);
   stack.Install (wifiStaNodes);
 
   // Install Ipv4 addresses
 
   Ipv4AddressHelper address;
   //address.SetBase ("10.1.1.0", "255.255.255.0");
   //Ipv4InterfaceContainer p2pInterfaces;
   //p2pInterfaces = address.Assign (p2pDevices);

   address.SetBase ("10.1.2.0", "255.255.255.0");
   Ipv4InterfaceContainer csmaInterfaces;
   csmaInterfaces = address.Assign (csmaDevices);

   address.SetBase ("10.1.3.0", "255.255.255.0");
   Ipv4InterfaceContainer staInterfaces;
   staInterfaces = address.Assign (staDevices);

   //address.SetBase ("10.1.1.0", "255.255.255.0");
   Ipv4InterfaceContainer apInterface;
   apInterface = address.Assign (apDevices);

   //Packet trace
   TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
   Ptr<Socket> recvSink = Socket::CreateSocket (wifiStaNodes.Get (0), tid);  // node 1, Destination
   InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
   recvSink->Bind (local);
   recvSink->SetRecvCallback (MakeCallback (&ReceivePacket)); 

   Ptr<Socket> source = Socket::CreateSocket (csmaNodes.Get (0), tid);    // node 0, Source
   InetSocketAddress remote = InetSocketAddress (Ipv4Address::GetBroadcast (), 80);
   source->SetAllowBroadcast (true);
   source->Connect (remote);

   // Install applications
 
   UdpEchoServerHelper echoServer (9);
   ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (1));
   serverApps.Start (Seconds (1.0));
   serverApps.Stop (Seconds (simTime));
   UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (1), 9);
   echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
   echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.)));
   echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
   ApplicationContainer clientApps = echoClient.Install (wifiStaNodes);
   clientApps.Start (Seconds (2.0));
   clientApps.Stop (Seconds (simTime));
 
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
   Simulator::Stop (Seconds (simTime));
 
   stream<<"runtrab-animation"<<distanceToRx<<".xml";
   //stream<<"runtrab-animation"<<".xml";
   //AnimationInterface anim (stream.str()); // Mandatory
   AnimationInterface anim (stream.str()); // Mandatory

   for (uint32_t i = 0; i < wifiStaNodes.GetN (); ++i)
     {
       anim.UpdateNodeDescription (wifiStaNodes.Get (i), "STA"); // Optional
       anim.UpdateNodeColor (wifiStaNodes.Get (i), 255, 0, 0); // Optional
     }
   for (uint32_t i = 0; i < wifiApNode.GetN (); ++i)
     {
       anim.UpdateNodeDescription (wifiApNode.Get (i), "AP"); // Optional
       anim.UpdateNodeColor (wifiApNode.Get (i), 0, 255, 0); // Optional
     }
   for (uint32_t i = 0; i < csmaNodes.GetN (); ++i)
     {
       anim.UpdateNodeDescription (csmaNodes.Get (i), "CSMA"); // Optional
       anim.UpdateNodeColor (csmaNodes.Get (i), 0, 0, 255); // Optional 
     }
 
   anim.EnablePacketMetadata (); // Optional
   anim.EnableIpv4RouteTracking ("runtrabanim.xml", Seconds (0), Seconds (5), Seconds (0.25)); //Optional
   anim.EnableWifiMacCounters (Seconds (0), Seconds (10)); //Optional
   anim.EnableWifiPhyCounters (Seconds (0), Seconds (10)); //Optional

  /** simulation setup **/
  // start traffic
   Simulator::Schedule (Seconds (0.0), &GenerateTraffic, source, PacketSize,
                       wifiStaNodes.Get (0), numPackets, interPacketInterval);

   Simulator::Run ();
   double energyConsumed = energyModel->GetTotalEnergyConsumption();
   std::cout<<"End of Simulation (00s) Total energy consumed by radio= "<<energyConsumed<<" J"<<std::endl;
   Simulator::Destroy ();
   return 0;
 }


