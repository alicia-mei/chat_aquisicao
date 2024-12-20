#include <stdio.h>

#define SIM_SCGC6_ADC0 (1 << 27) // Habilita o clock do ADC0
#define SIM_SCGC5_PORTB (1 << 10) // Habilita o clock do PORTB
#define ADC0_BASE (0x4003B000)    // Endereço base do ADC0
#define SIM_BASE (0x40048000)     // Endereço base do SIM
#define PORTB_BASE (0x4004A000)    // Endereço base do PORTB

// Configurações do ADC
#define ADC_CFG1_ADICLK (0x00)    // Clock do ADC
#define ADC_CFG1_MODE (0x01)       // 12-bit resolution
#define ADC_SC1_ADCH(channel) (channel) // Seleciona o canal
#define PORT_PCR_MUX (0x07 << 8) // MUX configurado no PCR

void adc_init(void) {
    // Habilita o clock do PORTB para usar PTB0
    *((volatile unsigned int *)(SIM_BASE + 0x38)) |= SIM_SCGC5_PORTB; // SIM->SCGC5

    // Habilita o clock do ADC0
    *((volatile unsigned int *)(SIM_BASE + 0x3C)) |= SIM_SCGC6_ADC0; // SIM->SCGC6

    // Configura o MUX do pino PTB0 para ADC (MUX = 0x00)
    *((volatile unsigned int *)(PORTB_BASE + 0x00)) |= ~PORT_PCR_MUX; // PORTB->PCR[0]

    // Configura o ADC (CFG1)
    *((volatile unsigned int *)(ADC0_BASE + 0x08)) = ADC_CFG1_ADICLK | ADC_CFG1_MODE; // ADC0->CFG1
}

unsigned int adc_read(unsigned char channel) {
    // Inicia a conversão no canal especificado
    *((volatile unsigned int *)(ADC0_BASE + 0x04)) = ADC_SC1_ADCH(channel); // ADC0->SC1[0]
    
    // Espera até que a conversão esteja completa
    while ((*((volatile unsigned int *)(ADC0_BASE + 0x1C)) & (1 << 7)) == 0); // ADC0->SC1[0] -> COCO

    // Retorna o resultado da conversão
    return *((volatile unsigned int *)(ADC0_BASE + 0x10)); // ADC0->RA
}

int main(void) {
    adc_init(); // Inicializa o ADC

    printf("Iniciando leitura do ADC...\n");

    unsigned int adc_value; // Variável para armazenar o valor lido

    while (1) {
        adc_value = adc_read(1); // Lê o valor do canal ADC1 (PTB0)
        printf("Valor ADC: %u\n", adc_value); // Imprime o valor lido

        // Atraso simples para não sobrecarregar a saída
        for (volatile unsigned int i = 0; i < 4800000; i++);
    }

    return 0;
}
