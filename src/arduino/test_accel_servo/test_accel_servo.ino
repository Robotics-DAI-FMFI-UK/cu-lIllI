#include <Servo.h>

#include "accel_movement.h"
#include "movement_sequence.h"
#include "servo_controller.h"
#include "simple_servo_controller.h"



void test_servo_traditional()
{
  Servo s;
  s.attach(2);
  delay(1000);
  s.write(5);
  delay(1000);
  s.write(175);
  delay(2000);
  s.detach();
}

void test_accel_movement()
{
  Servo s;
  s.attach(2);
  accel_movement am;
  s.write(175);
  delay(2000);

  digitalWrite(13, HIGH);
  double t = millis();
  am.setup(175.0, 5.0, t + 1000, t + 1700);
  for (int i = 0; i < 5000; i++)
  {    
    int p = (int)(0.5 + am.current_position());
    if (p >= 0) s.write(p);
    delay(1);
  }
  digitalWrite(13, LOW);

  t = millis();
  am.setup(5.0, 175.0, t + 1000, t + 2200);
  for (int i = 0; i < 5000; i++)
  {
    int p = (int)(0.5 + am.current_position());
    if (p >= 0) s.write(p);
    delay(1);
  }
  digitalWrite(13, HIGH);

  t = millis();
  am.setup(175.0, 90.0, t + 1000, t + 1400, 0, 0.1594);

  accel_movement am2;
  am2.setup(90.0, 5.0, t + 1400, t + 2200, 0.1594, 0);

  for (int i = 0; i < 5000; i++)
  {
    double p = am.current_position();    
    if (p >= 0) 
    {
      int pos = (int)(0.5 + p);
      if (pos == 90) break;
      s.write(pos);
    }
    delay(1);
  }

  for (int i = 0; i < 2000; i++)
  {
    double p = am2.current_position();
    if (p >= 0)
    {
      int pos = (0.5 + p);    
      s.write(pos);
    }
    delay(1);
  }

  t = millis();
  am.setup(175.0, 135, t + 1000, t + 1500, 0, 0.062);

  am2.setup(135, 60, t + 1500, t + 3200, 0.062, 0);

   for (int i = 0; i < 5000; i++)
  {
    double p = am.current_position();    
    if (p >= 0) 
    {
      int pos = (int)(0.5 + p);      
      s.write(pos);
    }
    else if (millis() >= t + 1500) break;
    delay(1);
  }

  for (int i = 0; i < 2000; i++)
  {
    double p = am2.current_position();
    if (p >= 0)
    {
      int pos = (0.5 + p);    
      s.write(pos);
    }
    delay(1);
  }

  digitalWrite(13, LOW);  
  s.detach();
}

void test_simple_servo_controller()
{
  int pins[] = {2, 3, 4};
  int init_pos[] = {90, 90, 90};

  simple_servo_controller ssc(3, pins, init_pos);
  movement_sequence ms(3, &ssc);
  ms.append(0, 1000, 2000, 90, 175);
  ms.append(1, 1000, 2500, 90, 30);
  ms.append(2, 1000, 2000, 90, 135);
  ms.append(0, 2000, 2500, 175, 135);
  ms.append(1, 2500, 3200, 30, 155);
  ms.append(2, 2500, 3000, 135, 85);
  ms.append(0, 2500, 4200, 135, 60);  
  ms.append(2, 3000, 3700, 85, 30);    
  ms.append(1, 3200, 4200, 155, 75);  
  ms.append(2, 3900, 4600, 30, 5);  
  ms.append(0, 4200, 4800, 60, 5);  
  ms.append(1, 4200, 4800, 75, 5);  

  /*
  movement_sequence ms(3, &ssc);
  ms.append(0, 1000, 2000, 90, 175);
  ms.append(1, 1200, 2000, 90, 135);
  ms.append(2, 1200, 1800, 90, 5);
  ms.append(0, 2000, 3000, 175, 5);
  ms.append(1, 2000, 2500, 135, 5);
  ms.append(2, 2200, 3200, 5, 130);
  ms.append(0, 3000, 4000, 5, 90);
  */
  ms.start(millis());
  for (int i = 0; i < 5000 / 8; i++)
  {
    ms.loop();
    ssc.loop();
    delay(18);
  }
}

void menu()
{
  Serial.println("Select test:");
  Serial.println(" 1: simple servo");
  Serial.println(" 2: accel movements on servo on pin 2");
  Serial.println(" 3: complex sequence with accel movements on servos on pins 2,3,4");
  Serial.print("> ");
  while (Serial.available()) Serial.read();
}

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT); 
  delay(500);
  menu();
}

void loop() {
  
  if (Serial.available())
  {
    char choice = Serial.read();
    Serial.println(choice);
    switch (choice)
    {
      case '1': test_servo_traditional(); break;
      case '2': test_accel_movement(); break;
      case '3': test_simple_servo_controller(); break;
    }
    Serial.println("---");
    menu();
  }
}
