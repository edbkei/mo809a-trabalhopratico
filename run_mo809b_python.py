#!/usr/bin/env python

import re, sys, getopt, os
import os.path
import numpy as np
import matplotlib.pyplot as plt
#matplotlib.rcParams["backend"]="TkAgg"

def strip(pattern,arg):
    m = re.search(pattern, arg)
    return m

def buildCSV(position,arg):
    s=list(arg)
    j=0
    for i in position:
        if j!= 0:
           s.insert(i+j-1,';')
        j=j+1
    return ''.join(s)

def lineType(arg):
    m=strip("Current harvested power",arg)
    if m:
         return "-currentHarvestedPower-"
    m=strip("Current remaining energy",arg)
    if m:
         return "-currentRemainingEnergy-"  
    m=strip("Total energy consumed by radio",arg)
    if m:
        return "-totalEnergyConsumedByRadio-"
    m=strip("Total energy harvested by harvester",arg)
    if m:
        return "-totalEnergyHarvestedByHarvester-"    
    m=strip("Packet sent",arg)
    if m:
        return "-packetSent-"   
    m=strip("Received one packet",arg)
    if m:
        return "-packetReceived-"   
    m=strip("distanceToRx",arg)
    if m:
        return "-distanceToRx-"  
    m=strip("Waf",arg)
    if m:
        return "-waf-"
    return False

def stripWordsFromText(arg):
    return re.sub("[^\w]", " ",  arg).split()

def read_file(arg_in, arg_out):
    if os.path.exists(arg_out):
        os.remove(arg_out)
    fout=open(arg_out,'a+')
    first=True
    lineout=""
    skipline="NO"
    packetSent=0
    packetReceived=0
    distanceToRx=0
    finp= open(arg_in, "r")
    linecsv=""
    for line in finp:
        if not line:
            finp.close()
            #fout.close()
            break
        s=lineType(line)
        #print(line)
        a=0
        b=0
        c=0
        d=0
        e=0
        if s=="-waf-":
            if  first:
                linecsv="timestamp, currentHarvestedPower, currentRemainingEnergy, totalEnergyConsumedByRadio, totalEnergyHarvestedByHarvester"
                fout.write(linecsv+"\n")
                first = False
                
        if s=="-currentHarvestedPower-":
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                linecsv=str(value[0])+", "+str(value[1])+", 0, 0, 0" 
                fout.write(linecsv+"\n")
                
            
        if s=="-currentRemainingEnergy-":
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                linecsv=str(value[0])+", 0, "+str(value[1])+", 0, 0" 
                fout.write(linecsv+"\n")
                
        if s=="-totalEnergyConsumedByRadio-":
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)           
                linecsv=str(value[0])+", 0, 0, "+str(value[1])+", 0" 
                fout.write(linecsv+"\n")
                
        if s=="-totalEnergyHarvestedByHarvester-":
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)             
                linecsv=str(value[0])+", 0, 0, 0, "+str(value[1]) 
                fout.write(linecsv+"\n")
        if s=="-packetSent-":
                packetSent = packetSent + 1       
        if s=="-packetReceived-":
                packetReceived = packetReceived + 1 
        if s=="-distanceToRx-":
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                distanceToRx=value[0]
    np.save('test', np.array([packetSent,packetReceived, distanceToRx]))
                
import csv
import math

