# MC833 - Programação de Redes de Computadores
<br>
Nessa disciplina foram propostos 3 projetos:

1. Sockets TCP
2. Sockets UDP
3. Analise de trafego de rede


# Projeto 1 - Sockets TCP &nbsp;&nbsp;&nbsp; ![C](https://img.shields.io/badge/language-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white) ![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black) ![TCP](https://img.shields.io/badge/-tcp-%2300AEFF.svg?style=for-the-badge) 
<br>
A proposta do projeto era criar um sistema cliente/servidor, no qual o servidor armazenaria
dados de músicas e o cliente faria requisições sobre os dados dessas músicas, tendo ainda um
cliente com mais privilégios que seria capaz de, através de requisições ao servidor, cadastrar e
remover músicas que estão cadastradas no banco de dados.
<br><br>

O projeto proposto era criar um cliente e servidor que fariam a comunicação através de
sockets, utilizando-se da linguagem C, e realizando todo o desenvolvimento em um ambiente
Linux devido às funcionalidades e bibliotecas já existentes, realizando o envio e recebimento
de mensagens sob o protocolo de transmissão TCP(Transmission Control Protocol).
<br>

<img src="https://github.com/raoniton/mc833/blob/main/img/projeto1-client-comum.png" alt=""  width="70%"  />
<img src="https://github.com/raoniton/mc833/blob/main/img/projeto1-client-super.png" alt=""  width="70%"  />
<img src="https://github.com/raoniton/mc833/blob/main/img/projeto1-server.png" alt=""  width="70%"  />



# Projeto 2 - Sockets UDP/TCP &nbsp;&nbsp;&nbsp;![C](https://img.shields.io/badge/language-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white) ![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black) ![UDP](https://img.shields.io/badge/-udp-green.svg?style=for-the-badge) ![TCP](https://img.shields.io/badge/-tcp-%2300AEFF.svg?style=for-the-badge) ![MP3](https://img.shields.io/badge/-mp3-orange?style=for-the-badge) 
<br>
O projeto proposto era criar um cliente e servidor que fariam a comunicação através de sockets,
utilizando-se da linguagem C, e realizando todo o desenvolvimento em um ambiente Linux devido às
funcionalidades e bibliotecas já existentes. O o envio e recebimento de mensagens deveria ser realizado
sob o protocolo de transmissão TCP(Transmission Control Protocol) e UDP(User Datagram Protocol).
<br><br>
Haviam duas possibilidades de desenvolvimento e a opção escolhida foi a que implementaria 2
consultas sob o protocolo TCP e 1 função de transmissão de arquivo sob UDP.
Diferentemente do projeto 1 no projeto 2 seria necessário apenas 2 funções de listagem dos dados
que haviam sido salvos no projeto 1, utilizando o protocolo TCP e mais uma função de download das
músicas que foram cadastradas no projeto 1, o download do arquivo .mp3 utilizando o protocolo UDP.
<br><br>
<img src="https://github.com/raoniton/mc833/blob/main/img/projeto2-client.png" alt=""  width="70%"  />
<img src="https://github.com/raoniton/mc833/blob/main/img/projeto2-error-message.png" alt=""  width="70%"  />
<img src="https://github.com/raoniton/mc833/blob/main/img/projeto2-server.png" alt=""  width="70%"  />


# Projeto 3 - Analise de trafego de Rede &nbsp;&nbsp;&nbsp;![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=white) ![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black) ![PCAPNG Files](https://img.shields.io/badge/-pcapng-lightgray?style=for-the-badge) 
<br>
A proposta do projeto era emular uma rede contendo 4 hosts e 1 switch, os pacotes fluíram
de um host para o outro, passando através do switch. Essa rede é criada através do Mininet, que
é um emulador de redes capaz de criar redes com switches, hosts, servidores, etc. Com a rede
criada, utilizamos outro programa, o Wireshark, que é um programa de análise de pacotes, que
é capaz de capturar os pacotes de forma muito detalhada. Após capturar os pacotes, os
analisamos com auxílio de algoritmos em python.
<br>
<img src="https://github.com/raoniton/mc833/blob/main/img/projeto3-resultado-script-python.png" alt=""  width="80%"  />
<img src="https://github.com/raoniton/mc833/blob/main/img/projeto3-printscreen-parte-final-wireshark.png" alt=""  width="80%"  />


