// Arduino pin assignment
#define PIN_LED 9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

float recent_distance = 0;

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float distance;

  // wait until next sampling time. 
  // millis() returns the number of milliseconds since the program started.
  //    Will overflow after 50 days.
  if (millis() < (last_sampling_time + INTERVAL))
    return;
  
  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  if (distance < _DIST_MIN) {
    distance = recent_distance;    // Set Lower Value
    digitalWrite(PIN_LED, 1);       // LED OFF
  } 
  else if (distance > _DIST_MAX) {
    distance = recent_distance;    // Set Higher Value
    digitalWrite(PIN_LED, 1);       // LED OFF
  }
  else {    // In desired Range
    if (_DIST_MIN <= distance <= 200){
      analogWrite(PIN_LED, abs(int((-2.55)*distance+510)));
    }
    else if (200 < distance <= _DIST_MAX){
      analogWrite(PIN_LED, abs(int((2.55)*distance-510)));
    }
  }

  recent_distance = distance;

//  // output the distance to the serial port
//  char buffer[300];
//  char tmp1[20];
//  dtostrf(distance, 4, 1, tmp1);
//  sprintf(buffer, "Min:100,distance:%s,Max:300", tmp1);
//  Serial.println(buffer);
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println();

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - round trip distance: 34.6m
  // - expected pulse duration: 0.1 sec, or 100,000us
  // - pulseIn(ECHO, HIGH, timeout) * 0.001 * 0.5 * SND_VEL
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346 * micro * sec * milli * meter
  //                                        ----------------------------
  //                                         micro * sec
  //        = 100 * 173 milli*meter = 17,300 mm = 17.3m
  // pulseIn() returns microseconds.
}
