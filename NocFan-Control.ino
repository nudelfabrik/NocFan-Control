
// Analog output (i.e PWM) pins. These must be chosen so that we can change the PWM frequency without affecting the millis()
// function or the MsTimer2 library. So don't use timer/counter 1 or 2. See comment in setup() function.
// THESE PIN NUMBERS MUST NOT BE CHANGED UNLESS THE CODE IN setup() AND setFanSpeed() IS CHANGED TO MATCH!
// On the Uno we can only use the OC1B pin

const int fanPin = 10;     // OC1B
const unsigned char maxFanSpeed = 80;   // this is calculated as 16MHz divided by 8 (prescaler), divided by 25KHz (target PWM frequency from Intel specification)
int fanPulse = 4;

#define BUTTON_PIN        2  // Button
#define DELAY            100  // Delay per loop in ms
boolean button_was_pressed;
int values[] = {20, 35, 50, 80};
int curSpeed = 0;
long previousMillis = 0; 
long interval = 2000; 
unsigned long pulseDuration;

void setup()
{
 // Set up the PWM pins for a PWM frequency close to the recommended 25KHz for the Intel fan spec.
 // We can't get this frequency using the default TOP count of 255, so we have to use a custom TOP value. 
 // Only timer/counter 1 is free because TC0 is used for system timekeeping (i.e. millis() function),
 // and TC2 is used for our 1-millisecond tick. TC1 controls the PWM on Arduino pins 9 and 10.
 // However, we can only get PWM on pin 10 (controlled by OCR1B) because we are using OCR1A to define the TOP value.
 // Using a prescaler of 8 and a TOP value of 80 gives us a frequency of 16000/(8 * 80) = 25KHz exactly.
 TCCR1A = (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10);  // OC1A (pin 9) disconnected, OC1B (pin 10) = inverted fast PWM  
#ifdef FAN_AUDIO_TEST
 // test code to get 440Hz output (= concert A) to test the logic 
 OCR1AH = 0;
 OCR1BL = 71;  // 50% duty cycle 
 TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12);  // TOP = OCRA, prescaler = 256

 OCR1AL = 141; // TOP = 141, 16000000 / (256 * 142) = 440.014
 OCR1BH = 0;
#else
 OCR1AH = 0;
 OCR1AL = 79;  // TOP = 79
 TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);  // TOP = OCR0A, prescaler = 8

 OCR1BH = 0;
 OCR1BL = maxFanSpeed;  // max fan speed 
#endif

 TCNT1H = 0;
 TCNT1L = 0; 

 // We have to enable the ports as outputs before PWM will work.
 pinMode(fanPin, OUTPUT);
 
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH); // pull-up
    button_was_pressed = false;
      Serial.begin(9600);
      

pinMode(fanPulse, INPUT);
digitalWrite(fanPulse,HIGH);


}

boolean handle_button()
{
  boolean event;
  int button_now_pressed = !digitalRead(BUTTON_PIN); // pin low -> pressed

  event = button_now_pressed && !button_was_pressed;
  button_was_pressed = button_now_pressed;
  return event;
}
// Set the fan speed, where 0 <= fanSpeed <= maxFanSpeed
void setTransistorFanSpeed(unsigned char fanSpeed)
{
 OCR1BH = 0;
 OCR1BL = fanSpeed;
}

void loop()
{
    //boolean raising_edge = handle_button();
      unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
      // save the last time you blinked the LED 
      previousMillis = currentMillis; 
      pulseDuration = pulseIn(fanPulse, LOW);
      double frequency = 1000000/pulseDuration;
      Serial.print("RPM:");
      Serial.println(frequency/2*60);
  }
    if (handle_button()) {
      if (curSpeed == 3) {
        curSpeed = 0;
      } else {
        curSpeed++;
      }
      setTransistorFanSpeed(values[curSpeed]);
      Serial.println(values[curSpeed]);
    }
    delay(DELAY);

}
