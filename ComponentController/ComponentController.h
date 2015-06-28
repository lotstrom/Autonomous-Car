/* ---------------------------------------------------------------------------
** 
** ComponentController.h
** Header file for arduino component controller.
**  -> Updates Servo/ESC
**  -> Reads Sensor Data
**
** Author: Robert Petre
** -------------------------------------------------------------------------*/
#ifndef COMPONENTCONTROLLER_H
#define COMPONENTCONTROLLER_H

#include "Arduino.h"
#include "../Servo/Servo.h"

#define SERVO_MIN 60
#define SERVO_MAX 130
#define ESC_MIN 74
#define ESC_MAX 82

#define US_1_TRIG 18
#define US_1_ECHO 19

#define US_2_TRIG 20
#define US_2_ECHO 21

#define LED_REAR_LEFT 12
#define LED_REAR_RIGHT 11

#define IR_PIN 0

#define SERVO_PIN 6
#define ESC_PIN 5

class ComponentController{
public:
	ComponentController();
	void writeEsc(int speed);
	void writeServo(int angle);
	int read_IR();
	int read_US_1();
	int read_US_2();
	int setup();
private:
	void blink(int led,bool state);
	bool is_blinking;
	Servo servo;
	Servo esc;
	int currentSpeed;
	int currentAngle;	
};

#endif
