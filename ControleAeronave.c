//Código desenvolvido por Andressa Sousa Fonseca

/*O presente código simula o painel de controle de uma aeronave, um drone, e emite alertas sobre o estado
do sistema, que podem ser visuais ou sonoros. Além disso, permite controlar a posição do drone pelo joystick, 
sendo visualizado no display no formato de um quadrado 8x8
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/timer.h" //Deboucing e interrupções

//Configurações para o displey
#include "hardware/i2c.h" //Biblioteca para comunicação i2c
#include "lib/ssd1306.h"
#include "lib/image.h"
#define I2C_PORT i2c1
#define I2C_SDA 14 //pinos para comunicação I2C
#define I2C_SCL 15 //pinos para comunicação I2C
#define endereco 0x3C
ssd1306_t ssd; // Inicializa a estrutura do display

//Arquivo .pio
#include "pio_matrix.pio.h"
#define IS_RGBW false 
#define MatrizLeds 7 //Pino para matriz de leds

PIO pio = pio0;
int sm = 0;
            //Definindo pinos
//Pinos RGB
#define LedGreen 11
#define LedBlue 12
#define LedRed 13
//Pinos Joystick
#define EixoX 26
#define EixoY 27
//Botão
#define BotaoA 5
//Buzzer
#define Buzzer 21

            //Definindo variáveis globais
static volatile bool modohabilitado = false; //Habilita ou desabilita modo de controle (inicia desligado)
static volatile uint32_t tempo_anterior = 0; //Para o debouncing
static volatile uint tipo_matriz = 3; //Variável para controlar a matriz de leds 5x5

            //Fução para configurar leds RGB
void config_leds(){
    gpio_init(LedBlue);
    gpio_init(LedGreen);
    gpio_init(LedRed);
    gpio_set_dir(LedBlue, GPIO_OUT);
    gpio_set_dir(LedGreen, GPIO_OUT);
    gpio_set_dir(LedRed, GPIO_OUT);
    gpio_put(LedBlue,0);
    gpio_put(LedGreen,0);
    gpio_put(LedRed,0);
};

        //Função para configurar o botão
void config_botao(){
    gpio_init(BotaoA);
    gpio_set_dir(BotaoA, GPIO_IN);
    gpio_pull_up(BotaoA);
};

void config_buzzer(){
    uint slice;
    gpio_set_function(Buzzer, GPIO_FUNC_PWM); //Configura pino do led como pwm
    slice = pwm_gpio_to_slice_num(Buzzer); //Adiquire o slice do pino
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (4400 * 4096));
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(Buzzer, 0); //Determina com o level desejado
};

//Configurações para a matriz de Leds

//Definindo struct para cores personalizadas
typedef struct {
    double red;
    double green;
    double blue;
}Led_RGB;

//Definindo tipo Cor
typedef Led_RGB COR_RGB;

// Definição de tipo da matriz de leds
typedef Led_RGB Matriz_leds[5][5];

//Retorno o valor binário para a cor passada por parâmetro
uint32_t cor_binario (double b, double r, double g)
{
  unsigned char R, G, B;
  R = r * 255; 
  G = g * 255;  
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
};

//Função responsável por acender os leds desejados 
void acender_leds(Matriz_leds matriz){
    //Primeiro for para percorrer cada linha
    for (int linha =4;linha>=0;linha--){
        
        /*
        Devido à ordem de disposição dos leds na matriz de leds 5X5, é necessário
        ter o cuidado para imprimir o desenho na orientação correta. Assim, o if abaixo permite o 
        desenho saia extamente como projetado.
        */

        if(linha%2){                             //Se verdadeiro, a numeração das colunas começa em 4 e decrementam
            for(int coluna=0;coluna<5;coluna++){
                uint32_t cor = cor_binario(matriz[linha][coluna].blue,matriz[linha][coluna].red,matriz[linha][coluna].green);
                pio_sm_put_blocking(pio, sm, cor);
            };
        }else{                                      //Se falso, a numeração das colunas começa em 0 e incrementam
            for(int coluna=4;coluna>=0;coluna--){
                uint32_t cor = cor_binario(matriz[linha][coluna].blue,matriz[linha][coluna].red,matriz[linha][coluna].green);
                pio_sm_put_blocking(pio, sm, cor);
            };
        };
    };
};

