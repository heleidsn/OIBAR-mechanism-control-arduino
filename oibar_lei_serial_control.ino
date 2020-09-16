/*
 * OIBAR end-effector RC radio control
 * Lei He
 * control actuator in and out using : AC1_IN AC1_OUT AC2_IN AC2_OUT
 * 13 Aug 2020
 * Change log:
 * V1.1: 09 Sep 2020 
 *       Add delay funciton for the linear actuator
 */
#include <Servo.h>
#include <AFMotor.h>

Servo linear_actuator;  // create servo object to control a servo
int ACTUATOR1_POS_IN = 0;    // degrees for normal servo
int ACTUATOR1_POS_OUT = 115; // degrees for normal servo

int ACTUATOR2_STEPS = 270;

// Connect a stepper motor with 24 steps per revolution (15 degree)
// to motor port #2 (M3 and M4)
AF_Stepper stepper_motor(24, 2);

// pwm read
byte PWM_PIN_1 = A5;
byte PWM_PIN_2 = A4;
int pwm_value_1;
int pwm_value_2;

int pos = 0;
int angle = 0;

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // actuator 1
  linear_actuator.attach(9, 1000, 2000); // attaches the servo on pin 9 to the servo object
  // linear_actuator.write(ACTUATOR1_POS_IN); // move actuator1 to in position
  for (pos = ACTUATOR1_POS_OUT; pos >= 0; pos -= 1){
          linear_actuator.write(pos);
          delay(50);
        }

  // actuator 2 stepper motor
  stepper_motor.setSpeed(40);

  // pwm read
  pinMode(PWM_PIN_1, INPUT);
  pinMode(PWM_PIN_2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (stringComplete) {
    Serial.println(inputString);
    if (inputString == "AC1_IN\n") {
      angle = linear_actuator.read();
      if (angle == ACTUATOR1_POS_IN){
        Serial.println("AC1 is already IN");
      }
      else{
        for (pos = ACTUATOR1_POS_OUT; pos >= 0; pos -= 1){
          linear_actuator.write(pos);
          delay(50);
        }
        Serial.println("OK");
      }
    }
    if (inputString == "AC1_OUT\n") {
      angle = linear_actuator.read();
      if (angle == ACTUATOR1_POS_OUT){
        Serial.println("AC1 is already OUT");
      }
      else{
        for (pos = 0; pos <= ACTUATOR1_POS_OUT; pos += 1){
          linear_actuator.write(pos);
          delay(50);
        }
        Serial.println("OK");
      }
    }

    if (inputString == "AC2_IN\n") {
      stepper_motor.release();
      stepper_motor.step(ACTUATOR2_STEPS, BACKWARD, INTERLEAVE);
      Serial.println("OK");
    }
    if (inputString == "AC2_OUT\n") {
      stepper_motor.release();
      stepper_motor.step(ACTUATOR2_STEPS, FORWARD, INTERLEAVE);
      Serial.println("OK");
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  pwm_value_1 = pulseIn(PWM_PIN_1, HIGH);
  pwm_value_2 = pulseIn(PWM_PIN_2, HIGH);
  /*
  Serial.print("CH1: ");
  Serial.println(pwm_value_1);
  Serial.print("CH2: ");
  Serial.println(pwm_value_2);
  */
  delay(100);
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
