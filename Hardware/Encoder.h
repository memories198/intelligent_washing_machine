#ifndef __ENCODER_H
#define __ENCODER_H

void Encoder_Init(void);
int16_t Encoder_Get(void);
int8_t Encoder_GetDirection(void);
uint8_t Key_GetEncoder(void);

#endif
