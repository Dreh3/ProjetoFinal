# Controle de Potência por Temperatura

__Projeto Final da Capacitação em Sistemas Embarcados__<br>
Repositório foi criado com o intuito de desenvolver um projeto prático sobre sistemas embarcados, aplicando os conhecimentos adquiridos ao longo do curso. O sistema desenvolvido foi feito para monitorar as temperaturas do ambiente e ajustar a potência entregue a um ventilador.

__Responsável pelo desenvolvimento:__
Andressa Sousa Fonseca

__Uma breve explicação sobre o desenvolvimento e resultados obtidos podem ser vistos no vídeo endereçado no seguinte link: [Controle de PotÊncia por Temperatura](https://youtu.be/GN4ztbMzv6M?si=sReb9UBP103bOldQ).__

## Detalhamento do Projeto

__Os Componentes Utilizados:__
1) LED RGB Verde, com o pino conectados à GPIO 11.
2) Joystick conectado aos GPIOs 27 (Eixo Y).
4) Botão A conectado à GPIO 5.

## Detalhamento Do Projeto

### 1. Funcionalidade do Sistema

No presente código utilizado o joystick integrado à BitDogLab para simular oscilações de temperatura do ambiente, que modificam a potência do led, que simula um motor, de acordo
com faixas pré definidas. Assim, o led pode desligar ou variar entre 3 níveis de potência distintos. Ademais, pode-se manter estável sem realizar novas leituras ao pressionar o Botão A, que habilita/desabilita o modo de leitura. As faixas e as potências correspondentes estão listadas abaixo: <br>
1. Temperaturas menores que 20° desligam o ventilador(led).
2. Temperaturas maiores que 20° e menores ou iguais a 25° entregam a potência mínima ao led.
3. Temperaturas maiores que 25° e menores ou iguais a 30° entregam a potência média ao led.
4. Temperaturas maiores que 30° entregam a potência máxima ao led.

### 2. Código desenvolvido
<br>
1. Configurando os componentes utilizados <br><br>
O led Verde no pino GPIO 11 foi configurado como saída pwm com as seguintes definições:

```bash
uint slice;
gpio_set_function(gpio, GPIO_FUNC_PWM);
slice = pwm_gpio_to_slice_num(gpio);
pwm_set_clkdiv(slice, DIVISOR_CLOCK);
pwm_set_wrap(slice,WRAP);
pwm_set_gpio_level(gpio, 0);
pwm_set_enabled(slice, true);
```
<br>O botão A no pino GPIO 5 foi configurado como entrada com resistor pull up da seguinte forma: 

```bash
gpio_init(gpio);
gpio_set_dir(gpio, GPIO_IN);
gpio_pull_up(gpio);
```

<br>Por último, para utilizar os dados do eixo Y do joystick no pino GPIO 27, a seguinte função foi necessária:
```bash
adc_gpio_init(SensorY);
```
<br>
2. Relacionando valores lido à temperatura <br><br>
A leitura do eixo do joystick retorna valores de 0 a 4096. Assim, para relacionar esses valores à temperatura em graus Celsius de 20° a 35° foi necessário realizar um cálculo de função que resultou na seguinte relação usada no código: 

```bash
temperatura = (15*adc_read() + 20 * 4096)/4096;
```
<br>
3. Variando a potência <br><br>
Depois que a temperatura é lida e a faixa é identificada, como especificado acima, uma função é chamada para variar entre a potência atual a a nova potência corresponde à mudança de temperatura. A função chamada é:

```bash
modifica_potencia(LedVentilador, nivel_atual, nivel_anterior);
```
Essa função recebe como parâmetros o pino do led, o nível_atual que corresponde à nova potência e o o nível_anterior que corresponde à potência entregue ao led no momento que será modificada.<br>
Assim, a potêcia incrementa ou decrementa com um passo de level de 100 para que a mudança seja suave.
