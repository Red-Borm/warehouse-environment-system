#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>
#include "sys.h"

/* 步进电机相位控制引脚 */
#define StepMotor_A PAout(0)  /* PA0 -> A 相 */
#define StepMotor_B PAout(1)  /* PA1 -> B 相 */
#define StepMotor_C PAout(2)  /* PA2 -> C 相 */
#define StepMotor_D PAout(3)  /* PA3 -> D 相 */

/* 直流电机（排风扇）控制引脚 */
#define DCColor PAout(4)      /* PA4 -> 直流电机使能 */

/* 初始化电机 GPIO（PA0~PA4，推挽输出） */
void MotorInit(void);

/* 步进电机控制：1=正转，2=反转，其他=停止 */
void StepMotorControl(uint8_t Status);

#endif
