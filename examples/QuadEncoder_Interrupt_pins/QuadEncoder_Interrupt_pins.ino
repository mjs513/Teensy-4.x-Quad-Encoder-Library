#include "QuadEncoder.h"


uint32_t mCurPosValue;
uint32_t old_position = 0;
uint32_t mCurPosValue1;
uint32_t old_position1 = 0;
QuadEncoder myEnc1(1, 0, 1, 0, 4);  // Encoder on channel 1 of 4 available
                                   // Phase A (pin0), PhaseB(pin1), Pullups Req(0)
                             
void setup()
{
  while(!Serial && millis() < 4000);

  /* Initialize the ENC module. */
  myEnc1.setInitConfig();
  myEnc1.EncConfig.INDEXTriggerMode = RISING_EDGE;

  myEnc1.init();
  
}

void loop(){
  
  /* This read operation would capture all the position counter to responding hold registers. */
  mCurPosValue = myEnc1.read();

  if(mCurPosValue != old_position){
    /* Read the position values. */
    Serial.printf("Current position value1: %ld\r\n", mCurPosValue);
    Serial.printf("Position differential value1: %d\r\n", (int16_t)myEnc1.getHoldDifference());
    Serial.printf("Position HOLD revolution value1: %d\r\n", myEnc1.getHoldRevolution());
    Serial.printf("Index Counter: %d\r\n", myEnc1.indexCounter);
    Serial.println();
  }

  old_position = mCurPosValue;

}
