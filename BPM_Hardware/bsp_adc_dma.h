#ifndef _BSP_ADC_DMA_H
#define _BSP_ADC_DMA_H
#include "stm32f1xx_hal.h"
#include "main.h"

// 1. 宏定义
#define ADC_BUFFER_SIZE 100
#define BP_BIT_LIT(x, n)   ((x) |= (1 << (n)))


// 2. 结构体定义
typedef struct {
    int sys;
    int dia;
    int hr;
} BP_Measure;

// 3. 外部变量声明
extern volatile uint8_t adc_dma_flag;


// 4. 函数声明
void BSP_ADC_DMA_Init(void);
void BSP_ADC_DMA_Start(void);
uint32_t BSP_ADC_Get_Buffer_Element(uint8_t index);
uint32_t BSP_ADC_Get_Average(void);
	


#endif
