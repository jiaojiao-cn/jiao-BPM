/*第一步：写 oled.c 的基础部分*/
#include "oled.h"
#include <stdint.h>
#include "OLED_Font.h"

extern I2C_HandleTypeDef hi2c1;	// 引用 CubeMX 生成的 I2C 句柄。这个 hi2c1 是在 main.c 里定义的，我们要告诉编译器它来自外部。


static void OLED_WriteCmd(uint8_t cmd)	//我们需要两个最基本的函数：写命令 和 写数据。这两个函数只在本文件内部使用，所以用 static 修饰（静态函数，外部不能调用）。
	{
		uint8_t buf[2];		// 定义一个数组存放要发送的两个字节
		buf[0]=0x00;	 // 第一个字节是控制字节，表示后面是命令
		buf[1]=cmd;		// 第二个字节是命令本身
		if(HAL_I2C_Master_Transmit(&hi2c1,OLED_I2C_ADDR,buf,2,100)!=HAL_OK)	// 发送
		{
			while(1); // 通信失败时进入死循环，方便定位
		}
	}

static void OLED_WriteData(uint8_t data)
	{
		uint8_t buf[2];
		buf[0]=0x40;	// 控制字节，表示后面是数据
		buf[1]=data;
		HAL_I2C_Master_Transmit(&hi2c1,OLED_I2C_ADDR,buf,2,100);	//注意：HAL_I2C_Master_Transmit 最后一个参数是超时时间（毫秒），100 表示如果 100 毫秒内没发完就超时返回。
	}
	
	
/*第二步：写初始化函数*/
void OLED_Init(void)
{
    HAL_Delay(200);  // 延长上电等待时间，确保电源稳定

    OLED_WriteCmd(0xAE);       // 关闭显示
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0x80);  // 时钟分频
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F);  // 多路复用率
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);  // 显示偏移
    OLED_WriteCmd(0x40);       // 显示开始行
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14);  // 开启电荷泵（关键！）
    HAL_Delay(10);             // 电荷泵启动延时
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x00);  // 水平地址模式
    OLED_WriteCmd(0xA1);       // 列重映射
    OLED_WriteCmd(0xC8);       // 行扫描方向
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x12);  // COM引脚配置
    OLED_WriteCmd(0x81); OLED_WriteCmd(0xCF);  // 对比度（可调）
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0xF1);  // 预充电周期
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x30);  // VCOMH电平（补全漏写的参数）
    OLED_WriteCmd(0xA4);       // 跟随RAM显示
    OLED_WriteCmd(0xA6);       // 正常显示
    OLED_WriteCmd(0x2E);       // 停止滚动
    OLED_WriteCmd(0xAF);       // 开启显示

    OLED_Clear();  // 初始化后清屏
}

	
	
	void OLED_Clear(void)
	{
		for(uint8_t page=0;page<8;page++)
		{
			OLED_WriteCmd(0xB0+page);	// 设置页地址 (0xB0 + page)
			OLED_WriteCmd(0x00);	 // 设置列起始地址为 0（低4位和高4位分别发）
			OLED_WriteCmd(0x10);
			
			for(uint8_t col=0; col<128; col++)	// 连续写 128 个字节的数据 0x00
			{
				OLED_WriteData(0x00);
			}
		}
	}
	
	
	void OLED_ShowString(uint8_t x, uint8_t y, char *str) 
		{
			// 这里应该有循环调用 OLED_ShowChar 的代码
			while(*str != '\0')  // 循环直到字符串结束
    {
        OLED_ShowChar(x, y, *str);
        x += 8;  // 每个字符占8列
        // 超出屏幕宽度则换行
        if(x > 120)
        {
            x = 0;
            y += 2;
        }
        str++;  // 指向下一个字符
    }
		}
		
		// 新增：显示单个字符函数（关键）
void OLED_ShowChar(uint8_t x, uint8_t y, char ch)
{
    // 字符偏移（只显示空格到~的字符）
    uint8_t c = ch - ' ';
    if(c > 94) return;

    // 设置显示位置
    OLED_WriteCmd(0xB0 + y);                // 页（y=0~7）
    OLED_WriteCmd(((x & 0xF0) >> 4) | 0x10); // 列地址高4位
    OLED_WriteCmd(x & 0x0F);                // 列地址低4位

    // 显示字符的上8行
    for(uint8_t i=0; i<8; i++)
    {
        OLED_WriteData(OLED_F8x16[c][i]);  // 匹配字体数组名
    }

    // 显示字符的下8行
    OLED_WriteCmd(0xB0 + y + 1);
    OLED_WriteCmd(((x & 0xF0) >> 4) | 0x10);
    OLED_WriteCmd(x & 0x0F);
    for(uint8_t i=0; i<8; i++)
    {
        OLED_WriteData(OLED_F8x16[c][i+8]);
    }
}
