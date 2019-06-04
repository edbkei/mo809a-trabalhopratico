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
    #print(len(arg))
    #print(arg)
    m=strip("E #",arg)
    if m:
         return "-begin-"
    #m=arg[0]
    if (len(arg)<3):
         return "-end-"
    m=strip("Residual capacity",arg)
    if m:
         return "-residual-"  
    m=strip("PowerConsumptionShowcase",arg)
    if m:
        return "-powerConsumptionShowCase-"
   
    return False

def returnState(arg):
    #print(len(arg))
    #print(arg)
    m=strip("IDLE.",arg)
    if m:
         return "IDLE"

    m=strip("TRANSMITTING.",arg)
    if m:
         return "TRANSMITTING"
        
    m=strip("RECEIVING.",arg)
    if m:
         return "RECEIVING"   
        
    m=strip("WHOLE.",arg)
    if m:
         return "WHOLE" 
        
    m=strip("NONE.",arg)
    if m:
         return "NONE" 
    return "UNDEFINED"

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
    ts=0
    ev=0
    node=9999
    capa=0
    rm="UNDEFINED"
    txState="UNDEFINED"
    rxState="UNDEFINED"
    txStatus="UNDEFINED"
    rxStatus="UNDEFINED"
    txPw=0
    rxPw=0
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

        if s=="-begin-":
            if  first:
                linecsv="event,timestamp,node,residual,txState,rxState,radioMode,txPw,rxPw,txStatus,rxStatus"
                fout.write(linecsv+"\n")
                first = False
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                ev=value[0]
                ts=value[1]

            else:
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                ev=value[0]
                ts=value[1]  
                
        if s=="-end-": 
                linecsv=str(ev)+","+str(ts)+","+str(node)+","+str(capa)+","+txState+","+rxState+\
                    ","+rm+","+str(txPw)+","+str(rxPw)+","+txStatus+","+rxStatus
                fout.write(linecsv+"\n")    

                ts=0
                ev=0
                node=9999
                capa=0
                txState="UNDEFINED"
                rxState="UNDEFINED"
                txStatus="UNDEFINED"
                rxStatus="UNDEFINED"
                rm="UNDEFINED"
                txPw=0
                rxPw=0
                
        if s=="-residual-":
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                node=value[0]
                capa=value[2]

        if s=="-powerConsumptionShowCase-":
                value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                m=strip("IPv4",line)
                OK=0
                if m:
                    node=value[1]
                    OK=1
                else:
                    node=value[0]
                m=strip("Ieee80211",line)
                if m:
                    node=value[1]
                else:
                    if (OK==0):
                        node=value[0]

                w=""

                w1=strip("Radio mode changed from",line)
                if w1:
                    w1=strip("TRANSMITTER to RECEIVER",line)
                    if w1:
                        rm = "RECEIVER"
                    else:
                        rm = "TRANSMITTER"
                    
                w1=strip("Changing radio transmission state from",line)
                if w1:
                    w= "-changeTxState-"
                w1=strip("Changing radio transmitted signal part from",line)
                if w1:
                    w= "-changeTxState-"
                w1=strip("Changing radio reception state from",line)
                if w1:
                    w= "-changeRxState-"
                w1=strip("Changing radio received signal part from",line)
                if w1:
                    w= "-changeRxState-"
                if (w=="-changeTxState-"):
                    txState=returnState(line)
                if (w=="-changeRxState-"):
                    rxState=returnState(line)
                w1=strip("Changing radio transmitted signal part from",line)
                if w1:
                    w= "-changeTxState-"
 
                w1=strip("Computing whether listening",line)
                if w1:
                    value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                    v1=int(value[3])
                    v2=int(value[4])
                    v3=(v1*10)**(-v2)
                    rxPw=v3

                w1=strip("Computing whether reception is possible",line)
                if w1:
                    value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                    v1=float(value[3])
                    v2=int(value[4])
                    #print(value)
                    v3=v1*10**(-v2)
                    rxPw=v3

                w1=strip("Transmission started",line)
                if w1:
                    value=re.findall(r"[-+]?\d*\.\d+|\d+", line)
                    v1=value[12]

                    txPw=v1
                    txStatus="TXSTARTED"
 
                w1=strip("Transmission ended",line)
                if w1:
                    txStatus="TXENDED"
                w1=strip("reception is possible",line)
                if w1:
                    rxStatus="RXPOSSIBLE"
                w1=strip("listening is impossible",line)
                if w1:
                    rxStatus="RXNOTPOSSIBLE"
    
    finp.close()
    fout.close()
                
import csv
import math

