#include <Adafruit_PWMServoDriver.h>
#include <avr/pgmspace.h>

#define MP3_OUTPUT_PIN 5   // connect to Rx pin of DFPlayer
#define MP3_BUSY_PIN 8     // connect to BUSY pin of DFPlayer

#define OE_LEFT_PIN 4
#define OE_RIGHT_PIN 3

#define BEEPER 7

#define SERVO_STEP 1

#define SERVO_MIN  ((4096L * 546 * 60) / 1000000L)
#define SERVO_MAX  ((4096L * 2375 * 60) / 1000000L)
#define SERVO_INIT ((4096L * 1500 * 60) / 1000000L)

#define CHOREO_LEN 100

/* 0-15 is controller in left leg, 16-31 is controller in right leg */
#define LEFT_FOOT                       0
#define LEFT_KNEE                       1
#define LEFT_HIP_LIFTING_FWD            2
#define LEFT_HIP_TURNING                3
#define LEFT_HIP_LIFTING_SIDEWAYS       4
#define LEFT_SHOULDER_LIFTING          10
#define LEFT_ARM_TURNING               11
#define LEFT_SHOULDER_TURNING          14
#define LEFT_ELBOW                      9
#define LEFT_WRIST                     12
#define LEFT_HAND_CLOSING              13

#define TORSO                          15
#define HEAD_TURNING                   23
#define HEAD_LIFTING                   24

#define RIGHT_FOOT                     16
#define RIGHT_KNEE                     17
#define RIGHT_HIP_LIFTING_FWD          19
#define RIGHT_HIP_TURNING              18
#define RIGHT_HIP_LIFTING_SIDEWAYS     20
#define RIGHT_SHOULDER_LIFTING         26
#define RIGHT_ARM_TURNING              22
#define RIGHT_SHOULDER_TURNING         25
#define RIGHT_ELBOW                    21
#define RIGHT_WRIST                    29
#define RIGHT_HAND_CLOSING             28

#define DEFAULT_CALIB_LEFT_FOOT                  292
#define DEFAULT_CALIB_LEFT_KNEE                  320
#define DEFAULT_CALIB_LEFT_HIP_LIFTING_FWD       291
#define DEFAULT_CALIB_LEFT_HIP_TURNING           480
#define DEFAULT_CALIB_LEFT_HIP_LIFTING_SIDEWAYS  309
#define DEFAULT_CALIB_LEFT_SHOULDER_LIFTING      482
#define DEFAULT_CALIB_LEFT_ARM_TURNING           370
#define DEFAULT_CALIB_LEFT_SHOULDER_TURNING      470
#define DEFAULT_CALIB_LEFT_ELBOW                 180
#define DEFAULT_CALIB_LEFT_WRIST                 290
#define DEFAULT_CALIB_LEFT_HAND_CLOSING          165

#define DEFAULT_CALIB_TORSO                      280
#define DEFAULT_CALIB_HEAD_TURNING               320
#define DEFAULT_CALIB_HEAD_LIFTING               310

#define DEFAULT_CALIB_RIGHT_FOOT                 325
#define DEFAULT_CALIB_RIGHT_KNEE                 312
#define DEFAULT_CALIB_RIGHT_HIP_LIFTING_FWD      300
#define DEFAULT_CALIB_RIGHT_HIP_TURNING          207
#define DEFAULT_CALIB_RIGHT_HIP_LIFTING_SIDEWAYS 417
#define DEFAULT_CALIB_RIGHT_SHOULDER_LIFTING     215
#define DEFAULT_CALIB_RIGHT_ARM_TURNING          250
#define DEFAULT_CALIB_RIGHT_SHOULDER_TURNING     170
#define DEFAULT_CALIB_RIGHT_ELBOW                530
#define DEFAULT_CALIB_RIGHT_WRIST                280
#define DEFAULT_CALIB_RIGHT_HAND_CLOSING         480

Adafruit_PWMServoDriver right = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver left = Adafruit_PWMServoDriver(0x41);

uint16_t servo_position[32];
uint16_t servo_target[32];
uint8_t servo_delay[32];
uint8_t servo_remains[32];

int current_servo;
int ch_len;

