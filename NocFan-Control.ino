// The MIT License (MIT)
// 
// Copyright (c) 2015 Benedikt Scholtes
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//   
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define DELAY            100  // Delay per loop in ms

const int fanPin = 10;      // OC1B
const int pulsePin = 4;     // Fan Speed 
const int buttonPin = 2;    // Button
const unsigned char maxFanSpeed = 80;   // this is calculated as 16MHz divided by 8 (prescaler), divided by 25KHz (target PWM frequency from Intel specification)

// List of Fan Speeds
// List of Fan Speeds
int speedValues[] = {20, 35, 50, 80};
int curSpeed = 0;
long previousMillis = 0; 
long interval = 2000; 
 
// Used to debounce Button
boolean button_was_pressed;
unsigned long pulseDuration;

void setup()
{
// Source: http://forum.arduino.cc/index.php?topic=109403#msg821808 

// Analog output (i.e PWM) pins. These must be chosen so that we can change the PWM frequency without affecting the millis()
// function or the MsTimer2 library. So don't use timer/counter 1 or 2. See comment in setup() function.
// THESE PIN NUMBERS MUST NOT BE CHANGED UNLESS THE CODE IN setup() AND setFanSpeed() IS CHANGED TO MATCH!
// On the Uno we can only use the OC1B pin
//
// Set up the PWM pins for a PWM frequency close to the recommended 25KHz for the Intel fan spec.
// We can't get this frequency using the default TOP count of 255, so we have to use a custom TOP value. 
// Only timer/counter 1 is free because TC0 is used for system timekeeping (i.e. millis() function),
// and TC2 is used for our 1-millisecond tick. TC1 controls the PWM on Arduino pins 9 and 10.
// However, we can only get PWM on pin 10 (controlled by OCR1B) because we are using OCR1A to define the TOP value.
// Using a prescaler of 8 and a TOP value of 80 gives us a frequency of 16000/(8 * 80) = 25KHz exactly.
    TCCR1A = (1 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (1 << WGM10);  // OC1A (pin 9) disconnected, OC1B (pin 10) = inverted fast PWM  
    OCR1AH = 0;
    OCR1AL = 79;  // TOP = 79
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);  // TOP = OCR0A, prescaler = 8

    OCR1BH = 0;
    OCR1BL = maxFanSpeed;  // max fan speed 

    TCNT1H = 0;
    TCNT1L = 0; 
// -----

    pinMode(fanPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    pinMode(pulsePin, INPUT);
    digitalWrite(buttonPin, HIGH);
    digitalWrite(pulsePin, HIGH);

    button_was_pressed = false;
    Serial.begin(9600);
}

// Set the fan speed, where 0 <= fanSpeed <= maxFanSpeed
void setTransistorFanSpeed(unsigned char fanSpeed)
{
 OCR1BH = 0;
 OCR1BL = fanSpeed;
}

boolean handle_button()
{
  boolean event;
  int button_now_pressed = !digitalRead(buttonPin); // pin low -> pressed

  event = button_now_pressed && !button_was_pressed;
  button_was_pressed = button_now_pressed;
  return event;
}

void loop()
{

    // Print RPM every 2 Seconds
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
        // save the last time you blinked the LED 
        previousMillis = currentMillis; 
        pulseDuration = pulseIn(pulsePin, LOW);
        double frequency = 1000000/pulseDuration;
        Serial.print("RPM:");
        Serial.println(frequency/2*60);
        Serial.println(sizeof(speedValues));
    }
    // Cycle through the different Values
    if (handle_button()) {
        if (curSpeed == (4 - 1)) {
            
            curSpeed = 0;
        } else {
            curSpeed++;
        }
        setTransistorFanSpeed(speedValues[curSpeed]);
        Serial.println(speedValues[curSpeed]);
    }
    delay(DELAY);

}
