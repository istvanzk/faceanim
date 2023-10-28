/*
 * Animate a "face" with four LEDs for: eyes, nose and mounth
 * Uses PWM pins 3, 9, 10 and 11 to control four Adafruit White LED Backlight Modules
 * An IR proximity sensor is used to trigger some of the animation steps
 * An external sound generator can be controlled with 6 digital pins
 * 
 * Created by István Z. Kovács, October 2020, updated October 2023
 */

// Enable IR sensor
#define IR_SENSOR
// Enable sound control
#define SOUND_CTRL
// Enable serial debug output
//#define DEBUG_SERIAL

// PWM pins controlling LEDs
int ledEyeLeft  = 10;   
int ledEyeRight = 11;   
int ledNose     = 9;    
int ledMouth    = 3;

// Groups of LEDs
int ledeyes[2] = {ledEyeLeft, ledEyeRight};
int ledall[4] = {ledEyeLeft, ledEyeRight, ledNose, ledMouth};

// DO pins controlling extrenal sound generator (active HIGH)
#if defined(SOUND_CTRL)
int soundGhost = 4;
int soundMusic = 5;
int soundLaugh = 6;
int soundOwl   = 7;
int soundCrow  = 12;
int soundWitch = 13;
// Group of sounds
int soundsall[6] = {soundGhost, soundMusic, soundLaugh, soundOwl, soundCrow, soundWitch};
int randSoundNumber = 0;
#endif

// IR sensor pin
#if defined(IR_SENSOR)
int irSens     = A0;    
unsigned int proxCounter = 0;
#endif

// Set initial state of the animation
//bool eyeLeftOpen  = false;
//bool eyeRightOpen = false;
//bool noseVis      = false;
//bool mouthVis     = false;

// Counters
unsigned int loopCounter   = 0;
long randWinkLoopNumber    = 3;
long randFadeOutLoopNumber = 13;

void setup() {

#if defined(DEBUG_SERIAL)
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("REBOOT started");
#endif

  // Define LED pins
  pinMode(ledEyeLeft, OUTPUT);
  pinMode(ledEyeRight, OUTPUT);
  pinMode(ledNose, OUTPUT);
  pinMode(ledMouth, OUTPUT);
  pinMode(irSens, INPUT);

  // Define sound pins and set them LOW (no SOUND_CTRL)
#if defined(SOUND_CTRL)
  pinMode(soundOwl, OUTPUT);
  digitalWrite(soundOwl, LOW);
  pinMode(soundLaugh, OUTPUT);
  digitalWrite(soundLaugh, LOW);
  pinMode(soundMusic, OUTPUT);
  digitalWrite(soundMusic, LOW);
  pinMode(soundGhost, OUTPUT);
  digitalWrite(soundGhost, LOW);
  pinMode(soundCrow, OUTPUT);
  digitalWrite(soundCrow, LOW);
  pinMode(soundWitch, OUTPUT);
  digitalWrite(soundWitch, LOW);
#endif

  // Dark face
  analogWrite(ledEyeLeft, 0);
  analogWrite(ledEyeRight, 0);
  analogWrite(ledNose, 0);
  analogWrite(ledMouth, 0);

  // If analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  // Intro animation & sound
  face_fade_in(10,100);
  delay(1000);
  scene_face_fade(1, 2000);  
  delay(1000);
#if defined(SOUND_CTRL) 
  sound(soundMusic);
#endif  
}

void loop() {

#if defined(IR_SENSOR)
  // Detect proximity
  int ir = analogRead(irSens);
#if defined(DEBUG_SERIAL)
  Serial.print("IRSensor: ");
  Serial.println(ir);
#endif
  if (ir<100) {
    proxCounter++;
#if defined(DEBUG_SERIAL)
  Serial.print("IRSensor counter: ");
  Serial.println(proxCounter);
#endif
    if (proxCounter > 1){
#if defined(SOUND_CTRL)
      sound(soundWitch);
#endif
      proxCounter = 0;
    }    
  }
#endif
  
  if (loopCounter==0){

    randWinkLoopNumber = random(5,16);
    randFadeOutLoopNumber = random(10,26);

    loopCounter++;
    
  } else if (loopCounter >= randFadeOutLoopNumber){

#if defined(SOUND_CTRL)    
    randSoundNumber = random(0,6);
#if defined(DEBUG_SERIAL)
    Serial.print("RandomSound: ");
    Serial.println(randSoundNumber);
#endif
    sound(soundsall[randSoundNumber]);
    proxCounter = 0;
#endif

    scene_face_fade(1, 3000);
    loopCounter = 0;

  } else {
  
    scene_blink_wink();
    
    loopCounter++;
  }
}


// Scene functions
void scene_face_fade(int num_repeat, int dly_ms)
{
  for(int rep=0; rep < num_repeat; rep++){
    face_fade_out(10,100);
    delay(dly_ms);
    face_fade_in(10,100);
    delay(dly_ms);  
  }
}

void scene_blink_wink()
{
  // Blink
  int len = sizeof(ledeyes)/sizeof(*ledeyes);
  //Serial.print(F("ledeyes len="));
  //Serial.println(len);
  blink(ledeyes, len, 250, 150);
  if (loopCounter % randWinkLoopNumber){
    //delay(3000);
    pulse(&ledMouth, 1, 250, 1);
    delay(1000);
    
  } else {
    // Wink
    delay(1000);
    if(random(0,2)==1)
      blink(&ledEyeRight, 1, 250, 750);
    else
      blink(&ledEyeLeft, 1, 250, 750);
    delay(1000);

    randWinkLoopNumber = random(5,14);
  }
   
}

