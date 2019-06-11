#!/bin/bash
#Autor: Eduardo S. Ito <e159086@g.unicamp.br>
#Data: 23/05/2019
#Nota: script para rodar simulacao de MO809A - T. de Sistema Distribuído - 2019
#Rodar: ./run_mo809b.sh p1

# * Combinações para o comando run_mo809b.sh
#
# ./run_mo809b.sh {p1} [p2] [p3]
#
# Parâmetro obrigatório: 
# p1: w = Lista de parâmetros do comando waf
#     v = executar waf com a opção visual (opção --vis)
#     h = help ou mostrar todas as opções do comando run_mo809b.sh.
#         Não precisa de outros parâmetros"
#     n = modo normal. modelo de energia + harvester conectados ao nó WSN
#     s = modelo de energia conectados ao nó WSN sem o harvester
#
# Parâmetros opcionais:
#
# sem nenhum parâmetro opcional: 10 segundos de tempo de simulação
# p2: distância entre os nós em metros (opção --distanceToRx=valor)
# p3: tempo de simulação em segundos (opção --simTime=valor)

#Vai para diretorio ns-3.29
cd workspace/bake/source/ns-3.29/scratch
./run_mo809b.sh n 10 20

#EOF
