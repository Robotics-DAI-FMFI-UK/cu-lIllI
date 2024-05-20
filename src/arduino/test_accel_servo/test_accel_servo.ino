#include <Servo.h>

#include "accel_movement.h"

Servo s;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  s.attach(2);
  delay(1000);
  s.write(5);
  delay(1000);
  s.write(175);
  delay(2000);

  accel_movement am;

  digitalWrite(13, HIGH);
  double t = millis();
  am.setup(175.0, 5.0, t + 1000, t + 1700);
  for (int i = 0; i < 5000; i++)
  {    
    s.write(am.current_position());
    delay(1);
  }
  digitalWrite(13, LOW);

  t = millis();
  am.setup(5.0, 175.0, t + 1000, t + 2200);
  for (int i = 0; i < 5000; i++)
  {
    s.write(am.current_position());
    delay(1);
  }
  digitalWrite(13, HIGH);
}

void loop() {
  
}
