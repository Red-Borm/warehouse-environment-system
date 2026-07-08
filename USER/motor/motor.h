#ifndef __MOTOR_H__
#define __MOTOR_H__
#include<stdint.h> 
#include "sys.h"

//步进电机引脚I/O
#define StepMotor_A PAout(0) //PA0->A
#define StepMotor_B PAout(1) //PA1->B
#define StepMotor_C PAout(2) //PA2->C
#define StepMotor_D PAout(3) //PA3->D

//直流电机引脚I/O
#define DCColor PAout(4) //PA4->DC 

void MotorInit(void);
void StepMotorControl(uint8_t Status);

#endif