uint16_t ch_time[CHOREO_LEN];
uint8_t ch_servo[CHOREO_LEN];
uint16_t ch_val[CHOREO_LEN];

void setup()
{
  pinMode(OE_LEFT_PIN, OUTPUT);
  digitalWrite(OE_LEFT_PIN, HIGH);
  pinMode(OE_RIGHT_PIN, OUTPUT);
  digitalWrite(OE_RIGHT_PIN, HIGH);
  pinMode(BEEPER, OUTPUT);
  tone(BEEPER, 1760, 50);
  
  delay(50);
  mp3_set_volume(30);
  delay(10);
  mp3_play(5);
    
  Serial.begin(115200);
  setup_default_calibration();
  left.begin();
  left.setPWMFreq(60);
  right.begin();
  right.setPWMFreq(60);
  delay(10);
  current_servo = 0;

  digitalWrite(OE_LEFT_PIN, LOW);
  for (int i = 0; i < 16; i++)
  {
    left.setPWM(i, 0, servo_position[i]);
    delay(300);
  }

  digitalWrite(OE_RIGHT_PIN, LOW);
  for (int i = 0; i < 16; i++)
  {
    right.setPWM(i, 0, servo_position[i + 16]);
    delay(300);
  }

  mp3_play(4);
}

void loop()
{
  console_loop();
}

void setup_default_calibration()
{
  servo_position[LEFT_FOOT] = DEFAULT_CALIB_LEFT_FOOT;
  servo_position[LEFT_KNEE] = DEFAULT_CALIB_LEFT_KNEE;
  servo_position[LEFT_HIP_LIFTING_FWD] = DEFAULT_CALIB_LEFT_HIP_LIFTING_FWD;
  servo_position[LEFT_HIP_TURNING] = DEFAULT_CALIB_LEFT_HIP_TURNING;
  servo_position[LEFT_HIP_LIFTING_SIDEWAYS] = DEFAULT_CALIB_LEFT_HIP_LIFTING_SIDEWAYS;
  servo_position[LEFT_SHOULDER_LIFTING] = DEFAULT_CALIB_LEFT_SHOULDER_LIFTING;
  servo_position[LEFT_ARM_TURNING] = DEFAULT_CALIB_LEFT_ARM_TURNING;
  servo_position[LEFT_SHOULDER_TURNING] = DEFAULT_CALIB_LEFT_SHOULDER_TURNING;
  servo_position[LEFT_ELBOW] = DEFAULT_CALIB_LEFT_ELBOW;
  servo_position[LEFT_WRIST] = DEFAULT_CALIB_LEFT_WRIST;
  servo_position[LEFT_HAND_CLOSING] = DEFAULT_CALIB_LEFT_HAND_CLOSING;
  servo_position[TORSO] = DEFAULT_CALIB_TORSO;
  servo_position[HEAD_TURNING] = DEFAULT_CALIB_HEAD_TURNING;
  servo_position[HEAD_LIFTING] = DEFAULT_CALIB_HEAD_LIFTING;
  servo_position[RIGHT_FOOT] = DEFAULT_CALIB_RIGHT_FOOT;
  servo_position[RIGHT_KNEE] = DEFAULT_CALIB_RIGHT_KNEE;
  servo_position[RIGHT_HIP_LIFTING_FWD] = DEFAULT_CALIB_RIGHT_HIP_LIFTING_FWD;
  servo_position[RIGHT_HIP_TURNING] = DEFAULT_CALIB_RIGHT_HIP_TURNING;
  servo_position[RIGHT_HIP_LIFTING_SIDEWAYS] = DEFAULT_CALIB_RIGHT_HIP_LIFTING_SIDEWAYS;
  servo_position[RIGHT_SHOULDER_LIFTING] = DEFAULT_CALIB_RIGHT_SHOULDER_LIFTING;
  servo_position[RIGHT_ARM_TURNING] = DEFAULT_CALIB_RIGHT_ARM_TURNING;
  servo_position[RIGHT_SHOULDER_TURNING] = DEFAULT_CALIB_RIGHT_SHOULDER_TURNING;
  servo_position[RIGHT_ELBOW] = DEFAULT_CALIB_RIGHT_ELBOW;
  servo_position[RIGHT_WRIST] = DEFAULT_CALIB_RIGHT_WRIST;
  servo_position[RIGHT_HAND_CLOSING] = DEFAULT_CALIB_RIGHT_HAND_CLOSING;
  for (uint8_t i = 0; i < 32; i++) servo_delay[i] = 5;
}

