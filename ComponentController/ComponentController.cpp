#include "Arduino.h"
#include "ComponentController.h"

ComponentController::ComponentController(){

	//servo.attach(SERVO_PIN);
  	//esc.attach(ESC_PIN);
	//pinMode(US_1_TRIG, OUTPUT);
  	//pinMode(US_1_ECHO, INPUT);
  	//pinMode(US_2_TRIG, OUTPUT);
 	//pinMode(US_2_ECHO, INPUT);

}
int ComponentController::setup(){
	servo.attach(SERVO_PIN);
	esc.attach(ESC_PIN);
	pinMode(US_1_TRIG, OUTPUT);
  	pinMode(US_1_ECHO, INPUT);
  	pinMode(US_2_TRIG, OUTPUT);
 	pinMode(US_2_ECHO, INPUT);
 	pinMode(LED_REAR_RIGHT,OUTPUT);
 	pinMode(LED_REAR_LEFT, OUTPUT);
 	is_blinking	=	false;

 	//delay(500);
 	return 0;
}
//Write esc
void ComponentController::writeEsc(int speed){
	esc.write(constrain(speed,ESC_MIN,ESC_MAX));
	if (speed == 74 && !is_blinking) {
		digitalWrite(LED_REAR_LEFT, HIGH);
		digitalWrite(LED_REAR_RIGHT,HIGH);
	}
	else if (!is_blinking) {
		digitalWrite(LED_REAR_RIGHT, LOW);
		digitalWrite(LED_REAR_LEFT, LOW);
	}
}

//Write servo
void ComponentController::writeServo(int angle){
	servo.write(constrain(angle,SERVO_MIN,SERVO_MAX));
	if (angle < 90) {
		blink(LED_REAR_LEFT,false);
		blink(LED_REAR_RIGHT,true);
		is_blinking = true;
	}
	else if( angle > 100) {
		blink(LED_REAR_LEFT,true);
		blink(LED_REAR_RIGHT,false);
		is_blinking = true;
	}
	else
	{
		blink(LED_REAR_LEFT,false);
		blink(LED_REAR_RIGHT,false);
		is_blinking = false;
	}
}

void ComponentController::blink(int led, bool state)
{
	if (state) {
		digitalWrite(led,HIGH);
	}
	else {
		digitalWrite(led,LOW);
	}

}

//Read infrared
int ComponentController::read_IR(){
	float scaledValue;
	int value = analogRead (IR_PIN); 
  	scaledValue = ((float)value / 615) * 1024;
  	return (int)scaledValue;
}

//Read ultrasound 1
int ComponentController::read_US_1(){
	//return 10;
	digitalWrite(US_1_TRIG, LOW);
	delayMicroseconds(2);
	digitalWrite(US_1_TRIG, HIGH);
	delayMicroseconds(10);
	digitalWrite(US_1_TRIG, LOW);
	return (int)(pulseIn(US_1_ECHO, HIGH, 10000) / 2 / 29.1);
	//return 50;
}

//Read ultrasound 2
int ComponentController::read_US_2(){
	//return 20;
	digitalWrite(US_2_TRIG, LOW);
	delayMicroseconds(2);
	digitalWrite(US_2_TRIG, HIGH);
	delayMicroseconds(10);
	digitalWrite(US_2_TRIG, LOW);
	return (int)(pulseIn(US_2_ECHO, HIGH, 10000) / 2 / 29.1);
}