//Padrão para inicialização com numeros
void alertas_Matrix(){

    //Declarando variáveis necessárias para modificar intensidade
    int inten =4, inten2 =2;
    //Declrando as cores persanalizadas
    COR_RGB apagado = {0.0,0.0,0.0};
    COR_RGB azulForte = {0.0,0.0,0.5*inten};
    COR_RGB azulClaro = {0.0,0.0,0.2*inten2};
    COR_RGB magentaForte = {0.5*inten,0.0,0.5*inten};
    COR_RGB vermelhoForte = {0.5*inten,0.0,0.0};

    //Matriz que mostrará o um 
    Matriz_leds um =
        {{apagado, apagado, azulClaro, apagado, apagado},{apagado, azulClaro, azulClaro,apagado, apagado},{apagado, apagado, azulClaro,apagado, apagado},{apagado, apagado, azulClaro,apagado, apagado},{azulClaro, azulClaro, azulClaro, azulClaro,azulClaro}};
    //Matriz que mostrará o dois 
    Matriz_leds dois =
        {{apagado, magentaForte, magentaForte, magentaForte, apagado},{magentaForte, apagado, apagado,apagado, magentaForte},{apagado, apagado, magentaForte,magentaForte, apagado},{apagado, magentaForte, apagado,apagado, apagado},{magentaForte, magentaForte, magentaForte, magentaForte,magentaForte}};
    //Matriz que mostrará o três 
    Matriz_leds tres =
        {{apagado, azulForte, azulForte, azulForte, apagado},{azulForte, apagado, apagado,apagado, azulForte},{apagado, apagado, azulForte,azulForte, apagado},{azulForte, apagado, apagado,apagado, azulForte},{apagado, azulForte, azulForte, azulForte,apagado}};
    //Matriz que mostrará todos os leds apagados
    Matriz_leds limpar = 
        {{apagado,apagado,apagado,apagado,apagado},{apagado,apagado,apagado,apagado,apagado},{apagado,apagado,apagado,apagado,apagado},{apagado,apagado,apagado,apagado,apagado},{apagado,apagado,apagado,apagado,apagado}};
    //Matriz que mostrará o sinal de alerta
    Matriz_leds alerta = 
        {{apagado,apagado,vermelhoForte,apagado,apagado},{apagado,apagado,vermelhoForte,apagado,apagado},{apagado,apagado,vermelhoForte,apagado,apagado},{apagado,apagado,apagado,apagado,apagado},{apagado,apagado,vermelhoForte,apagado,apagado}};
    
    if (tipo_matriz==1){ //O tipo 1 mostra a contagem regressiva
        acender_leds(tres);
        sleep_ms(2000);
        acender_leds(limpar);
        sleep_ms(500);
        acender_leds(dois);
        sleep_ms(2000);
        acender_leds(limpar);
        sleep_ms(500);
        acender_leds(um);
        sleep_ms(2000);
        acender_leds(limpar);
        tipo_matriz=2;
        printf("Sistema iniciado!\n");
    }else if(tipo_matriz==2){ //O tipo 2 mostra o alerta
        acender_leds(alerta);
    }else if(tipo_matriz==3){ //O tipo 3 apaga a matriz 
        acender_leds(limpar);
    };
};

        //Função para alternar os leds RGB de acordo com as ações do joystick
void alertas_RGB(uint gpio){
    switch (gpio)
    {
    case LedGreen: //Desliga o led vermelho
        gpio_put(LedRed,0);
        break;
    case LedRed: //Desliga o led azul
        gpio_put(LedGreen,0);
        break;
    default:
        break;
    };
    gpio_put(gpio,1); //Liga o led desejado
};

            //Protótipo da função de interrupção
void InterrupcaoBotao(uint gpio, uint32_t events);

