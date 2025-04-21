# ControleAeronave
Repositório destinando ao desenvolvimento do Projeto Integrado, que é a primeira tarefa da fase 2 da Residência em Software Embarcado - Embarca Tech TIC 37.

 # Projeto Integrado

 O objetivo da tarefa era revisar os conceitos estudados na capacitação básica, utilizando todos os componentes aprendidos em um único projeto.<br>

__Responsável pelo desenvolvimento:__
Andressa Sousa Fonseca

## Descrição Da Tarefa 
__Integrando os componentes da BitDogLab__  <br>

__Os Componentes necessários para a execução da atividade são:__
1) Matriz 5x5 de LEDs (endereçáveis) WS2812, conectada à GPIO 7;
2) LED RGB, com os pinos conectados às GPIOs (11, 12 e 13);
3) Botão A conectado à GPIO 5;
4) Joystick;
5) Buzzer conectado ao GPIO 21;
6) Display gráfico 128x64.
   
__Uma breve explicação do desenvolvimento e resultados obtidos podem ser vistos no vídeo endereçado no seguine link: [Projeto Integrador](https://youtu.be/7CeK5zgrJB4?feature=shared).__

## Detalhamento Das Funcionalidades

O sistema desenvolvido simula um painel de controle de uma aeronave. Nele, há alertas visuais e sonoros a fim de instruir o operador sobre o estado do sistema. Além de controlar a posição da aeronave pelo joystick, sendo possível ainda visualizar a posição no display.<br>
O sistema é habilitado e desabilitado pelo Botão A. Ao pressionar o botão para iniciar o sitema, o led verde é ligado e a matriz de leds inicia uma contagem regressiva (3,2,1), indicando que o joystick ainda não está operante. Após o final da contagem, um novo alerta é emitido, um sinal de exclamação vermelho. <br>
A partir desse momento já é possível mover os joystick e o quadrado (que representa a aeronave) moverá de maneira correspondente pelo display. Outro alerta importante é o alerta sonoro quando o quadrado está nas extremidades do dispay, simbolizando que a aeronave está nos limites de alcance. E por fim, ao pressionar o botão A para desligar, o led vermelho é ligado, não há mais leituras do joystick e a matriz de leds apaga.