// Face fade functions
void face_fade_in(int step_fadeValue, int dly_ms) 
{
  for (int fadeValue = 0; fadeValue <= 50; fadeValue += step_fadeValue) {
    analogWrite(ledEyeLeft, fadeValue);
    analogWrite(ledEyeRight, fadeValue);
    delay(dly_ms);
  }  
  for (int fadeValue = 50+step_fadeValue; fadeValue <= 150; fadeValue += step_fadeValue) {
    analogWrite(ledEyeLeft, fadeValue);
    analogWrite(ledEyeRight, fadeValue);
    analogWrite(ledMouth, fadeValue-50);
    delay(dly_ms);
  }  
  for (int fadeValue = 150+step_fadeValue; fadeValue <= 250; fadeValue += step_fadeValue) {
    analogWrite(ledEyeLeft, fadeValue);
    analogWrite(ledEyeRight, fadeValue);
    analogWrite(ledNose, fadeValue-150);
    analogWrite(ledMouth, fadeValue-50);
    delay(dly_ms);
  }  
  for (int fadeValue = 200+step_fadeValue; fadeValue <= 250; fadeValue += step_fadeValue) {
    analogWrite(ledNose, fadeValue-100);
    analogWrite(ledMouth, fadeValue);
    delay(dly_ms);
  }  
  for (int fadeValue = 150+step_fadeValue; fadeValue <= 250; fadeValue += step_fadeValue) {
    analogWrite(ledNose, fadeValue);
    delay(dly_ms);
  }  
}

void face_fade_out(int step_fadeValue, int dly_ms) 
{
  for (int fadeValue = 250; fadeValue >= 150+step_fadeValue; fadeValue -= step_fadeValue) {
    analogWrite(ledNose, fadeValue);
    delay(dly_ms);
  }  
  for (int fadeValue = 250; fadeValue >= 200+step_fadeValue; fadeValue -= step_fadeValue) {
    analogWrite(ledNose, fadeValue-100);
    analogWrite(ledMouth, fadeValue);
    delay(dly_ms);
  }  
  for (int fadeValue = 250; fadeValue >= 150+step_fadeValue; fadeValue -= step_fadeValue) {
    analogWrite(ledEyeLeft, fadeValue);
    analogWrite(ledEyeRight, fadeValue);
    analogWrite(ledNose, fadeValue-150-step_fadeValue);
    analogWrite(ledMouth, fadeValue-50);
    delay(dly_ms);
  }  
  for (int fadeValue = 150; fadeValue >= 50+step_fadeValue; fadeValue -= step_fadeValue) {
    analogWrite(ledEyeLeft, fadeValue);
    analogWrite(ledEyeRight, fadeValue);
    analogWrite(ledMouth, fadeValue-50-step_fadeValue);
    delay(dly_ms);
  }   
  for (int fadeValue = 50; fadeValue >= 0; fadeValue -= step_fadeValue) {
    analogWrite(ledEyeLeft, fadeValue);
    analogWrite(ledEyeRight, fadeValue);
    delay(dly_ms);
  }  
}

// Action functions
void blink(int *ledarr, int lenarr, int startend_fadeValue, int dly_ms)
{
  // Blink = fade out from start_fadeValue to 0 and fade in from 0 to start_fadeValue, with dly_ms in-between fades
  fade_out(ledarr, lenarr, startend_fadeValue, 50, 10);
  delay(dly_ms);
  fade_in(ledarr, lenarr, startend_fadeValue, 60, 10);
}

void pulse(int *ledarr, int lenarr, int startend_fadeValue, int num_repeat)
{
  for (int rep = 0; rep < num_repeat; rep+=1){
    fade_out(ledarr, lenarr, startend_fadeValue, 10, 40);
    fade_in(ledarr, lenarr, startend_fadeValue, 10, 40);
  }

}

// Fade functions
void fade_in(int *ledarr, int lenarr, int end_fadeValue, int step_fadeValue, int dly_ms)
{
  // Fade in, from 0 to end_fadeValue in increments of step_fadeValue points, with dly_ms in-between increments
  for (int fadeValue = 0 ; fadeValue <= end_fadeValue; fadeValue += step_fadeValue) {
    for (int led = 0; led < lenarr; led++)
      analogWrite(ledarr[led], fadeValue);
    delay(dly_ms);
  }  
}

void fade_out(int *ledarr, int lenarr, int start_fadeValue, int step_fadeValue, int dly_ms)
{
  // Fade out, from start_fadeValue to 0 in decrements of step_fadeValue points, with dly_ms in-between decrements
  for (int fadeValue = start_fadeValue ; fadeValue >= 0; fadeValue -= step_fadeValue) {
    for (int led = 0; led < lenarr; led++)
      analogWrite(ledarr[led], fadeValue);
    delay(dly_ms);
  }  
}

// Sound control functions 
// Assumes an external sound generator activated via a digital pin
void sound(int soundpin)
{
#if defined(DEBUG_SERIAL)
  Serial.print("Sound: ");
  Serial.println(soundpin);
#endif
  digitalWrite(soundpin, HIGH);
  delay(500);
  digitalWrite(soundpin, LOW);
  delay(100);
}
