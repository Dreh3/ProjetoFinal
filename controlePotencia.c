#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

//Definindo pinos 
#define Led_Alerta 13 //Simula luz de sinalização para o modo ativado
#define LedVentilador 11 //Simula o ventilador, indicando a mudança de potência 
#define EixoY 27 //Simula o sensor de temperatura
#define BotaoA 5 //Habilita ou desabilita modo de verificação de temperatura

//Definindo variáveis globais
static volatile bool modohabilitado = true; //Habilita ou desabilita modo de verificação de temperatura
static volatile uint32_t tempo_anterior = 0; //Para o debouncing

//Definindo parâmetros para o PWM
#define DIVISOR_CLOCK 100
#define WRAP 24999

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
void modifica_velocidade(){
    
};


static void interrupcaoBotao(uint gpio, uint32_t events);

int main()
{
    stdio_init_all();

    //Chamada da interrupção para o botão
    gpio_set_irq_enabled_with_callback(BotaoA, GPIO_IRQ_EDGE_FALL, true, interrupcaoBotao);

    while (true) {
        
    }
}


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