def make_statistics(arg_in): 
    headers=[]
    first=True
    energy=[]

    with open(arg_in) as csvfile:
        readCSV = csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            if first:
                first=False
                headers=row
            else:
                energy.append(row)
                
        csvfile.close()
    x = np.array(energy)
    energy = x.astype(np.float)
    last=0
    j=0
    #print("energy.shape= ",energy.shape)
    somay=0
    for i in np.nditer(energy[:, 4:5]):
        if(i!=0):
            last=energy[j, 4:5]
        else:
            energy[j, 4:5]=last
        somay=somay+energy[j, 4:5]
        j=j+1
        #print(i, end=' ')
    #print("somay= ",somay)
    last=0
    j=0
    powerJ=[]
    soma=0
    for i in np.nditer(energy[:, 3:4]):
        if(i!=0):
            last=energy[j, 3:4]
        else:
            energy[j, 3:4]=last
        soma=soma+energy[j, 3:4]
        powerJ=np.append(powerJ,soma)
        j=j+1
        
  

    last=0
    j=0

    for i in np.nditer(energy[:, 2:3]):
        if(i!=0):
            last=energy[j, 2:3]
        else:
            energy[j, 2:3]=last
        j=j+1
        #print(i, end=' ')
    #np.save('outfile', energy)
    #print(energy)
    X=energy[:, 0:1]
    Y=energy[:, 4:5]
    Z=energy[:, 3:4]
    #print(X)
    #print(Y)
    #print(Z)
    #Z=powerJ
    W=energy[:, 2:3]
        #print(X)
        #print(Y)
    k=np.load('test.npy')
    #print("Packets Sent = ",k[0], "Packets Received =",k[1],"Packets Lost =",int(k[0])-int(k[1]))
    #print("Consumed Energy by WiFi Radio =",round(soma[0],2),"J     Distance Between Nodes = ",k[2], "meters" )
    titlex=""
    kk=somay[0]
    if(kk!=0):
         titlex="Energy Consumed by Wireless Node With Harvester"
    else:
         titlex="Energy Consumed by Wireless Node Without Harvester"
    plt.xlabel('Time (s)')
    plt.ylabel('Energy(J)')
    plt.title(titlex)
    #fig = plt.figure()
    #ax = plt.subplot(111)
    #plt.plot(X, Y, label='Energy Harvested by Harvester')
    #plt.plot(X, Y, '-b')
    plt.plot(X, Z, label='Total Energy Consumed By Wireless Node')
    #plt.plot(X, Z, '-r')
    plt.plot(X, W, label='Current remaining energy')
    #plt.plot(X, W, '-g')
    #plt.errorbar(X,Z,ls="None",label='Total Energy Consumed By Wireless Node')
    # Get current size
    fig_size = plt.rcParams["figure.figsize"]
 
    # Prints: [9.0, 7.0]
    #print ("Current size:", fig_size)
 
    # Set figure width to 12 and height to 9
    fig_size[0] = 6
    fig_size[1] = 5
    plt.rcParams["figure.figsize"] = fig_size
    #print ("Current size:", fig_size)
    #chartBox = ax.get_position()
    #chartBox = plt.get_position()
    #plt.set_position([chartBox.x0, chartBox.y0, chartBox.width*0.7, chartBox.height])
    #plt.legend(loc='upper center', bbox_to_anchor=(1.45, 0.8), shadow=True, ncol=1)
    #ax.legend()
    #ax.set_position([chartBox.x0, chartBox.y0, chartBox.width*0.6, chartBox.height])
    #ax.legend(loc='upper center', bbox_to_anchor=(1.45, 0.8), shadow=True, ncol=1)
    #ax.legend(loc='upper center', bbox_to_anchor=(2.0, 1.0), shadow=True, ncol=1)
    #print("OK")
    aux1="Pkts Sent by orig. Node="+str(k[0])
    aux2="Pkts Received at dest. Node="+str(k[1])
    aux3="Distance (in mts) between Nodes="+str(k[2])
    plt.figtext(.2, .7, aux1, fontsize='small')
    plt.figtext(.2, .66, aux2, fontsize='small')
    plt.figtext(.2, .62, aux3, fontsize='small')
    plt.legend(loc='center right', shadow=True, fancybox=True,fontsize='small')
    plt.show()

if __name__ == "__main__":
   inputfile =  '/home/ubu01/workspace/bake/source/ns-3.29/results/ns3.output'
   outputfile = '/home/ubu01/workspace/bake/source/ns-3.29/results/MO809A_NS3_TrabalhoPratico.out'
    
   read_file(inputfile, outputfile)
  
   make_statistics(outputfile)
