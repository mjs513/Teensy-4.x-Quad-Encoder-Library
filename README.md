# Hardware Quadrature Library for the Teensy 4.x
Library based on NXP Quad Encoder SDK driver for the Encoder module but modified to support the Teensy 4.x infrastructure.

There are 4 hardware quadrature encoder channels available the Teensy 4.x.  The Teensy 4.x Encoders are supported on pins: 0, 1, 2, 3, 4, 5, 7, 8, 30, 31 and 33. On the T4.1 the following additional pins are supported: 36 and 37.

WARNING! Pins 0, 5 and 37 share the same internal crossbar connections and are as such exclusive...pick one or the other.  Same thing applies to pins 1 / 36 and 5 / 37.

For Teensy 4.0: pins 0-8, 30, 31, 33 are supported.
For Teensy 4.1: pins 0-8, 30, 31, 33 and pin 37 are supported.
For Teensy Micromod: pins 0-8, 30, 31, 33, 36 and 37 are supported

The constuctor is designed to tell the library what encoder channel, PhaseA and PhaseB pins to use as well as whether to use pullups on those pins.  Example:
```c++
QuadEncoder myEnc1(1, 0, 1, 0);  // Encoder on channel 1 of 4 available
                                 // Phase A (pin0), PhaseB(pin1), Pullups Req(0)
QuadEncoder myEnc2(2, 2, 3, 0);  // Encoder on channel 2 of 4 available
                                 //Phase A (pin2), PhaseB(pin3), Pullups Req(0)
```
The full constructor allows for the INDEX, HOME and TRIGGER pins if available:
```c++
QuadEncoder(uint8_t encoder_ch = 255, uint8_t PhaseA_pin = 255, uint8_t PhaseB_pin = 255, uint8_t pin_pus = 0, uint8_t index_pin = 255, uint8_t home_pin = 255, uint8_t trigger_pin = 255);
```

In setup the encoder is initialized as:
```c++
  myEnc1.setInitConfig();  //Loads default configuration for the encoder channel
  myEnc1.init();           //Initializers the encoder for the channel selected
 ```
To access counts in loop:
```c++ 
    mCurPosValue = myEnc1.read();
    Serial.printf("Current position value1: %ld\r\n", mCurPosValue);
    Serial.printf("Position differential value1: %d\r\n", (int16_t)myEnc1.getHoldDifference());
``` 
all you have to call is ```getPosition()``` while ```myEnc1.getHoldDifference()``` shows direction.

Another example is the ability to change initial parameters once they are loaded:
```c++
  myEnc2.setInitConfig();  //
  myEnc2.EncConfig.positionInitialValue =100;
  myEnc2.init();
  ```
In this case ```myEnc2.EncConfig.positionInitialValue``` changes the starting value for the position counts.

Current available parameters:
```c++
		/* Basic counter. */
		bool enableReverseDirection;
		bool decoderWorkMode; // 0 = Normal mode, 1 = PHASEA input generates a count signal while PHASEB input control the direction. 

		/* Signal detection. */
		uint8_t HOMETriggerMode;   //0 - disable, 1 - rising, 2 - falling
		uint8_t INDEXTriggerMode; //0 - disabled, 1 - Use positive going edge-to-trigger initialization of position counters!, 2 - use falling
		bool clearCounter;  
		bool clearHoldCounter; 

		/* Filter for PHASEA, PHASEB, INDEX and HOME. */
		/* Input Filter Sample Count. This value should be chosen to reduce the probability of noisy samples causing an incorrect transition to be recognized. The value represent the number of consecutive samples that must agree prior to the input filter accepting an  input transition. A value of 0x0 represents 3 samples. A value of 0x7 represents 10 samples. The Available range is 0 - 7. */
		uint16_t filterCount; 

		/* Input Filter Sample Period. This value should be set such that the sampling period is larger than the period of the expected noise. This value represents the sampling period (in IPBus clock cycles) of the decoder input signals.	The available range is 0 - 255. */
		uint16_t filterSamplePeriod; 

		/* Position compare. */
		/* 0 - POSMATCH pulses when a match occurs between the	position counters (POS) and the compare value (COMP). 1 - POSMATCH pulses when any position counter register is read. */
		bool positionMatchMode;
		
		/* Position Compare Enabled. */
		bool positionCompareMode;   **<< NEW examples updated **
		/*!< Position compare value. The available value is a 32-bit number.*/
		uint32_t positionCompareValue;   

		/* Modulus counting. */
		/*0 - Use INDEX pulse to increment/decrement revolution counter. 1 - Use modulus counting roll-over/under to increment/decrement revolution counter. */ 
		bool revolutionCountCondition; 	
								
		bool enableModuloCountMode;     //Enable Modulo Counting. */
		
		/*Position modulus value. This value would be available only when	"enableModuloCountMode" = true. The available value is a 32-bit number. */
		uint32_t positionModulusValue;  
		
		//Position initial value. The available value is a 32-bit number. */
		uint32_t positionInitialValue;  
   ```
   
