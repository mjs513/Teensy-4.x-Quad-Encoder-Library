#include "QuadEncoder.h"


uint32_t mCurPosValue;
uint32_t old_position = 0;
uint32_t mCurPosValue1;
uint32_t old_position1 = 0;
QuadEncoder myEnc1(1, 0, 1, 0);  // Encoder on channel 1 of 4 available
                                 // Phase A (pin0), PhaseB(pin1), Pullups Req(0)
QuadEncoder myEnc2(2, 2, 3, 0);  // Encoder on channel 2 of 4 available
                                 //Phase A (pin2), PhaseB(pin3), Pullups Req(0)
                                 
void setup()
{
  while(!Serial && millis() < 4000);

  /* Initialize the ENC module. */
  myEnc1.setInitConfig();  //
  myEnc1.EncConfig.revolutionCountCondition = ENABLE;
  myEnc1.EncConfig.enableModuloCountMode = ENABLE;
  myEnc1.EncConfig.positionModulusValue = 20; 
  // with above settings count rev every 20 ticks
  // if myEnc1.EncConfig.revolutionCountCondition = ENABLE;
  // is not defined or set to DISABLE, the position is zeroed every
  // 20 counts, if enabled revolution counter is incremented when 
  // phaseA ahead of phaseB, and decrements from 65535 when reversed.
  myEnc1.init();
  
  myEnc2.setInitConfig();  //
  myEnc2.EncConfig.positionInitialValue = 160;
  myEnc2.EncConfig.positionMatchMode = ENABLE;
  myEnc2.EncConfig.positionCompareValue = 200;
  myEnc2.EncConfig.filterCount = 5;
  myEnc2.EncConfig.filterSamplePeriod = 255;
  myEnc2.init();
}

void loop(){
  
  /* This read operation would capture all the position counter to responding hold registers. */
  mCurPosValue = myEnc1.read();

  if(mCurPosValue != old_position){
    /* Read the position values. */
    Serial.printf("Current position value1: %ld\r\n", mCurPosValue);
    Serial.printf("Position differential value1: %d\r\n", (int16_t)myEnc1.getHoldDifference());
    Serial.printf("Position HOLD revolution value1: %d\r\n", myEnc1.getHoldRevolution());
    Serial.println();
  }

  old_position = mCurPosValue;

  mCurPosValue1 = myEnc2.read();

  if(myEnc2.compareValueFlag == 1) {
    //myEnc2.init();
    //resets counter to positionInitialValue so compare 
    //will hit every 200
    myEnc2.write(myEnc2.EncConfig.positionInitialValue);
    Serial.print("Compare Value Hit for Encoder 2:  ");
    Serial.println(myEnc2.compareValueFlag);
    Serial.println();
    myEnc2.compareValueFlag = 0;
  }

  if(mCurPosValue1 != old_position1){
    /* Read the position values. */
    Serial.printf("Current position value2: %ld\r\n", mCurPosValue1);
    Serial.printf("Position differential value2: %d\r\n", (int16_t)myEnc2.getHoldDifference());
    Serial.printf("Position revolution value2: %d\r\n", myEnc2.getHoldRevolution());
    Serial.println();
  }

  old_position1 = mCurPosValue1;
}
