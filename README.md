# Teensy-4.x-Quad-Encoder-Library
Hardware Quadrature Library for the Teensy 4.x

Library based on NXP Quad Encoder SDK driver for the Encoder module but modified to support the Teensy 4.x infrastructure.

There are 4 hardware quadrature encoder channels available the Teensy 4.0.  These are supported on pins: 0, 1, 2, 3, 4, 5 and 7.

The constuctor is designed to tell the library what encoder channel, PhaseA and PhaseB pins to use as well as whether to use pullups on those pins.  Example:
``` 
QuadEncoder myEnc1(1, 0, 1, 0);  // Encoder on channel 1 of 4 available
                                 // Phase A (pin0), PhaseB(pin1), Pullups Req(0)
QuadEncoder myEnc2(2, 2, 3, 0);  // Encoder on channel 2 of 4 available
                                 //Phase A (pin2), PhaseB(pin3), Pullups Req(0)
```

In setup the encoder is initialized as:
```
  myEnc1.setInitConfig();  //Loads default configuration for the encoder channel
  myEnc1.init();           //Initializers the encoder for the channel selected
 ```
To access counts in loop:
``` 
    mCurPosValue = myEnc1.getPosition();
    Serial.printf("Current position value1: %ld\r\n", mCurPosValue);
    Serial.printf("Position differential value1: %d\r\n", (int16_t)myEnc1.getHoldDifference());
``` 
all you have to call is ```getPosition()``` while ```myEnc1.getHoldDifference()``` shows direction.

Another example is the ability to change initial parameters once they are loaded:
```
  myEnc2.setInitConfig();  //
  myEnc2.EncConfig.positionInitialValue =100;
  myEnc2.init();
  ```
In this case ```myEnc2.EncConfig.positionInitialValue``` changes the starting value for the position counts.

Current available parameters:
```
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
		
		/*!< Position compare value. The available value is a 32-bit number.*/
		uint32_t positionCompareValue;   

		/* Modulus counting. */
		/*0 - Use INDEX pulse to increment/decrement revolution counter. 1 - Use modulus counting roll-over/under to increment/decrement revolution counter. */ 
		bool revolutionCountCondition; 	
								
		bool enableModuloCountMode;     //Enable Modulo Counting. */
		
		/*Position modulus value. This value would be available only when			"enableModuloCountMode" = true. The available value is a 32-bit number. */
		uint32_t positionModulusValue;  
		
		//Position initial value. The available value is a 32-bit number. */
		uint32_t positionInitialValue; 
    ```
