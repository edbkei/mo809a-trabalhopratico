#!/bin/bash
#Autor: Eduardo S. Ito <e159086@g.unicamp.br>
#Data: 23/05/2019
#Nota: script para rodar simulacao de MO809A - T. de Sistema Distribuído - 2019
#Rodar: ./run_mo809b.sh p1

#$1 = options (h: run with help, v: run with vis, n: normal run

#Vai para diretorio ns-3.29
cd ..

#Resumo da simulacao
echo
echo " * MO809A - Topicos de Sistemas Distribuidos - 2019"
#---- delete file if exists ---
file="~/workspace/output.mo809a"
if [ -f $file ] ; then
   rm $file
fi
#------------------------------
if [ $1 == "w" ]
then
        echo " * Lista de parâmetro do comando waf"
			#Chamada da simulacao
	./waf --run "scratch/mo809a2019b --PrintHelp"
elif [ $1 == "v" ]
then
       #---- delete file if exists ---
       if [ -f ~/workspace/output.mo809a ];
       then
           rm ~/workspace/output.mo809a #echo "file deleted"
       fi


       #------------------------------
       echo " * opção visual"
       ./waf --run scratch/mo809a2019b --vis &>> ~/workspace/output.mo809a

elif [ $1 == "h" ]
then
       echo " * Combinações para o comando run_mo809b.sh"
       echo " "
       echo " ./run_mo809b.sh {p1} [p2] [p3]"
       echo " "
       echo " Parâmetro obrigatório: "
       echo " p1: w = Lista de parâmetros do comando waf"
       echo "     v = executar waf com a opção visual (opção --vis)"
       echo "     h = help ou mostrar todas as opções do comando run_mo809b.sh."
       echo "         Não precisa de outros parâmetros"
       echo "     n = modo normal. modelo de energia + harvester conectados ao nó WSN"
       echo "     s = modelo de energia conectados ao nó WSN sem o harvester"
       echo " "
       echo " Parâmetros opcionais:"
       echo " "
       echo " sem nenhum parâmetro opcional: 10 segundos de tempo de simulação"
       echo " p2: distância entre os nós em metros (opção --distanceToRx=valor)"
       echo " p3: tempo de simulação em segundos (opção --simTime=valor)"

elif [ $1 == "s" ]
then
       param=""
       if [ -z "${2+set}" ]; then
           param=""
       else
           param=" --distanceToRx=$2"
           echo "p2:distance entre os nós= $2 metros"
       fi

       if [ -z "${3+set}" ]; then
           param=$param
       else
           aux=" --simTime=$3"
           param=$param$aux
           echo "p3:tempo de simulação= $3 segundos"
       fi

       [ -d results ] || mkdir results
       if [ -f "results/ns3.output" ]
       then
           rm results/ns3.output #echo "file deleted"
       fi
       ./waf --run "scratch/mo809a2019c $param" &>> results/ns3.output
       cd scratch
       python run_mo809b_python.py
else 
       param=""
       if [ -z "${2+set}" ]; then
           param=""
       else
           param=" --distanceToRx=$2"
           echo "p2:distance entre os nós= $2 metros"
       fi

       if [ -z "${3+set}" ]; then
           param=$param
       else
           aux=" --simTime=$3"
           param=$param$aux
           echo "p3:tempo de simulação= $3 segundos"
       fi

       [ -d results ] || mkdir results
       if [ -f "results/ns3.output" ]
       then
           rm results/ns3.output #echo "file deleted"
       fi
       ./waf --run "scratch/mo809a2019b $param" &>> results/ns3.output
       cd scratch
       python run_mo809b_python.py
fi

echo "---------------------------------------------------------------"

#EOF