void println_servo_name(uint8_t servo_number)
{
  serial_println_flash(servo_name(servo_number));
}

const char *servo_name(uint8_t servo_number)
{
  switch (servo_number)
  {
    case LEFT_FOOT: return PSTR("LEFT_FOOT");
    case LEFT_KNEE: return PSTR("LEFT_KNEE");
    case LEFT_HIP_LIFTING_FWD: return PSTR("LEFT_HIP_LIFTING_FWD");
    case LEFT_HIP_TURNING: return PSTR("LEFT_HIP_TURNING");
    case LEFT_HIP_LIFTING_SIDEWAYS: return PSTR("LEFT_HIP_LIFTING_SIDEWAYS");
    case LEFT_SHOULDER_LIFTING: return PSTR("LEFT_SHOULDER_LIFTING");
    case LEFT_ARM_TURNING: return PSTR("LEFT_ARM_TURNING");
    case LEFT_SHOULDER_TURNING: return PSTR("LEFT_SHOULDER_TURNING");
    case LEFT_ELBOW: return PSTR("LEFT_ELBOW");
    case LEFT_WRIST: return PSTR("LEFT_WRIST");
    case LEFT_HAND_CLOSING: return PSTR("LEFT_HAND_CLOSING");
    case TORSO: return PSTR("TORSO");
    case HEAD_TURNING: return PSTR("HEAD_TURNING");
    case HEAD_LIFTING: return PSTR("HEAD_LIFTING");
    case RIGHT_FOOT: return PSTR("RIGHT_FOOT");
    case RIGHT_KNEE: return PSTR("RIGHT_KNEE");
    case RIGHT_HIP_LIFTING_FWD: return PSTR("RIGHT_HIP_LIFTING_FWD");
    case RIGHT_HIP_TURNING: return PSTR("RIGHT_HIP_TURNING");
    case RIGHT_HIP_LIFTING_SIDEWAYS: return PSTR("RIGHT_HIP_LIFTING_SIDEWAYS");
    case RIGHT_SHOULDER_LIFTING: return PSTR("RIGHT_SHOULDER_LIFTING");
    case RIGHT_ARM_TURNING: return PSTR("RIGHT_ARM_TURNING");
    case RIGHT_SHOULDER_TURNING: return PSTR("RIGHT_SHOULDER_TURNING");
    case RIGHT_ELBOW: return PSTR("RIGHT_ELBOW");
    case RIGHT_WRIST: return PSTR("RIGHT_WRIST");
    case RIGHT_HAND_CLOSING: return PSTR("RIGHT_HAND_CLOSING");
    default: return PSTR("-");
  }
}

void serial_print_flash(const char *str)
{
  int ln = strlen_P(str);
  for (int i = 0; i < ln; i++)
    Serial.write(pgm_read_byte(str + i));
}

void serial_println_flash(const char *str)
{
  serial_print_flash(str);
  Serial.write(13);
  Serial.write(10);
}

void show_servo(int s)
{
  Serial.print(s);
  Serial.print(": ");
  Serial.println(servo_position[s]);
}

int servo_can_move_up(int s)
{
  return servo_position[s] < SERVO_MAX - SERVO_STEP;
}

int servo_can_move_down(int s)
{
  return servo_position[s] > SERVO_MIN + SERVO_STEP;
}

int readNumber()
{
  int num = 0;
  int z;
  do {
    z = Serial.read();
    if (z == '#') while (z != 13) z = Serial.read();
    //if (mpuIntStatus) mpu.resetFIFO();
  } while ((z < '0') || (z > '9'));
  while ((z >= '0') && (z <= '9'))
  {
    num *= 10;
    num += (z - '0');
    do {
      z = Serial.read();
      if (z == -1) delayMicroseconds(10);
      //if (mpuIntStatus) mpu.resetFIFO();
    } while (z < 0);
  }
  return num;
}

