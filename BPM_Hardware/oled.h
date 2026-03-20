#ifndef _OLED_H		//防止这个头文件被多次包含，避免重复定义错误
#define _OLED_H



#include "main.h"	// 包含 HAL 库的头文件，这样我们才能用 HAL 类型


#define OLED_I2C_ADDR    0x78	// OLED 的 I2C 地址（如果不行再改0x3D）
#define OLED_WIDTH       128
#define OLED_HEIGHT      64

// 函数声明
void OLED_Init(void);	// 初始化 OLED
void OLED_Clear(void);	// 清屏
void OLED_ShowChar(uint8_t x, uint8_t y, char ch);	// 显示一个字符
void OLED_ShowString(uint8_t x, uint8_t y, char *str);	 // 显示字符串

	




#endif 










