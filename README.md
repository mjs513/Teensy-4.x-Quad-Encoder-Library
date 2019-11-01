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