void load_choreography()
{
  ch_len = 0;
  int tm;
  do {
    tm = readNumber();
    ch_time[ch_len] = tm;
    ch_servo[ch_len] = readNumber();
    ch_val[ch_len] = readNumber();
    if (ch_len == CHOREO_LEN) break;
    if ((tm == 0) && (ch_servo[ch_len] == 0) && (ch_val[ch_len] == 0)) break;
    ch_len++;
  } while (1);
  Serial.print("Loaded. Lines: ");
  Serial.println(ch_len);
}

void dance_tick()
{
  for (uint8_t i = 0; i < 32; i++)
  {
    if (servo_target[i] != servo_position[i])
    {
      if (!servo_remains[i]) 
      { 
        servo_remains[i] = servo_delay[i];
        if (servo_position[i] > servo_target[i]) servo_position[i]--;
        else servo_position[i]++;
        setPWM(i, servo_position[i]);
      }
      else servo_remains[i]--;
    }
  }
  delay(1);
}

uint32_t dance_finish_time;
uint32_t choreography_started;
uint32_t subroutine_pushed_time;
uint8_t pushed_position;

void dance()
{
  subroutine_pushed_time = 0;
  choreography_started = millis();
  dance_finish_time = choreography_started + 1000L*60*60;
  for (uint8_t i = 0; i < 32; i++)
    servo_target[i] = servo_position[i];
  uint8_t i = 0;
  do {
    if (millis() - choreography_started >= subroutine_pushed_time + ch_time[i])
    {   
      Serial.print(".");
      uint8_t servo = ch_servo[i];
      if (servo < 32)
      {
        servo_target[servo] = ch_val[i];
        servo_remains[servo] = 0;
      }
      else i = special_command(i, ch_servo[i], ch_val[i], ch_len);
      i++;
    } else dance_tick();
    if (Serial.available()) { Serial.print("kbd"); break; }
    if (millis() > dance_finish_time) { Serial.println("done"); break; } 
  } while (i < ch_len);
  tone(BEEPER, 1760, 200);
}

uint8_t special_command(uint16_t i, uint8_t cmd, uint16_t arg, int len)
{
  if (cmd < 64) servo_delay[cmd - 32] = arg;
  else if (cmd == 64) dance_finish_time = millis() + 1000 * (uint32_t)arg; 
  else if (cmd == 65) { choreography_started = millis() - ch_time[arg - 1]; return arg - 1; }
  else if (cmd == 66) mp3_play(arg);
  else if (cmd == 67) choreography_started = millis();
  else if (cmd == 68) 
  {
    do { i++; } while ((ch_servo[i] != 69) && (i < len));
    return i;
  }
  else if (cmd == 69) { subroutine_pushed_time = 0; return pushed_position; }
  else if (cmd == 70)
  {
    pushed_position = i; 
    subroutine_pushed_time = millis() - choreography_started;
    for (i = 0; i < len; i++)
      if ((ch_servo[i] == 68) && (ch_val[i] == arg)) return i;
    i = pushed_position;
  }
  else if (cmd == 71) tone(BEEPER, 1760, 200);
  return i;
}

void print_choreography()
{
  for (int i = 0; i < ch_len; i++)
  {
    Serial.print(ch_time[i]);
    Serial.print(" ");
    Serial.print(ch_servo[i]);
    Serial.print(" ");
    Serial.println(ch_val[i]);
  }
  Serial.println("---");
  tone(BEEPER, 1760, 200);
}

void setPWM(int servo_number, int value)
{
  if (servo_number < 16)
    left.setPWM(servo_number, 0, value);
  else
    right.setPWM(servo_number - 16, 0, value);
}

void look_around()
{
  int look_left = 400;
  int look_right = 240;
  int old_head_position = servo_position[HEAD_TURNING];
  int look = old_head_position;
  int delta = (old_head_position > look_right) ? (-1) : 1;

  while (look != look_right)
  {
    look += delta;
    delay(20);
    setPWM(HEAD_TURNING, look);
  }

  while (look < look_left)
  {
    look++;
    delay(20);
    setPWM(HEAD_TURNING, look);
  }
  while (look > look_right)
  {
    look--;
    delay(20);
    setPWM(HEAD_TURNING, look);
  }

  while (look != old_head_position)
  {
    look -= delta;
    delay(20);
    setPWM(HEAD_TURNING, look);
  }
}

