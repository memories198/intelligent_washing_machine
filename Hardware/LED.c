#include "stm32f10x.h"                  // Device header

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructureA;
	GPIO_InitStructureA.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructureA.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructureA.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructureA);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructureB;
	GPIO_InitStructureB.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructureB.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructureB.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructureB);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

void LED_A1_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void LED_A1_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void LED_A1_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	}
}

void LED_B12_ON(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void LED_B12_OFF(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}
void LED_B12_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	}
}

void LED_B13_ON(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

void LED_B13_OFF(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}


void LED_B14_ON(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

void LED_B14_OFF(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}
void LED_B15_ON(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void LED_B15_OFF(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}


