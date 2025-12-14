#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "i2c.h"

#define OLED_ADDR        0x78  // 0x3C左移一位
#define OLED_WIDTH       128
#define OLED_HEIGHT      32

// 函数声明
void OLED_Init(void);
void OLED_Clear(void);
void OLED_WriteCmd(uint8_t); 
void OLED_SetPos(uint8_t page, uint8_t col);
void OLED_ShowNum32(uint8_t col, uint8_t num);
void OLED_PutChar(uint8_t col,uint8_t ch);
void OLED_ShowDot(uint8_t page, uint8_t col);

#endif