def make_statistics(arg_in): 
    
    dataset=np.genfromtxt(arg_in, dtype=(int, float, int, int, "|U13", "|U13", "|U13", float, float, "|U13", "|U13"), delimiter=',', names=True)
    #print(dataset.dtype.names)
    #print(dataset['rxStatus'])
    X=dataset[dataset['node']==0]
    Y=dataset[dataset['node']==1]

    x1=X['timestamp']
    y1=X['residual']
    x2=Y['timestamp']
    y2=Y['residual']
    y3=Y['txPw']
    y4=Y['rxPw']
    y5=X['txPw']
    y6=X['rxPw']
    y7=X['txStatus']
    y8=X['rxStatus']
    y9=Y['txStatus']
    y10=Y['rxStatus']
    y11=X['radioMode']
    y12=Y['radioMode']
    an = np.linspace(0, 2*np.pi, 100)
    fig, axs = plt.subplots(3, 2)
    
    axs[0, 0].set_xlabel('time (s)')
    axs[0, 0].set_ylabel('Residual capacity in %')
    axs[0, 0].scatter(x1, y1)
    axs[0, 0].set_title('Residual Energy Capacity of Node 0')

    axs[0, 1].set_xlabel('time (s)')
    axs[0, 1].set_ylabel('Residual capacity in %')
    axs[0, 1].scatter(x2, y2)
    axs[0, 1].set_title('Residual Energy Capacity of Node 1')
    fig_size = plt.rcParams["figure.figsize"]
    
    axs[1, 0].set_xlabel('time (s)')
    axs[1, 0].set_ylabel('Energy (J)')
    l1=axs[1, 0].scatter(x1, y5, color="red", label="Tx energy")
    l2=axs[1, 0].scatter(x1, y6, color="blue", label="Rx energy")  
    axs[1, 0].legend(handles=[l1, l2])
    axs[1, 0].set_title('Energy behaviour at Node 0')   
    
    axs[1, 1].set_xlabel('time (s)')
    axs[1, 1].set_ylabel('Energy (J)')
    l3=axs[1, 1].scatter(x2, y3, color="red", label="Tx energy")
    l4=axs[1, 1].scatter(x2, y4, color="blue",label ="Rx energy")
    axs[1, 1].legend(handles=[l3, l4])
    axs[1, 1].set_title('Energy behaviour at Node 1')
    
    #axs[2, 0].set_xlabel('time (s)')
    #axs[2, 0].set_ylabel('Radio Status')
    #axs[2, 0].tick_params(labelsize=15)
    #l7=axs[2, 0].scatter(x1, y7, color="red", label="Tx Status" )
    #l8=axs[2, 0].scatter(x1, y8, color="blue", label="Rx Status" )
    #axs[2, 0].legend(handles=[l7, l8])
    #axs[2, 0].set_title('Radio Status at Node 0')  
    
    #axs[2, 1].set_xlabel('time (s)')
    #axs[2, 1].set_ylabel('Radio Status')
    #axs[2, 1].tick_params(labelsize=15)
    #l9=axs[2, 1].scatter(x2, y9, color="red", label="Tx Status")
    #l10=axs[2, 1].scatter(x2, y10, color="blue", label="Rx Status")  
    #axs[2, 1].legend(handles=[l9, l10])
    #axs[2, 1].set_title('Radio Status at Node 1')
    
    axs[2, 0].set_xlabel('time (s)')
    axs[2, 0].set_ylabel('Radio Mode')
    #axs[3, 0].tick_params(labelsize=15)
    axs[2, 0].scatter(x1, y11, color="blue" )
    axs[2, 0].set_title('Radio Mode at Node 0')   
    

    axs[2, 1].set_xlabel('time (s)')
    axs[2, 1].set_ylabel('Radio Mode')
    #axs[3, 1].tick_params(labelsize=15)
    axs[2, 1].scatter(x2, y12, color="blue")
    axs[2, 1].set_title('Radio Mode at Node 1')   
	
    fig_size = plt.rcParams["figure.figsize"] 
    fig_size[0] = 25.0 #18
    fig_size[1] = 22.0 #15
 
    # Prints: [9.0, 7.0]
    #print ("Current size:", fig_size)
    
    fig.tight_layout()
    
    plt.show()

if __name__ == "__main__":
   #inputfile =  '/home/ubu01/workspace/bake/source/ns-3.29/results/ns3.output'
   #outputfile = '/home/ubu01/workspace/bake/source/ns-3.29/results/MO809A_NS3_TrabalhoPratico.out'
    
   inputfile = 'C:\\\\Users\\\\Eduardo\\\\Documents\\\\2019\\\\UNICAMP\\\\MO809A TOPICOS COMP DIST\\\\Trabalho\\\\TrabalhoPratico\\\\OMNETPPINET\\\\General-#0.elog'
   outputfile = 'C:\\\\Users\\\\Eduardo\\\\Documents\\\\2019\\\\UNICAMP\\\\MO809A TOPICOS COMP DIST\\\\Trabalho\\\\TrabalhoPratico\\\\OMNETPPINET\\\\omnet12.pp'
   read_file(inputfile, outputfile)
  
   make_statistics(outputfile)