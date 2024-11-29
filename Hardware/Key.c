#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructureA;
	GPIO_InitStructureA.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructureA.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_11;
	GPIO_InitStructureA.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructureA);
}

uint8_t Key_Get_A0(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}
	
	return KeyNum;
}

uint8_t Key_Get_A11(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 1)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 1);
		Delay_ms(20);
		KeyNum = 1;
	}
	
	return KeyNum;
}
