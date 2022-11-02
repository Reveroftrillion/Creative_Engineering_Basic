#include <Servo.h>

// Arduino pin assignment

#define PIN_POTENTIOMETER 3 // Potentiometer at Pin A3
// Add IR Sensor Definition Here !!!
#define PIN_SERVO 10
#define PIN_LED 9

#define PIN_IR 0

#define _DIST_MIN 100
#define _DIST_MAX 250

#define _EMA_ALPHA 0.35

#define _DUTY_MIN 553  // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1476 // servo neutral position (90 degree)
#define _DUTY_MAX 2399 // servo full counter-clockwise position (180 degree)

#define LOOP_INTERVAL 50   // Loop Interval (unit: msec)

Servo myservo;
unsigned long last_loop_time;   // unit: msec
unsigned long dist;

float dist_prev;        // Distance last-measured
float dist_ema;


void setup()
{
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(1000000);
}

void loop()
{
  unsigned long time_curr = millis();
  int a_value, duty;
  float dist_raw;

  // wait until next event time
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  a_value = analogRead(PIN_IR);
  // Read IR Sensor value !!!
  dist_raw = (6762.0/(a_value-9)-4.0)*10.0 - 60.0;
  // Convert IR sensor value into distance !!!
  // we need distance range filter here !!!
  if (dist_raw < _DIST_MIN) {
    dist_raw = dist_prev;    // Set Lower Value
    digitalWrite(PIN_LED, 1);       // LED OFF
  } else if (dist_raw  > _DIST_MAX) {
    dist_raw = dist_prev;    // Set Higher Value
    digitalWrite(PIN_LED, 1);       // LED OFF
  } else {    // In desired Range
    digitalWrite(PIN_LED, 0);       // LED ON      
    dist_prev = dist_raw;
  }
  // we need EMA filter here !!!
  dist_ema = _EMA_ALPHA*dist_raw + (1-_EMA_ALPHA)*dist_ema;


  // map distance into duty
  duty = (((dist_raw)-(_DIST_MIN))/150)*(_DUTY_MAX-_DUTY_MIN)+_DUTY_MIN;
  myservo.writeMicroseconds(duty);

  // print IR sensor value, distnace, duty !!!
  Serial.print("MIN: "); Serial.print(_DIST_MIN);
  Serial.print(",IR: "); Serial.print(a_value);
  Serial.print(",dist: "); Serial.print(dist);
  Serial.print(",ema: "); Serial.print(dist_ema);
  Serial.print(",servo: "); Serial.print(duty);
  Serial.print(",MAX: "); Serial.print(_DIST_MAX);
  Serial.println("");
}
