#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Encoder.h"
#include "Timer.h"
#include "key.h"
#include "Motor.h"
#include "LED.h"

// 电机转速切换时间(单位: 秒)。示例：三秒切换一次转向
#define DefaultMotorTime 5
// 电机转速最大值
#define MaxMotorSpeed 100 // 最大100%，即100

// 计时最大值
#define MaxTime 99

// 温度最大值
#define MaxTemperature 80

// 水位最大值
#define MaxWaterLevel 5

// 运行和停止状态
#define RunStateStopped 0
#define RunStateRunning 1
// 运行和停止状态
#define DefaultRunState RunStateStopped

// 旋转编码器转动方向
#define PressKey 1
#define NotPressKey 0

// 步长
#define Time_Step 5
#define WaterLevel_Step 1
#define Temperature_Step 5
#define Speed_Step 20

// 按钮状态
#define Clockwise 1
#define Anticlockwise -1

// 支持的Mode数量
#define ModifyNum 5 // 修改模式Mode的数量时需要同步更新该值，例如Mode数量为4，所以这里应该设置为4
// Modify默认值
#define DefaultModify ModifySetEmpty
// Modify
#define ModifySetEmpty 0
#define ModifySetTime 1
#define ModifySetSpeed 2
#define ModifySetWaterLevel 3
#define ModifySetTemperature 4

// 支持的Mode数量
#define ModeNum 7 // 修改模式Mode的数量时需要同步更新该值，例如Mode数量为4，所以这里应该设置为4
// Mode默认值
#define DefaultMode ModeStandardWash
// Mode
#define ModeStandardWash 0
#define ModeFastWash 1
#define ModeSlowWash 2
#define ModePowerWash 3
#define ModeDelicateWash 4
#define ModeHotWash 5
#define ModeColdWash 6

// 修改项闪烁
#define FlickerHidden 1
#define FlickerShow 0

// 流水灯
#define WaterfallLightDefaultTime 8 //(单位: 秒)
#define WaterfallLightHelloTime 1 //(单位: 秒)
#define WaterfallLightStatusNum 3
#define WaterfallLightStatusAllON 0 // 全部点亮
#define WaterfallLightStatusDefault 1 // 流水灯模式
#define WaterfallLightStatusAllOFF 2  // 全部熄灭

int8_t WaterfallLightTime = WaterfallLightHelloTime; // 流水灯流水时长
int8_t WaterfallLightStatus = WaterfallLightStatusAllOFF; // 流水灯状态
int8_t MotorTime = DefaultMotorTime; // 电机转向变换时间
uint8_t RunState = DefaultRunState; // 运行状态，running和stopped
uint8_t Mode = DefaultMode; // 模式 
uint8_t Modify = DefaultModify; // 修改对象
int16_t Time; // 计时时间
int8_t MotorSpeed; // 电机速度
int8_t Temperature; // 温度
int8_t WaterLevel; //水位
uint8_t Flicker; // 修改项闪烁

// 计算一个有符号数的绝对值
int8_t Abs(int8_t n) {
    if (n < 0) {
        return -1 * n;
    }
    return n;
}

// 调整运行状态
void Run_State_Turn(void) {
    if (RunState == RunStateStopped) {
        RunState = RunStateRunning;
        MotorTime = DefaultMotorTime;
    } else if (RunState == RunStateRunning) {
        RunState = RunStateStopped;
    }
}

// 调整闪烁状态
void Flicker_Turn(void) {
    if (Flicker == FlickerShow) {
        Flicker = FlickerHidden;
    } else if (Flicker == FlickerHidden) {
        Flicker = FlickerShow;
    }
}

// Mode状态调整
void Mode_Turn(void) {
    Mode++;
    if (Mode > ModeNum - 1) {
        Mode = 0;
    }
}

// Modify状态调整
void Modify_Turn(void) {
    Modify++;
    if (Modify > ModifyNum - 1) {
        Modify = 0;
    }
}

// WaterfallLightStatus状态调整
void WaterfallLightStatus_Turn(void) {
    WaterfallLightStatus++;
    if (WaterfallLightStatus > WaterfallLightStatusNum - 1) {
        WaterfallLightStatus = 0;
    }
}

// TIM3定时中断函数，每隔一秒中断一次
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        if (RunState == RunStateRunning) {
            Time--;
            MotorTime--;
            if (MotorTime <= 0) { // 计时结束进行反转
                MotorTime = DefaultMotorTime;
                MotorSpeed = MotorSpeed * (-1);
            }
        }

        // 每个一秒触发一次选项闪烁
        Flicker_Turn();

        if (WaterfallLightStatus == WaterfallLightStatusDefault && WaterfallLightTime > 0) {
            WaterfallLightTime--;
        }
        // 清除中断标志位
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