int main()
{

    //Declarando veriáveis necessárias
    uint16_t valor_atualX; //Valores do eixo x
    uint16_t valor_atualY; //Valores do eixo y
    uint linhaX = 60; //Armazenará valores de posição no eixo x do display
    uint linhaY=55; //Armazenará valores de posição no eixo y do display


    stdio_init_all(); //Inicia as comunicações padrões
    adc_init(); //Inicializa o adc
    config_leds(); //Chama a função para configurar os leds
    config_botao(); //Chama a função para configurar o botão
    config_buzzer(); //Chama a função para configurar o buzzer com pwm

    //Inicializando entradas analógicas
    adc_gpio_init(EixoX); 
    adc_gpio_init(EixoY);

    //Configurações para matriz de leds
    uint offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, MatrizLeds, 800000, IS_RGBW);

    // Inicializando comunicação I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura pino SDA para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura pino SCL para I2C

    // Inicializa o display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display
     
    bool cor = true;    //Controla cor do display para o fundo contrastar com letra
    cor = !cor;
    ssd1306_draw_image(&ssd, 'Q', 60, 55); //Inicia com o quadrado no centro inferior
    ssd1306_send_data(&ssd); // Atualiza o display


    //Chamada da interrupção para o botão
    gpio_set_irq_enabled_with_callback(BotaoA, GPIO_IRQ_EDGE_FALL, true, &InterrupcaoBotao);


    while (true) {

        if(tipo_matriz==1){ //Se o sistema tiver sido desliagado e ligado novamente, o quadrado volta para a posição de retorno
            //Atualizando display
            ssd1306_fill(&ssd, cor); // Limpa o display
            ssd1306_draw_image(&ssd, 'Q', 60, 55); //Posiciona o quadrado no lugar desejado
            ssd1306_send_data(&ssd); // Atualiza o display
        };
        
        //Chamada da função que emite os alertas na matriz de leds 5X5
        alertas_Matrix();

        //As leituras do joystick só serão feitas se o sistema estiver habilitado
        if(modohabilitado){
            //Selecionando o canal e lendo o valor correpondente
            adc_select_input(1);
            valor_atualX = abs(adc_read() - 4096);
            adc_select_input(0);
            valor_atualY = adc_read();
            //printf("%d\n",valor_atualY);

            //Converte os valores adc para posições correpondentes no display
            valor_atualX = (120*4096 - (120*valor_atualX))/4096;
            valor_atualY = (56*4096 - (56*valor_atualY))/4096;

            //O valor dois somado aos valores no if é uma margem de segurança para maior estabilidade nas leituras do joystick

            if(valor_atualX>60+2){ //Se o valor de X lido for maior que 60 significa que o joystick está sendo movido para a esquerda
                if(linhaX==119) //Se o valor de linhaX já é 119, não é possível incrementar, pois ultrapassa a borda do display
                    linhaX=119;
                else
                    linhaX++; //Incrementa a posição no eixo X
            }else if(valor_atualX<60-2){ //Se o valor de X for menor que 60 significa que o 
                if(linhaX==0) //Se o valor de X já é 0 não é possível decrementar mais
                    linhaX=0;
                else
                    linhaX--; //Decrementa para a posição desejada
            };

            if(valor_atualY>29+2){
                if(linhaY==56)
                    linhaY=56;
                else
                    linhaY++;
            }else if(valor_atualY<29-2){
                if(linhaY==0)
                    linhaY=0;
                else
                    linhaY--;
            };

            if(linhaX==119 || linhaX==0 || linhaY==0 || linhaY==56){ //Verifica se o quadrado está posicionado nos limites do display
                printf("Limites ultrapassados!\n"); //Mensagem de alerta
                pwm_set_gpio_level(Buzzer, 32768); //Emite o aviso sonoro
            }else{
                pwm_set_gpio_level(Buzzer, 0); //Desliga o buzzer quando está dentro dos limites
            };

            //Atualizando display
            ssd1306_fill(&ssd, cor); // Limpa o display
            ssd1306_draw_image(&ssd, 'Q', linhaX, linhaY); //Posiciona o quadrado no lugar desejado
            ssd1306_send_data(&ssd); // Atualiza o display
        };
    };
};

//Função chamada na interrupção do botão

void InterrupcaoBotao(uint gpio, uint32_t events){

    uint32_t tempo_atual = to_us_since_boot(get_absolute_time()); //Armazena na variável o tempo atual em us

    if(tempo_atual - tempo_anterior > 300000){ //Admite que o botão foi pressionado se tiver uma diferença maior que 300ms
        
        tempo_anterior = tempo_atual; //Atualiza a referência de tempo
        
        if(modohabilitado){ //Se estiver habilitado
            modohabilitado = !modohabilitado; //Passará a estar desabilitado
            alertas_RGB(LedRed); //O Led vermelho liga indicando que o sistema está desligado
            tipo_matriz=3; //A matriz de leds 5x5 fica apagada
            pwm_set_gpio_level(Buzzer, 0); //Desliga o buzzer
        }else{
            modohabilitado = !modohabilitado; //O sistema estava desabilitado e aqui passa a estar habilitado
            printf("Sistema habilitado!Aguarde inicialização...\n"); //Mensagem para informar o estado do sistema
            alertas_RGB(LedGreen); //Led verde liga para indicar que o sistema está operante
            tipo_matriz = 1; //Mostra na matriz de leds a contagem regressiva
        };

    };
};