# mo809a-trabalhopratico
# Entrega de Trabalho Prático para o Curso MO809A Tópicos de Sistema Distribuído
# Professor da UNICAMP: Leandro Villas
# Aluno: Eduardo Seiti Ito, RA 159086
#
# Observação a respeito do OMNETPP
# 1. Em versões anteriores ao 5.4.1, os logs são registrados no General-#0.elog, não há informações úteis no .vec e .sca
# 2. Precisa de um script para ler as informações no *.elog. O script test3.sh é feito para isso.
# 3. É recomendável que que a versão do UBUNTU seja 18.04.2 LTS"
# 4. Ao instalar o OMNETPP, observe os parâmetros extras no arquivo config.ini, dado aqui no github. Os problemas estão lá comentados.
# 5. A partir da versão 5.4.1, as estatísticas podem ser obtidas diretamente, por duplo click, nos arquivos *.vec, *.sca e *.elog (este é obtido apenas quando se clica no botão de registro quando rodar o cenário)
#
# Observação a respeito do NS3
# Ler as instruções do script test4.sh. Basicamente, o test4.sh foi feito para evitar a repetição de se digitar nomes complicados de diretórios e parâmetros do comando trabprat.sh. É mais facil editar o arquivo test4.sh e entrar com os parâmetros necessários e rodar o testes NS3.
# Caso se queira ver o gráficos gerados pelo Python no Windows, e uma vez que está rodando no Ubuntu, recomenda-se a configuração do X11 forwarding no Putty. Caso contrário, os comandos devem ser realizados por um terminal da própria VM.
