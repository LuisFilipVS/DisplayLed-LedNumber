#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812b.pio.h"

#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13
#define LED_COUNT 25
#define MATRIZ_LED_PIN 7
#define BUTTON_PIN_0 5 //Botao B DECREMENT
#define BUTTON_PIN_1 6 //Botao A INCREMENT

static int INCREMENT = 0;

// Estrutura com os dados de cor e luminozidade para um led
typedef struct{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} led;

led MATRIZ_LED[LED_COUNT];


static volatile uint a = 1;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

PIO pio = pio0;
uint offset;
uint sm;

// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);

//Array que reprensenta cada numero na matriz de led 5x5
const uint8_t numbers_indices[10][25] = {
    {23,22,21,18,11,8,1,16,13,6,3,2}, // Número 0
    {22,17,12,7,2}, // Número 1
    {23,22,21,18,11,12,13,6,3,2,1}, // Número 2
    {23,22,21,18,11,12,13,8,3,2,1}, // Número 3
    {23,21,16,18,13,12,11,8,1}, // Número 4
    {23,22,21,16,13,12,11,8,3,2,1}, // Número 5
    {23,22,21,16,13,12,11,8,6,3,2,1}, // Número 6
    {23,22,21,18,11,8,1}, // Número 7
    {23,22,21,18,11,8,1,16,13,12,6,3,2}, // Número 8
    {23,22,21,18,11,8,1,16,13,12} // Número 9
};

// Array que armazena os dados dos 25 leds
led matriz_led[LED_COUNT];

// Função que junta 3 bytes com informações de cores e mais 1 byte vazio para temporização
uint32_t valor_rgb(uint8_t B, uint8_t R, uint8_t G){
  return (G << 24) | (R << 16) | (B << 8);
};

//Usado para configurar cada led dentro da matriz de led.
void set_led(uint8_t indice, uint8_t r, uint8_t g, uint8_t b){
    if(indice < 25){
    matriz_led[indice].R = r;
    matriz_led[indice].G = g;
    matriz_led[indice].B = b;
    }
};

//Função recebe um numero de 0 a 9 e configura o mesmo para ser mostrado na matriz de led. 
void config_number_led(int number){
    int size = sizeof(numbers_indices[number]) / sizeof(numbers_indices[number][0]);

    for(int i = 0; i < size; i++ ){
        
            if(numbers_indices[number][i] == 0){
                break;
            }
            set_led(numbers_indices[number][i],0,1,0);
        }
    };

//Função usada para desligar todos os leds
void clear_leds(){
    for(uint8_t i = 0; i < LED_COUNT; i++){
        matriz_led[i].R = 0;
        matriz_led[i].B = 0;
        matriz_led[i].G = 0;
    }
};

// Função que envia os dados do array para os leds via PIO
void print_leds(PIO pio, uint sm){
    uint32_t valor;
    for(uint8_t i = 0; i < LED_COUNT; i++){
        valor = valor_rgb(matriz_led[i].B, matriz_led[i].R,matriz_led[i].G);
        pio_sm_put_blocking(pio, sm, valor);
    }
};

//Função criada para organizar o código, configurando as portas GPIO
void configurarPortas(){
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    gpio_init(BUTTON_PIN_0);
    gpio_set_dir(BUTTON_PIN_0, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_0);

    gpio_init(BUTTON_PIN_1);
    gpio_set_dir(BUTTON_PIN_1, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_1);

};

//Função que utiliza a porta GPIO 11 para ligar o led RGB e piscá-lo 5x por segundo.
void piscarLed(uint portaLED){
    gpio_put(LED_RED, 1);
    sleep_ms(20);
    gpio_put(LED_RED, 0);
    sleep_ms(20);
};

//Função usada para incrementar ou decrementar o número do LED baseado no clique dos botões
void update_number_led(int number, PIO pio, uint sm){
    clear_leds();
    printf("%d", number);
    
    if (number == 1){
        INCREMENT += 1;
    }

    if (number == 0){
        INCREMENT -= 1;
    }

    if(INCREMENT < 0){
        INCREMENT = 9;
    }
    
    int var = INCREMENT % 10;
    config_number_led(var);
    print_leds(pio,sm);
}

void updateLed(uint PORTA_GPIO){
    gpio_put(PORTA_GPIO, !gpio_get(PORTA_GPIO));
}

//Função que opera a interrupção criada por meio do clique de algum dos botões
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento

    if(gpio == BUTTON_PIN_1){
        if (current_time - last_time > 200000) // 200 ms de debouncing
        {
            last_time = current_time; // Atualiza o tempo do último evento
            //update_number_led(1, pio, sm);
            updateLed(LED_BLUE);
        }
    } else if (gpio == BUTTON_PIN_0){
            if (current_time - last_time > 200000) // 200 ms de debouncing
        {
            last_time = current_time; // Atualiza o tempo do último evento
            //update_number_led(0, pio, sm);
            updateLed(LED_GREEN);
        }
    }
}


bool verificar_numero(char caractere){
    bool ehNumero = false;
    int numeros[10] = {0,1,2,3,4,5,6,7,8,9};
    
    for(int i=0; i < 10; i++){
        if(numeros[i] == caractere){
            ehNumero = true;
            break;
        }
    }

    return ehNumero;

}

//Função principal
int main()
{   
    stdio_init_all();
    configurarPortas();

    char input;

    pio = pio0; 
    bool ok = set_sys_clock_khz(128000, false);
    offset = pio_add_program(pio, &ws2812b_program);
    sm = pio_claim_unused_sm(pio, true);
    ws2812b_program_init(pio, sm, offset, MATRIZ_LED_PIN);
    
    config_number_led(0);
    print_leds(pio,sm);

    //_____________________________Serial Display__________________
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    ssd1306_t ssd; // Inicializa a estrutura do display

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    bool cor = true;
    
    //_________________________________________________________________________________

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_1, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        printf("Insira um caractere \n");
        scanf("%c", &input);
        //while (getchar() != '\n');

        char texto[2];  // Um espaço para o caractere e um para o terminador nulo
        texto[0] = input;
        texto[1] = '\0';  // Adiciona o terminador de string

        ssd1306_fill(&ssd, !cor); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo


        printf("Numero inserido: %c\n", input);
        if (verificar_numero(texto[0] - '0')) { 
            ssd1306_draw_string(&ssd, texto, 8, 10);
            clear_leds();
            printf("Gerando Led com numero");
            config_number_led(texto[0] - '0');  
            print_leds(pio, sm);
        } else {
            ssd1306_draw_string(&ssd, texto, 8, 10);
        }

        ssd1306_send_data(&ssd); // Atualiza o display
        sleep_ms(1000);
    }
};



