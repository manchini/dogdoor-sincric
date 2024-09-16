# Dog Door Automation with Sinric Pro

Este projeto tem como objetivo automatizar a porta do cachorro, permitindo abrir e fechar remotamente via Google Home. Utilizando a integração com a plataforma Sinric Pro, o sistema possibilita o controle por comandos de voz ou por meio de dispositivos conectados.

## Funcionalidades

- Controle remoto da porta do cachorro através do Google Home.
- Integração com Sinric Pro para comunicação entre o dispositivo e a assistente virtual.
- Utilização de um servo motor para abrir e fechar a porta de 30 cm x 30 cm, com aproximadamente 2 kg.

## Tecnologias Utilizadas

- **Sinric Pro**: Gerenciamento da comunicação entre o microcontrolador e os dispositivos conectados.
- **Arduino IDE**: Desenvolvimento do código de controle da porta.
- **ESP8266/ESP32**: Microcontrolador responsável pela automação.
- **Servo Motor**: Mecanismo utilizado para mover a porta.

## Como Funciona

1. O código utiliza a biblioteca do Sinric Pro para estabelecer uma conexão com a internet e permitir a integração com o Google Home.
2. Ao receber um comando para abrir ou fechar a porta, o servo motor é acionado para movimentar a estrutura.
3. A porta pode ser controlada remotamente por comandos de voz ou dispositivos compatíveis com Sinric Pro, como o Google Home.
