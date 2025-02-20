#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

//Definindo pinos 
#define Led_Alerta 13
#define LedVentilador 11
#define EixoY 26 //??
#define BotaoA 5

//Definindo variáveis globais
static volatile bool modohabilitado = true;
static volatile uint32_t tempo_anterior = 0; //Para o debouncing

//Definindo parâmetros para o PWM
#define DIVISOR_CLOCK 100
#define WRAP 24999

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