A couple of things to note when using the INDEX or the HOME triggers are used:

1. If one of the two trigger pins are used while ```INDEXTriggerMode``` is ```DISABLED``` in the configuration structure the position counts will continue to increment while the "Position HOLD revolution value" will increment when the ```index``` pulse is seen on the pin.
2. If ```INDEXTriggerMode``` is set to ```RISING_EDGE``` or ```FALLING_EDGE`` the associated interrupt will fire and increment the ```indexCounter``` but the position counts will be reset to zero.
3. This applies to the HOME trigger as well.
4. If indexTrigger = ENABLE and INDEXTriggerMode = DISABLE (default).  The encoder count will continue increase with no reset while the indexCounter with increment when trigger by the index signal (needs to be negative trigger) and the Position HOLD revolution value will increment or decrement depending on direction.
5. If indexTrigger = ENABLE and INDEXTriggerMode = ENABLE.  The encoder count will continue reset and the indexCounter with increment when trigger by the index signal (needs to be negative trigger) and the Position HOLD revolution value will increment or decrement depending on direction.
6. If indexTrigger = DISABLE (default) and INDEXTriggerMode = ENABLE. The encoder count will continue reset and the indexCounter will not increment with index signal (needs to be negative trigger) and the Position HOLD revolution value will increment or decrement depending on direction.
7. items 4, 5, and 6 apply for the home trigger signal (needs to be positive) as well

## Basic Usage for the Teensy 4.x
Basic usage for the T4 is similar to the current Teensy Encoder library.

```
Encoder myEnc(enc, pin1, pin2); 
   enc - encoder object (specify 1, 2, 3 or 4).
   pin1 - phase A
   pin2 - phase b
myEnc.read();
   Returns the accumulated position. This number can be positive or negative. 
myEnc.write(newPosition);
   Set the accumulated position to a new number. 
 ```
## Example Program 
(similar to example on https://www.pjrc.com/teensy/td_libs_Encoder.html. See SimpleEncoder.ino in the examples folder.
 ```/* Teensy 4 H/S Encoder Library - TwoKnobs Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

 #include "Quadencoder.h"

// Change these pin numbers to the pins connected to your encoder.
// Allowable encoder pins:
// 0, 1, 2, 3, 4, 5, 7, 30, 31 and 33
// Encoder on channel 1 of 4 available
// Phase A (pin0), PhaseB(pin1), 
QuadEncoder knobLeft(1, 0, 1);
// Encoder on channel 2 of 4 available
//Phase A (pin2), PhaseB(pin3), Pullups Req(0)
QuadEncoder knobRight(2, 2, 3);
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(9600);
  Serial.println("TwoKnobs Encoder Test:");
  /* Initialize Encoder/knobLeft. */
  knobLeft.setInitConfig();
  //Optional filter setup
  //knobLeft.EncConfig.filterCount = 5;
  //knobLeft.EncConfig.filterSamplePeriod = 255;
  knobLeft.init();
  /* Initialize Encoder/knobRight. */
  knobRight.setInitConfig();
  //knobRight.EncConfig.filterCount = 5;
  //knobRight.EncConfig.filterSamplePeriod = 255;
  knobRight.init();
}

long positionLeft  = -999;
long positionRight = -999;

void loop() {
  long newLeft, newRight;
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  if (newLeft != positionLeft || newRight != positionRight) {
    Serial.print("Left = ");
    Serial.print(newLeft);
    Serial.print(", Right = ");
    Serial.print(newRight);
    Serial.println();
    positionLeft = newLeft;
    positionRight = newRight;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);
  }
}
```