void console_loop()
{
  if (Serial.available())
  {
    char c = Serial.read();
    switch (c) {
      case 'q': if (servo_can_move_up(current_servo))
        {
          servo_position[current_servo] += SERVO_STEP;
          if (current_servo < 16)
            left.setPWM(current_servo, 0, servo_position[current_servo]);
          else
            right.setPWM(current_servo - 16, 0, servo_position[current_servo]);
          show_servo(current_servo);
        }
        break;
      case 'a': if (servo_can_move_down(current_servo))
        {
          servo_position[current_servo] -= SERVO_STEP;
          if (current_servo < 16)
            left.setPWM(current_servo, 0, servo_position[current_servo]);
          else
            right.setPWM(current_servo - 16, 0, servo_position[current_servo]);
          show_servo(current_servo);
        }
        break;
      case '1': if (current_servo > 0) current_servo--;
        Serial.print("s: ");
        Serial.print(current_servo);
        Serial.print(" - ");
        println_servo_name(current_servo);
        break;
      case '9': if (current_servo < 31) current_servo++;
        Serial.print("s: ");
        Serial.print(current_servo);
        Serial.print(" - ");
        println_servo_name(current_servo);
        break;
	  case 'l': for (int i = 0; i < 32; i++)
                {
                 Serial.print("s"); Serial.print(i); Serial.print("("); 
                 Serial.print(servo_name(i)); Serial.print(")=");
                 Serial.print(servo_position[i]);
                 if (i & 1) Serial.println(); else Serial.print("\t");
                }
                break; 
      case 9: look_around();
        break;
      case '@': load_choreography();
        break;
      case 'd': dance();
        break;
      case '?': print_choreography();
                break;
      case ' ': tone(BEEPER, 1760, 200);
                break;
      default: Serial.println((int)c);
    }
  }
}


//---------DFPlayer utilities
// volume 0-30
void mp3_set_volume(uint8_t volume)
{
 mp3_send_packet(0x06, volume);  
}

void mp3_play(uint8_t song_number)
{
 mp3_send_packet(0x03, song_number);  
}

void mp3_send_byte(uint8_t pin, uint8_t val)
{
  pinMode(MP3_OUTPUT_PIN, OUTPUT);
  float start_transmission = micros();
  float one_bit = 1000000 / 9600.0;
  float next_change = start_transmission + one_bit;
  digitalWrite(pin, LOW);
  while (micros() < next_change);
  
  for (int i = 2; i < 10; i++)
  {
    if (val & 1) digitalWrite(pin, HIGH);
    else digitalWrite(pin, LOW);
    next_change = start_transmission + one_bit * i;
    val >>= 1;
    while (micros() < next_change);
  }

  digitalWrite(pin, HIGH);
  next_change = micros() + 2 * one_bit;
  while (micros() < next_change);
  pinMode(MP3_OUTPUT_PIN, INPUT);
}

void mp3_send_packet(uint8_t cmd, uint16_t param)
{
  mp3_send_byte(MP3_OUTPUT_PIN, 0x7E);
  mp3_send_byte(MP3_OUTPUT_PIN, 0xFF);
  mp3_send_byte(MP3_OUTPUT_PIN, 0x06);
  mp3_send_byte(MP3_OUTPUT_PIN, cmd);
  mp3_send_byte(MP3_OUTPUT_PIN, 0x00);
  mp3_send_byte(MP3_OUTPUT_PIN, (uint8_t)(param >> 8));
  mp3_send_byte(MP3_OUTPUT_PIN, (uint8_t)(param & 0xFF));
  uint16_t chksm = 0xFF + 0x06 + cmd + (param >> 8) + (param & 0xFF);
  chksm = -chksm;
  mp3_send_byte(MP3_OUTPUT_PIN, (uint8_t)(chksm >> 8));
  mp3_send_byte(MP3_OUTPUT_PIN, (uint8_t)(chksm & 0xFF));
  mp3_send_byte(MP3_OUTPUT_PIN, 0xEF);
}