// 选项闪烁
void Modify_Object_Flicker(void) {
    if (Flicker == FlickerHidden) {
        switch (Modify) {
            case ModifySetTime:
                OLED_ShowString(3, 6, "  ");
                break;
            case ModifySetSpeed:
                OLED_ShowString(4, 7, "   ");
                break;
            case ModifySetWaterLevel:
                OLED_ShowString(3, 15, " ");
                break;
            case ModifySetTemperature:
                OLED_ShowString(4, 15, "  ");
                break;
        }
    } else if (Flicker == FlickerShow) {
        OLED_ShowNum(3, 6, Time, 2); // 显示计时

        OLED_ShowNum(3, 15, WaterLevel, 1); // 显示水位

        OLED_ShowNum(4, 7, Abs(MotorSpeed), 3); // 显示转速

        OLED_ShowNum(4, 15, Temperature, 2); // 显示温度
    }
}


void Washing_Machine_State_Show(void) {
    // 显示字段名
    OLED_ShowString(1, 1, "Mode:");
    OLED_ShowString(2, 1, "State:");
    OLED_ShowString(3, 1, "Time:");
    OLED_ShowString(3, 9, "Level:");
    OLED_ShowString(4, 1, "Speed:");
    OLED_ShowString(4, 11, "Tem:");

    // 显示字段值
    if (Mode == ModeFastWash) { // 显示Mode的状态
        OLED_ShowString(1, 6, "FastWash ");
    } else if (Mode == ModeSlowWash) {
        OLED_ShowString(1, 6, "SlowWash ");
    } else if (Mode == ModeStandardWash) {
        OLED_ShowString(1, 6, "Standard ");
    } else if (Mode == ModePowerWash) {
        OLED_ShowString(1, 6, "PowerWash");
    } else if (Mode == ModeDelicateWash) {
        OLED_ShowString(1, 6, "Delicate ");
    } else if (Mode == ModeHotWash) {
        OLED_ShowString(1, 6, "HotWash  ");
    } else if (Mode == ModeColdWash) {
        OLED_ShowString(1, 6, "ColdWash ");
    }


    if (RunState == RunStateRunning) { // 显示洗衣机的状态
        OLED_ShowString(2, 7, "Running...");
    } else if (RunState == RunStateStopped) {
        OLED_ShowString(2, 7, "Stoped    ");
    }

    // 进行选项修改时闪烁
    Modify_Object_Flicker();
}

// 函数：根据模式编号设置参数
void setWashMode() {
    switch (Mode) {
        case ModeFastWash:  // 快洗 Fast Wash
            Time = 20;
            MotorSpeed = 80;
            WaterLevel = 2;
            Temperature = 25;
            break;
        case ModeSlowWash:  // 慢洗 Slow Wash
            Time = 90;
            MotorSpeed = 40;
            WaterLevel = 5;
            Temperature = 30;
            break;
        case ModeStandardWash:  // 标准洗 Standard Wash
            Time = 60;
            MotorSpeed = 60;
            WaterLevel = 3;
            Temperature = 25;
            break;
        case ModePowerWash:  // 强力洗 Power Wash
            Time = 80;
            MotorSpeed = 100;
            WaterLevel = 4;
            Temperature = 30;
            break;
        case ModeDelicateWash:  // 柔洗 Delicate Wash
            Time = 70;
            MotorSpeed = 20;
            WaterLevel = 3;
            Temperature = 30;
            break;
        case ModeHotWash:  // 热水洗 Hot Wash
            Time = 50;
            MotorSpeed = 60;
            WaterLevel = 4;
            Temperature = 45;
            break;
        case ModeColdWash:  // 冷水洗 Cold Wash
            Time = 60;
            MotorSpeed = 60;
            WaterLevel = 4;
            Temperature = 15;
            break;
    }
}


