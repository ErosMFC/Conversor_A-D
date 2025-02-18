# Conversor_A-D

Este código já faz a leitura do joystick e dos botões, além de exibir informações no display SSD1306. Agora, para atender ao enunciado da atividade, serão necessárias as seguintes modificações:

#Controle de LEDs RGB via PWM

Implementar o controle do brilho do LED Azul com base no eixo Y.
Implementar o controle do brilho do LED Vermelho com base no eixo X.
Garantir que a intensidade do LED varie suavemente com o PWM.
Movimento do Quadrado no Display

#Criar uma função para desenhar um quadrado de 8x8 pixels.
Atualizar a posição do quadrado com base nos valores do joystick.
Interrupções nos Botões

#Configurar uma interrupção para o botão do joystick (GPIO 22).
Alternar o estado do LED Verde.
Modificar a borda do display ao pressionar.
Configurar uma interrupção para o Botão A (GPIO 5).
Ativar ou desativar os LEDs RGB controlados por PWM.
Implementar debounce para evitar leituras erradas dos botões.
