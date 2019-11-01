#include "Quadencoder.h"


uint32_t mCurPosValue;
uint32_t old_position = 0;
uint32_t mCurPosValue1;
uint32_t old_position1 = 0;
QuadEncoder myEnc1(1);  // Encoder on channel 1 of 4 available
QuadEncoder myEnc2(2);  // Encoder on channel 2 of 4 available

void setup()
{
  while(!Serial && millis() < 4000);
  delay(2000);

  /* Initialize the ENC module. */
  myEnc1.begin(0, 1, 0);  //Phase A (pin0), PhaseB(pin1), Pullups Req(0)
  myEnc2.begin(2, 3, 0);  //Phase A (pin2), PhaseB(pin3), Pullups Req(0)

}

void loop(){
  
  /* This read operation would capture all the position counter to responding hold registers. */
  mCurPosValue = myEnc1.getPosition();

  if(mCurPosValue != old_position){
    /* Read the position values. */
    Serial.printf("Current position value1: %ld\r\n", mCurPosValue);
    Serial.printf("Position differential value1: %d\r\n", (int16_t)myEnc1.getHoldDifference());
    Serial.printf("Position revolution value1: %d\r\n", myEnc1.getHoldRevolution());
    Serial.println();
  }

  old_position = mCurPosValue;

  mCurPosValue1 = myEnc2.getPosition();

  if(mCurPosValue1 != old_position1){
    /* Read the position values. */
    Serial.printf("Current position value2: %ld\r\n", mCurPosValue1);
    Serial.printf("Position differential value2: %d\r\n", (int16_t)myEnc2.getHoldDifference());
    Serial.printf("Position revolution value2: %d\r\n", myEnc2.getHoldRevolution());
    Serial.println();
  }

  old_position1 = mCurPosValue1;
}
