#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

//Definindo pinos 
//#define Led_Alerta 13 //Simula luz de sinalização para o modo ativado
#define LedVentilador 11 //Simula o ventilador, indicando a mudança de potência 
#define SensorY 27 //Simula o sensor de temperatura no eixo Y do joystick
#define BotaoA 5 //Habilita ou desabilita modo de verificação de temperatura

//Definindo variáveis globais
static volatile bool modohabilitado = true; //Habilita ou desabilita modo de verificação de temperatura
static volatile uint32_t tempo_anterior = 0; //Para o debouncing
uint potencia_atual = 0; //Inicia desligado

//Definindo parâmetros para o PWM
#define DIVISOR_CLOCK 100
#define WRAP 24999

//Definindo parâmetros de potência
#define nivel0 0
#define nivel1 500
#define nivel2  1500
#define nivel3  24900

//Funções para configurar os pinos de leds e botões
void configurar_leds(uint gpio){
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 0);
};

void configurar_botao(uint gpio){
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
};

//Função para configurar pwm
void configurar_pwm(uint gpio){
    uint slice;
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv(slice, DIVISOR_CLOCK);
    pwm_set_wrap(slice,WRAP);
    pwm_set_gpio_level(gpio, 0);
    pwm_set_enabled(gpio, true);
};

//Função para mudar a potência suavemente
void modifica_potencia(uint gpio, uint nova_potencia, uint potencia_atual){
    bool incrementa = (nova_potencia>potencia_atual)?true:false;
    while(potencia_atual!=nova_potencia){
        if(incrementa){
            potencia_atual += 100;
        }else{
            potencia_atual -= 100;
        };
        pwm_set_gpio_level(gpio, potencia_atual);
        sleep_ms(10);
    };
};


static void interrupcaoBotao(uint gpio, uint32_t events);

int main()
{
    uint nivel_atual = nivel0;
    uint nivel_anterior = nivel0;
    uint temperatura = 20;

    stdio_init_all();
    adc_init();

     //Inicializando entradas analógicas
     adc_gpio_init(SensorY);

    configurar_leds(LedVentilador); //Já inicia desligado
    //configurar_leds(LedAlerta);
    configurar_botao(BotaoA);
    configurar_pwm(LedVentilador);

    //Chamada da interrupção para o botão
    gpio_set_irq_enabled_with_callback(BotaoA, GPIO_IRQ_EDGE_FALL, true, interrupcaoBotao);

    while (true) {

        if(modohabilitado){
            //Leitura do eixo Y para pegar dados analógicos que simulam o sensor de temperatura
            //Relação que converte o valor lido na entrada analalógica para temperatura na faixa de 20° a 35°
            adc_select_input(0);
            temperatura = (15*adc_read() + 20 * 4096)/4096;

            //Condicionais para identificar os intervalos correpondentes às potências
            if(temperatura<=20){
                nivel_anterior = nivel_atual;
                nivel_atual = nivel0;
            }else if(temperatura>20 && temperatura<=25){
                nivel_anterior = nivel_atual;
                nivel_atual = nivel1;
            }else if(temperatura>25 && temperatura<=30){
                nivel_anterior = nivel_atual;
                nivel_atual = nivel2;
            }else if(temperatura>30){
                nivel_anterior = nivel_atual;
                nivel_atual = nivel3;
            };
            modifica_potencia(LedVentilador, nivel_atual, nivel_anterior);
        };
    };
};


//Função de interrupção para habilitar/desabilita funcionalidade a partir do Botão A
void interrupcaoBotao(uint gpio, uint32_t events){
    uint32_t tempo_atual = 0; //Variável para o deboucing

    tempo_atual = to_us_since_boot(get_absolute_time()); //Salva o tempo atual

    if(tempo_atual-tempo_anterior >= 300000){ //Aguarda um tempo de 300ms para considerar o botão acionado
        tempo_anterior = tempo_atual;           //Armazena o novo tempo anterior
        if(gpio==BotaoA){
            modohabilitado = !modohabilitado; //Inverte o valor da variável
        };
    };

};