int main(void) {
    // 初始化
    LED_Init();
    OLED_Init();
    Encoder_Init();
    Timer_Init();
    Key_Init();
    Motor_Init();
    setWashMode();

    // 开始时执行流水灯动画
    WaterfallLightStatus = WaterfallLightStatusDefault;

    /*
     * A0按键控制B12到B15流水灯代码。将下面的while循环注释，将这里的while循环解除注释即可使用
     * A0第一次按下，B12到B15流水灯全部点亮
     * A0第二次按下，B12到B15进行8秒计时流水灯，计时结束后流水灯全部关闭
    WaterfallLightStatus = WaterfallLightStatusAllOFF;
    WaterfallLightTime = WaterfallLightDefaultTime;

    while (1) {
        //  按下了按键，进行状态的调整
        if (Key_Get_A0() == PressKey) {
            WaterfallLightStatus_Turn();
        }

        if (WaterfallLightTime <= 0) {
            WaterfallLightTime = WaterfallLightDefaultTime;
            WaterfallLightStatus = WaterfallLightStatusAllOFF;
        }

        if (WaterfallLightStatus == WaterfallLightStatusDefault) {
            int16_t Num = 100;

            LED_B12_ON();
            LED_B13_OFF();
            LED_B14_OFF();
            LED_B15_OFF();

            Delay_ms(Num);

            LED_B13_ON();
            LED_B12_OFF();
            LED_B14_OFF();
            LED_B15_OFF();
            Delay_ms(Num);

            LED_B14_ON();
            LED_B13_OFF();
            LED_B12_OFF();
            LED_B15_OFF();

            Delay_ms(Num);

            LED_B15_ON();
            LED_B13_OFF();
            LED_B14_OFF();
            LED_B12_OFF();

            Delay_ms(Num);
        } else if (WaterfallLightStatus == WaterfallLightStatusAllON) {
            LED_B12_ON();
            LED_B13_ON();
            LED_B14_ON();
            LED_B15_ON();
        } else if (WaterfallLightStatus == WaterfallLightStatusAllOFF) {
            LED_B12_OFF();
            LED_B13_OFF();
            LED_B14_OFF();
            LED_B15_OFF();
        }
    }
     */


    while (1) {
        // 获取旋转编码器转向。1表示正转，-1表示反转，0表示没有转动
        int8_t direction = Encoder_GetDirection();

        //  按下了按键，进行状态的调整
        if (Key_Get_A0() == PressKey) {
            Mode_Turn();
            setWashMode();
        }

        // 和A1的LED同步
        if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) == 1){
            WaterfallLightStatus = WaterfallLightStatusAllON;
        }

        if (WaterfallLightTime <= 0) { // 计时结束后全部关闭
            WaterfallLightTime = WaterfallLightHelloTime;
            WaterfallLightStatus = WaterfallLightStatusAllOFF;
        }

        if (WaterfallLightStatus == WaterfallLightStatusDefault) {
            int16_t Num = 50;

            LED_B12_ON();
            LED_B13_OFF();
            LED_B14_OFF();
            LED_B15_OFF();

            Delay_ms(Num);

            LED_B13_ON();
            LED_B12_OFF();
            LED_B14_OFF();
            LED_B15_OFF();
            Delay_ms(Num);

            LED_B14_ON();
            LED_B13_OFF();
            LED_B12_OFF();
            LED_B15_OFF();

            Delay_ms(Num);

            LED_B15_ON();
            LED_B13_OFF();
            LED_B14_OFF();
            LED_B12_OFF();

            Delay_ms(Num);
        } else if (WaterfallLightStatus == WaterfallLightStatusAllON) {
            LED_B12_ON();
            LED_B13_ON();
            LED_B14_ON();
            LED_B15_ON();
        } else if (WaterfallLightStatus == WaterfallLightStatusAllOFF) {
            LED_B12_OFF();
            LED_B13_OFF();
            LED_B14_OFF();
            LED_B15_OFF();
        }


        if (RunState == RunStateRunning) {// 判断是否处于运行中，暂停状态停止电机转动
            Motor_SetSpeed(MotorSpeed);
        } else if (RunState == RunStateStopped) {
            Motor_SetSpeed(0);
        }

        // 按下按键之后进行模式的切换
        if (Key_GetEncoder() == PressKey) {
            if (Modify == ModifySetEmpty) {
                //Mode_Turn();
                //Modify_Turn();
                Run_State_Turn();
            } else {
                Modify_Turn();
            }
        }

        // 空状态时转动旋转编码器选择参数设置
        if (Modify == ModifySetEmpty && direction == Clockwise) {
            Modify_Turn();
        }

        // 设置转速模式
        if (Modify == ModifySetSpeed) {
            // 增加电机转速，如果正转(正值)进行正向增加，如果反转(负值)进行反向增加，
            if (direction == Clockwise) {
                if (MotorSpeed < 0) {
                    MotorSpeed -= Speed_Step;
                } else {
                    MotorSpeed += Speed_Step;
                }
            } else if (direction == Anticlockwise) {
                if (MotorSpeed < 0) {
                    MotorSpeed += Speed_Step;
                } else if (MotorSpeed == 0) {
                    MotorSpeed = 0;
                } else {
                    MotorSpeed -= Speed_Step;
                }
            }
        }

        if (MotorSpeed > MaxMotorSpeed) { // 判断是否超过范围
            MotorSpeed = MaxMotorSpeed;
        } else if (MotorSpeed < -1 * MaxMotorSpeed) {
            MotorSpeed = -1 * MaxMotorSpeed;
        }

        // 设置时间模式
        if (Modify == ModifySetTime) {
            Time += direction * Time_Step;
        }

        if (Time < 0) { // 判断计时是否结束
            Time = 0;
            RunState = RunStateStopped;
            WaterfallLightStatus = WaterfallLightStatusDefault;
        } else if (Time > MaxTime) {
            Time = MaxTime;
            RunState = RunStateStopped;
        }

        // 设置温度模式
        if (Modify == ModifySetTemperature) {
            Temperature += direction * Temperature_Step;
        }

        if (Temperature <= 0) {  // 判断是否超过范围
            Temperature = 0;
        } else if (Temperature > MaxTemperature) {
            Temperature = MaxTemperature;
        }

        // 设置水位模式
        if (Modify == ModifySetWaterLevel) {
            WaterLevel += direction * WaterLevel_Step;
        }

        if (WaterLevel <= 0) {  // 判断是否超过范围
            WaterLevel = 0;
        } else if (WaterLevel > MaxWaterLevel) {
            WaterLevel = MaxWaterLevel;
        }

        // 显示洗衣机状态
        Washing_Machine_State_Show();

        // 显示电源灯状态
        if (RunState == RunStateRunning) {
            LED_A1_ON();
        } else {
            LED_A1_OFF();
        }
    }
}





