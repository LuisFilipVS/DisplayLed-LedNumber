## **Display Led + Matriz Led com Raspberry Pi Pico**

Autor: Luis Filipe Vieira Silva

---

## **Funcionalidades**

Funcionalidades do Projeto:
1. O LED RGB pisca continuamente com um efeito visual.
2. O botão A (GPIO 6) decrementa o número exibido na matriz de LEDs.
3. O botão B (GPIO 5) incrementa o número exibido na matriz de LEDs.
4. A matriz de LEDs WS2812 é utilizada para representar os números de 0 a 9, e os LEDs acendem de acordo com o número selecionado.
5. O número selecionado é exibido em um display OLED I2C utilizando a biblioteca SSD1306.
6. O sistema é projetado com a utilização de interrupções para a detecção de cliques nos botões, evitando a necessidade de polling contínuo.

---

## **Observações**
1. **Uso de interrupções**: Todas as funcionalidades relacionadas aos botões foram implementadas utilizando rotinas de interrupção (IRQ), o que garante a resposta imediata aos cliques, sem a necessidade de aguardar constantemente em um loop principal.
2. **Debouncing**: Foi tratado o problema de *bouncing* dos botões, com um atraso de 200 ms entre as leituras de pressionamento para evitar múltiplos disparos da interrupção para um único clique.
3. **Integração de componentes**: A interação entre os LEDs WS2812 e o display OLED foi realizada de forma sincronizada, proporcionando uma interface visual clara com a exibição do número no display e a iluminação correspondente na matriz de LEDs.
4. **Exibição de números**: Os números de 0 a 9 são exibidos na matriz de LEDs através de uma representação binária, onde os LEDs são acesos ou apagados conforme a posição correspondente ao número.
5. **Bibliotecas utilizadas**: O projeto utiliza bibliotecas específicas para o controle de LEDs WS2812 (via PIO) e o display OLED SSD1306 via I2C, ambas com configuração de comunicação de alta velocidade.
6. **Fontes e exibição**: A função `ssd1306_draw_string` é utilizada para mostrar os números no display OLED, e o número correspondente é alterado conforme a interação com os botões.

---

## **Hardware Utilizado**

1. **Raspberry Pi Pico** – Controlador principal do sistema.
2. **Matriz de LEDs WS2812** – Usada para exibir números.
3. **Display OLED SSD1306** – Utilizado para mostrar o número em tempo real.
4. **Botões de Entrada (GPIO 5 e GPIO 6)** – Usados para incrementar e decrementar o número exibido.

---

## **Referências**

- [Pico SDK - Documentação oficial](https://www.raspberrypi.org/documentation/pico/getting-started/)
- [WS2812B PIO Driver - GitHub](https://github.com/earlephilhower/arduino-pico)
- [SSD1306 OLED Display - GitHub](https://github.com/olikraus/SSD1306)
