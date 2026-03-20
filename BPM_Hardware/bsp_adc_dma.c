#include "bsp_adc_dma.h"
#include "main.h"

// 1. 全局变量放这里（前面加static）
static ADC_HandleTypeDef hadc1;	//ADC句柄
static DMA_HandleTypeDef hdma_adc1; // 定义ADC1对应的DMA句柄
static volatile uint32_t adc_buffer[ADC_BUFFER_SIZE];	//定义数据缓冲区

//static BP_Measure last_measure;  // TOD0:后面存储上一次测量结果时会用到
volatile uint8_t adc_dma_flag=0;	/*全局标志：DMA采集满一轮后置1，volatile防止编译器优化，
									外部需要访问所以不加static*/

/*2. 函数实现放这里*/
void BSP_ADC_DMA_Init(void)
{
	/*第1块：DMA初始化（来自 MX_DMA_Init和
	adc.c文件的void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)函数）*/
	 __HAL_RCC_DMA1_CLK_ENABLE();
	hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);
  
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	/*第2块：ADC初始化（全部来自 MX_ADC1_Init）*/
  ADC_ChannelConfTypeDef sConfig = {0};
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  
  
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
    Error_Handler();
	}
}


void BSP_ADC_DMA_Start(void)// 启动ADC+DMA采集，数据写入adc_buffer
{
    
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE);// 关键：调用HAL库函数，指定缓冲区和长度
}


/*第3块：中断回调函数(DMA 传输完成（缓冲区满）回调函数)*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance==ADC1)
	{
		adc_dma_flag=1;
	}
}


/*第4块：用封装函数的方法解决以下问题：
static 修饰的变量只能在当前 .c 文件里可见，main.c 里直接访问它，编译器就会报：use of 
undeclared identifier 'adc_buffer'（未声明的标识符）*/
uint32_t BSP_ADC_Get_Buffer_Element(uint8_t index)
{
	if(index>=ADC_BUFFER_SIZE) 
	{
		return 0;//越界保护，防止程序崩溃
	}		
		return adc_buffer[index];
}
uint32_t BSP_ADC_Get_Average(void)// 直接用之前的平均值函数
{
	uint32_t sum=0;
	for(int i=0;i<ADC_BUFFER_SIZE;i++)
	{
		sum+=adc_buffer[i];
	}
	return sum/ADC_BUFFER_SIZE;
}

 //3. 位操作在函数里用
void example_use(void) 
{
    uint8_t reg = 0;
     BP_BIT_LIT(reg, 3);  // 调用刚才的宏
}

