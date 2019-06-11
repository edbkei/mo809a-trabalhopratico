#!/bin/bash
#Autor: Eduardo S. Ito <e159086@g.unicamp.br>
#Data: 23/05/2019
#Nota: script para rodar simulacao de MO809A - T. de Sistema Distribuído - 2019
#Rodar: ./run_mo809b.sh p1

# * Combinações para o comando run_mo809b.sh
#
# ./run_trab.sh {p1} [p2] [p3]
#
# Parâmetro obrigatório: 
# p1: número de interação para a distância escolhida
# p2: tempo de simulação em segundos
# p3: distância até o Rx em metros
#

#Vai para diretorio ns-3.29
cd ~/repos/ns-3-allinone/ns-3-dev/scratch
./run_trab.sh n 10 20

#EOF
