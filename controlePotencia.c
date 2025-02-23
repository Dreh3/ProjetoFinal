//Código desenvolvido por Andressa Sousa Fonseca

/*
No presente código foi utilizado o joystick integrado à BitDogLab para simular oscilações de 
temperatura do ambiente, que modificam a potência do led, que simula um motor, de acordo
com faixas prè definidas. Assim, o led pode desligar ou variar entre 3 níveis de potência distintos.
Ademais, pode manter estável sem realizar novas leituras ao pressionar o Botão A, que habilita/desabilita 
o modo de leitura.
*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

//Definindo pinos 
#define LedVentilador 11 //Simula o ventilador, indicando a mudança de potência 
#define SensorY 27 //Simula o sensor de temperatura no eixo Y do joystick
#define BotaoA 5 //Habilita ou desabilita modo de verificação de temperatura

//Definindo variáveis globais
static volatile bool modohabilitado = true; //Habilita ou desabilita modo de verificação de temperatura
static volatile uint32_t tempo_anterior = 0; //Para o debouncing

//Definindo parâmetros para o PWM
#define DIVISOR_CLOCK 100   
#define WRAP 24999          //Valor do wrap que corresponde ao divisor escolhido

//Definindo parâmetros de potência
#define nivel0 0        //Valor do level para desligar
#define nivel1 5000     //Valor do level para potência mínima
#define nivel2  15000   //Valor do level para potência média
#define nivel3  24900   //Valor do level para potência máxima

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
    pwm_set_enabled(slice, true);
};

//Função para mudar a potência suavemente
void modifica_potencia(uint gpio, uint nova_potencia, uint potencia_atual){
    bool incrementa = (nova_potencia>=potencia_atual)?true:false;   //Verifica se haverá incremento ou decremento da potência
    while(potencia_atual!=nova_potencia){       //Repete enquanto não chegar na potência desejada
        if(incrementa){
            potencia_atual += 100;              //Incrmenta a potência
        }else{
            potencia_atual -= 100;              //Decrementa a potência
        };
        pwm_set_gpio_level(gpio, potencia_atual);   //Atualiza o level
        sleep_ms(10);                               //Suaviza a variação
    };
};

//Protótipo da função de interrrupção
static void interrupcaoBotao(uint gpio, uint32_t events);

int main()
{
    uint nivel_atual = nivel0;          //Variável que armazena o valor de nível atual
    uint nivel_anterior = nivel0;       //Variável que armazena o valor de nível anterior
    uint temperatura = 20;              //Armazena o valor de temperatura lido

    adc_init(); //Inicializa as configurações necessárias para usar o conversor a/d

    //Inicializando entradas analógicas
    adc_gpio_init(SensorY);

    configurar_botao(BotaoA);   //Chama a função que inicializa e configura o botão A
    configurar_pwm(LedVentilador); //Chama a função que define o led como saída pwm

    //Chamada da interrupção para o botão
    gpio_set_irq_enabled_with_callback(BotaoA, GPIO_IRQ_EDGE_FALL, true, interrupcaoBotao);

    while (true) {

        if(modohabilitado){

            //Leitura do eixo Y para pegar dados analógicos que simulam o sensor de temperatura
            //Relação que converte o valor lido na entrada analalógica para temperatura na faixa de 20° a 35°
            adc_select_input(0);                            //Seleciona o canal para leitura
            temperatura = (15*adc_read() + 20 * 4096)/4096; //Faz a leitura e transforma o valor em medida temperatura

            //Condicionais para identificar os intervalos correpondentes às potências
            if(temperatura<=20){                    //Se for menor ou igual a 20°
                nivel_anterior = nivel_atual;       //Armazena novo valor do anterior
                nivel_atual = nivel0;               //Modifica o valor do nível atual
            }else if(temperatura>20 && temperatura<=25){ //Se for maior que 20° e menor ou igual a 25° 
                nivel_anterior = nivel_atual;       //Armazena novo valor do anterior
                nivel_atual = nivel1;               //Modifica o valor do nível atual
            }else if(temperatura>25 && temperatura<=30){    //Se for maior que 25° e menor ou igual a 30°
                nivel_anterior = nivel_atual;       //Armazena novo valor do anterior
                nivel_atual = nivel2;               //Modifica o valor do nível atual
            }else if(temperatura>30){               //Se for maior que 30°
                nivel_anterior = nivel_atual;       //Armazena novo valor do anterior
                nivel_atual = nivel3;               //Modifica o valor do nível atual
            };
            modifica_potencia(LedVentilador, nivel_atual, nivel_anterior);  //Chama a função passando os parâmetrs atualizados
        };
        sleep_ms(10);       //Pequena pausa para novas leituras
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
