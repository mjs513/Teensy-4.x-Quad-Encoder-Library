#include "Arduino.h"
#include "QuadEncoder.h"

//#define DEBUG_OUTPUT

const QuadEncoder::ENC_Channel_t QuadEncoder::channel[] = {	
	{0,&IMXRT_ENC1, IRQ_ENC1, isrEnc1, 66, 67, 68, 69, 70,&CCM_CCGR4,CCM_CCGR4_ENC1(CCM_CCGR_ON)},  //this is a dummy entry - use 1-4 for channels
	{1, &IMXRT_ENC1, IRQ_ENC1, isrEnc1, 66, 67, 68, 69, 70,&CCM_CCGR4,CCM_CCGR4_ENC1(CCM_CCGR_ON)},
	{2, &IMXRT_ENC2, IRQ_ENC2, isrEnc2, 71, 72, 73, 74, 75,&CCM_CCGR4,CCM_CCGR4_ENC2(CCM_CCGR_ON)},
	{3, &IMXRT_ENC3, IRQ_ENC3, isrEnc3, 76, 77, 78, 79, 80,&CCM_CCGR4,CCM_CCGR4_ENC3(CCM_CCGR_ON)},
	{4, &IMXRT_ENC4, IRQ_ENC4, isrEnc4, 81, 82, 83, 84, 95,&CCM_CCGR4,CCM_CCGR4_ENC4(CCM_CCGR_ON)}
};
const uint8_t QuadEncoder::_channel_count =  (sizeof(QuadEncoder::channel)/sizeof(QuadEncoder::channel[0]));

//xbara1 pin config
//pin, idx, *reg, alt
#ifdef ARDUINO_TEENSY40
const  QuadEncoder::ENC_Hardware_t QuadEncoder::hardware[] = {	
	{0, 0, &CORE_XIO_PIN0, 1, 17, 1},	{1, 1, &CORE_XIO_PIN1, 1, 16, 0},
	{2, 2, &CORE_XIO_PIN2, 3, 6, 0},	{3, 3, &CORE_XIO_PIN3, 3, 7, 0},
	{4, 4, &CORE_XIO_PIN4,3, 8, 0},		{5, 5, &CORE_XIO_PIN5, 3, 17, 0},
	{7, 7, &CORE_XIO_PIN7, 1, 15, 1},	{8, 8, &CORE_XIO_PIN8, 1, 14, 1},
	{9, 30, &CORE_XIO_PIN30, 1, 23, 0},	{10, 31, &CORE_XIO_PIN31, 1, 22, 0},
	{11, 33, &CORE_XIO_PIN33, 3, 9, 0}
};
#endif
#ifdef ARDUINO_TEENSY41
const  QuadEncoder::ENC_Hardware_t QuadEncoder::hardware[] = {	
	{0, 0, &CORE_XIO_PIN0, 1, 17, 1},	{1, 1, &CORE_XIO_PIN1, 1, 16, 0},
	{2, 2, &CORE_XIO_PIN2, 3, 6, 0},	{3, 3, &CORE_XIO_PIN3, 3, 7, 0},
	{4, 4, &CORE_XIO_PIN4,3, 8, 0},		{5, 5, &CORE_XIO_PIN5, 3, 17, 0},
	{7, 7, &CORE_XIO_PIN7, 1, 15, 1},	{8, 8, &CORE_XIO_PIN8, 1, 14, 1},
	{9, 30, &CORE_XIO_PIN30, 1, 23, 0},	{10, 31, &CORE_XIO_PIN31, 1, 22, 0},
	{11, 33, &CORE_XIO_PIN33, 3, 9, 0},	{12, 36, &CORE_XIO_PIN36, 1, 16, 1},
	{13, 37, &CORE_XIO_PIN37, 1, 17, 3}

};
#endif
const uint8_t QuadEncoder::_hardware_count =  (sizeof(QuadEncoder::hardware)/sizeof(QuadEncoder::hardware[0]));

QuadEncoder * QuadEncoder::list[5];

  uint8_t QuadEncoder::compareValueFlag;
 uint32_t QuadEncoder::homeCounter;
 uint32_t QuadEncoder::indexCounter;

 
void QuadEncoder::setInitConfig() {
  getConfig1(&EncConfig);
}

void QuadEncoder::init()
{
  Init(&EncConfig);
  setConfigInitialPosition();
  enableInterrupts(&EncConfig);
}


QuadEncoder::QuadEncoder(uint8_t encoder_ch, uint8_t PhaseA_pin, uint8_t PhaseB_pin, uint8_t pin_pus, uint8_t index_pin, uint8_t home_pin, uint8_t trigger_pin){
	if(encoder_ch >= 0){
	  _encoder_ch = encoder_ch;
	
	CCM_CCGR2 |= CCM_CCGR2_XBAR1(CCM_CCGR_ON);   //turn clock on for xbara1
  
#ifdef DEBUG_OUTPUT
	Serial.printf("begin: encoder channel-> %d\n",_encoder_ch);
	Serial.printf("begin: pinA-> %d, pinB-> %d\n", PhaseA_pin, PhaseB_pin);
#endif
	  if(PhaseA_pin != 255 )
		enc_xbara_mapping(PhaseA_pin, PHASEA, pin_pus);
	  if(PhaseA_pin != 255 )
		enc_xbara_mapping(PhaseB_pin, PHASEB, pin_pus);
	  if(home_pin != 255 )
		 enc_xbara_mapping(home_pin, HOME, pin_pus);
	  if(index_pin != 255 )
		 enc_xbara_mapping(index_pin, INDEX, pin_pus);
	  if(trigger_pin != 255 )
		 enc_xbara_mapping(trigger_pin, TRIGGER, pin_pus);
	} else {
	  Serial.println("No Encoder Channel Selected!");
	}
	
	disableInterrupts(_positionROEnable);
	disableInterrupts(_positionRUEnable);
}

void QuadEncoder::getConfig1(enc_config_t *config)
{
    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableReverseDirection = DISABLE;
    config->decoderWorkMode = DISABLE;
    config->HOMETriggerMode = DISABLE;
    config->INDEXTriggerMode = DISABLE;
	config->IndexTrigger = DISABLE;
	config->HomeTrigger = DISABLE;
    config->clearCounter = DISABLE;
    config->clearHoldCounter = DISABLE;
    config->filterCount = 0;
    config->filterSamplePeriod = 0;
    config->positionMatchMode = DISABLE;
    config->positionCompareValue = 0xffffffff;
    config->revolutionCountCondition = DISABLE;
    config->enableModuloCountMode = DISABLE;
    config->positionModulusValue = 0;
    config->positionInitialValue = 0;
	config->positionROIE = DISABLE;
	config->positionRUIE = DISABLE;
	
}

void QuadEncoder::printConfig(enc_config_t *config)
{
	Serial.printf("\tenableReverseDirection: %d\n",config->enableReverseDirection);
	Serial.printf("\tdecoderWorkMode: %d\n",config->decoderWorkMode);
	Serial.printf("\tHOMETriggerMode: %d\n",config->HOMETriggerMode);
	Serial.printf("\tINDEXTriggerMode: %d\n",config->INDEXTriggerMode);
	Serial.printf("\tIndexTrigger: %d\n",config->IndexTrigger);
	Serial.printf("\tHomeTrigger: %d\n",config->HomeTrigger);	
	
	Serial.printf("\tclearCounter: %d\n",config->clearCounter);
	Serial.printf("\tclearHoldCounter: %d\n",config->clearHoldCounter);
	
	Serial.printf("\tfilterCount: %d\n",config->filterCount);
	Serial.printf("\tfilterSamplePeriod: %d\n",config->filterSamplePeriod);
	Serial.printf("\tpositionCompareValue: %x\n",config->positionCompareValue);
	Serial.printf("\trevolutionCountCondition: %d\n",config->clearCounter);
	Serial.printf("\tenableModuloCountMode: %d\n",config->clearHoldCounter);
	
	Serial.printf("\tpositionInitialValue: %d\n",config->positionInitialValue);
	Serial.printf("\tpositionROIE: %d\n",config->positionROIE);
	Serial.printf("\tpositionRUIE: %x\n",config->positionRUIE);
	Serial.printf("\n");
}

void QuadEncoder::setConfigInitialPosition()
{
    uint16_t tmp16 = channel[_encoder_ch].ENC->CTRL & (uint16_t)(~ENC_CTRL_W1C_FLAGS);

    tmp16 |= ENC_CTRL_SWIP_MASK; /* Write 1 to trigger the command for loading initial position value. */
    channel[_encoder_ch].ENC->CTRL = tmp16;
}

void QuadEncoder::Init(const enc_config_t *config)
{
    uint32_t tmp16;

	// make sure the appropriate clock gate is enabled.
	*channel[_encoder_ch].clock_gate_register |= channel[_encoder_ch].clock_gate_mask;

    /* ENC_CTRL. */
    tmp16 = channel[_encoder_ch].ENC->CTRL & (uint16_t)(~(ENC_CTRL_W1C_FLAGS | ENC_CTRL_HIP_MASK | ENC_CTRL_HNE_MASK | ENC_CTRL_REV_MASK | ENC_CTRL_PH1_MASK | ENC_CTRL_XIP_MASK | ENC_CTRL_XNE_MASK | ENC_CTRL_WDE_MASK));
	
    /* For HOME trigger. */
    if (config->HOMETriggerMode != DISABLE)
    {
        tmp16 |= ENC_CTRL_HIP_MASK;
        if (FALLING_EDGE == config->HOMETriggerMode)
        {
            tmp16 |= ENC_CTRL_HNE_MASK;
        }
    }
	
    /* For encoder work mode. */
    if (config->enableReverseDirection)
    {
        tmp16 |= ENC_CTRL_REV_MASK;
    }
    if (true == config->decoderWorkMode)
    {
        tmp16 |= ENC_CTRL_PH1_MASK;
    }
    /* For INDEX trigger. */
    if (DISABLE != config->INDEXTriggerMode)
    {
        tmp16 |= ENC_CTRL_XIP_MASK;
        if (FALLING_EDGE == config->INDEXTriggerMode)
        {
            tmp16 |= ENC_CTRL_XNE_MASK;
        }
    }

    channel[_encoder_ch].ENC->CTRL = tmp16;
	
    /* ENC_FILT. */
    channel[_encoder_ch].ENC->FILT = ENC_FILT_FILT_CNT(config->filterCount) | ENC_FILT_FILT_PER(config->filterSamplePeriod);

    /* ENC_CTRL2. */
    tmp16 = channel[_encoder_ch].ENC->CTRL2 & (uint16_t)(~(ENC_CTRL2_W1C_FLAGS | ENC_CTRL2_OUTCTL_MASK | ENC_CTRL2_REVMOD_MASK | ENC_CTRL2_MOD_MASK | ENC_CTRL2_UPDPOS_MASK | ENC_CTRL2_UPDHLD_MASK | ENC_CTRL2_ROIRQ_MASK | ENC_CTRL2_RUIRQ_MASK));
	
    if (1 == config->positionMatchMode)
    {
        tmp16 |= ENC_CTRL2_OUTCTL_MASK;
    }
    if (1 == config->revolutionCountCondition)
    {
        tmp16 |= ENC_CTRL2_REVMOD_MASK;
    }
    if (config->enableModuloCountMode)
    {
        tmp16 |= ENC_CTRL2_MOD_MASK;
        /* Set modulus value. */
        channel[_encoder_ch].ENC->UMOD = (uint16_t)(config->positionModulusValue >> 16U); /* Upper 16 bits. */
        channel[_encoder_ch].ENC->LMOD = (uint16_t)(config->positionModulusValue);        /* Lower 16 bits. */
    }
    if (config->clearCounter)
    {
        tmp16 |= ENC_CTRL2_UPDPOS_MASK;
    }
    if (config->clearHoldCounter)
    {
        tmp16 |= ENC_CTRL2_UPDHLD_MASK;
    }
    channel[_encoder_ch].ENC->CTRL2 = tmp16;

    /* ENC_UCOMP & ENC_LCOMP. */
    channel[_encoder_ch].ENC->UCOMP = (uint16_t)(config->positionCompareValue >> 16U); /* Upper 16 bits. */
    channel[_encoder_ch].ENC->LCOMP = (uint16_t)(config->positionCompareValue);        /* Lower 16 bits. */

    /* ENC_UINIT & ENC_LINIT. */
    channel[_encoder_ch].ENC->UINIT = (uint16_t)(config->positionInitialValue >> 16U); /* Upper 16 bits. */
    channel[_encoder_ch].ENC->LINIT = (uint16_t)(config->positionInitialValue);        /* Lower 16 bits. */
	
}

int32_t QuadEncoder::read()
{
    uint32_t ret32;

    ret32 = channel[_encoder_ch].ENC->UPOS; /* Get upper 16 bits and make a snapshot. */
    ret32 <<= 16U;
    ret32 |= channel[_encoder_ch].ENC->LPOSH; /* Get lower 16 bits from hold register. */

    return (int32_t) ret32;
}

void QuadEncoder::write(uint32_t value)
{
    channel[_encoder_ch].ENC->UINIT = (uint16_t)(value >> 16U); /* Set upper 16 bits. */
    channel[_encoder_ch].ENC->LINIT = (uint16_t)(value);        /* Set lower 16 bits. */
	
	setConfigInitialPosition();
}

uint32_t QuadEncoder::getHoldPosition()
{
    uint32_t ret32;

    ret32 = channel[_encoder_ch].ENC->UPOSH; /* Get upper 16 bits and make a snapshot. */
    ret32 <<= 16U;
    ret32 |= channel[_encoder_ch].ENC->LPOSH; /* Get lower 16 bits from hold register. */

    return ret32;
}


uint16_t QuadEncoder::getPositionDifference()
{
    return channel[_encoder_ch].ENC->POSD;
}


uint16_t QuadEncoder::getHoldDifference()
{
    return channel[_encoder_ch].ENC->POSDH;
}


uint16_t QuadEncoder::getRevolution()
{
    return channel[_encoder_ch].ENC->REV;
}

uint16_t QuadEncoder::getHoldRevolution()
{
    return channel[_encoder_ch].ENC->REVH;
}

void QuadEncoder::enc_xbara_mapping(uint8_t pin, uint8_t PHASE, uint8_t PUS){ 

  const struct digital_pin_bitband_and_config_table_struct *p;

  for (int idx_channel = 0; idx_channel < _hardware_count; idx_channel++) {
	if (hardware[idx_channel].pin == pin) {
		_pin_idx = idx_channel;
		break;	
	}
  }
#ifdef DEBUG_OUTPUT
  Serial.printf("\nxbara_mapping: pin-> %d, idx-> %d\n", pin, _pin_idx);
  Serial.printf("xbara_mapping: hw_count-> %d\n",_hardware_count); 
  Serial.printf("xbara_mapping: hardware[_pin_idx].pin-> %d, .select_val-> %d\n", hardware[_pin_idx].pin, hardware[_pin_idx].select_val);
  Serial.printf("xbara_mapping: Encoder-> %d\n", _encoder_ch);
#endif

  if (_pin_idx == _hardware_count) return;
    
  p = digital_pin_to_info_PGM + hardware[_pin_idx].pin;
  //mux is ctrl config for pin
  //pad is pad config
  //pinmode = *(p->reg + 1);
  
  //Pin ctrl configuration for encoder/xbara1
    *(p->mux) = hardware[_pin_idx].select_val;
  
  //Pad configuration for encoder/xbara1
  if(PUS == 0){
    *(p->pad) = 0x10B0;
  } else {
    *(p->pad) = 0x1f038;
  }
  
  //x = xio_pin_to_info_PGM + pin;
  //*(x->reg) = xbara1_mux[pin];
  *hardware[_pin_idx].reg = hardware[_pin_idx].xbarMUX;
#ifdef DEBUG_OUTPUT
  if(PHASE == 1) Serial.printf("xbarIO-> %d, PhaseA-> %d\n",hardware[_pin_idx].xbarIO, channel[_encoder_ch].phaseA);
  if(PHASE == 2)Serial.printf("xbarIO-> %d, PhaseB-> %d\n",hardware[_pin_idx].xbarIO, channel[_encoder_ch].phaseB);
  if(PHASE == 3)Serial.printf("xbarIO-> %d, INDEX-> %d\n",hardware[_pin_idx].xbarIO, channel[_encoder_ch].index);
  if(PHASE == 4)Serial.printf("xbarIO-> %d, HOME-> %d\n",hardware[_pin_idx].xbarIO, channel[_encoder_ch].home);
#endif

  //XBARA1 Connection to encoder
  if(PHASE == 1) xbar_connect(hardware[_pin_idx].xbarIO, channel[_encoder_ch].phaseA);
  if(PHASE == 2) xbar_connect(hardware[_pin_idx].xbarIO, channel[_encoder_ch].phaseB);
  if(PHASE == 3) xbar_connect(hardware[_pin_idx].xbarIO, channel[_encoder_ch].index);
  if(PHASE == 4) xbar_connect(hardware[_pin_idx].xbarIO, channel[_encoder_ch].home);
  if(PHASE == 5) xbar_connect(hardware[_pin_idx].xbarIO, channel[_encoder_ch].trigger);
}

void QuadEncoder::xbar_connect(unsigned int input, unsigned int output)
{
  if (input >= 88) return;
  if (output >= 132) return;
#if 1
  volatile uint16_t *xbar = &XBARA1_SEL0 + (output / 2);
  uint16_t val = *xbar;
  if (!(output & 1)) {
    val = (val & 0xFF00) | input;
  } else {
    val = (val & 0x00FF) | (input << 8);
  }
  *xbar = val;
#else
  // does not work, seems 8 bit access is not allowed
  volatile uint8_t *xbar = (volatile uint8_t *)XBARA1_SEL0;
  xbar[output] = input;
#endif
}

void QuadEncoder::enableInterrupts(const enc_config_t *config)
{
    uint32_t tmp16 = 0;
	
	//enable interrupt
	NVIC_SET_PRIORITY(channel[_encoder_ch].interrupt, 32);
	NVIC_ENABLE_IRQ(channel[_encoder_ch].interrupt);
	attachInterruptVector(channel[_encoder_ch].interrupt, channel[_encoder_ch].isr);
	
    /* ENC_CTRL. */
    if (config->HomeTrigger != DISABLE)
    {
        tmp16 |= ENC_CTRL_HIE_MASK;
    }
    if (config->IndexTrigger != DISABLE)
    {
        tmp16 |= ENC_CTRL_XIE_MASK;
    }

    if (config->positionMatchMode == ENABLE)
    {
        tmp16 |= ENC_CTRL_CMPIE_MASK;
    }
    if (tmp16 != 0)
    {
        channel[_encoder_ch].ENC->CTRL = (channel[_encoder_ch].ENC->CTRL & (uint16_t)(~ENC_CTRL_W1C_FLAGS)) | tmp16;
    }
	/* ENC_CTRL2. */
    tmp16 = 0U;
    if (config->positionROIE != DISABLE)
    {
        tmp16 |= ENC_CTRL2_ROIRQ_MASK;
    }
    if (config->positionRUIE != DISABLE)
    {
        tmp16 |= ENC_CTRL2_RUIRQ_MASK;
    }
    if (tmp16 != 0U)
    {
        channel[_encoder_ch].ENC->CTRL2 = (uint16_t)(channel[_encoder_ch].ENC->CTRL2 & (uint16_t)(~ENC_CTRL2_W1C_FLAGS)) & (uint16_t)(~tmp16);
    }
	
}


void QuadEncoder::disableInterrupts(uint32_t flag)
{
    uint16_t tmp16 = 0U;

    /* ENC_CTRL. */
    if (_HOMETransitionEnable == (_HOMETransitionEnable & flag))
    {
        tmp16 |= ENC_CTRL_HIE_MASK;
    }
    if (_INDEXPulseEnable == (_INDEXPulseEnable & flag))
    {
        tmp16 |= ENC_CTRL_XIE_MASK;
    }
    if (_positionCompareEnable == (_positionCompareEnable & flag))
    {
        tmp16 |= ENC_CTRL_CMPIE_MASK;
    }
    if (0U != tmp16)
    {
        channel[_encoder_ch].ENC->CTRL = (uint16_t)(channel[_encoder_ch].ENC->CTRL & (uint16_t)(~ENC_CTRL_W1C_FLAGS)) & (uint16_t)(~tmp16);
    }
    /* ENC_CTRL2. */
    tmp16 = 0U;
    if (_positionRollOverFlag == (_positionRollOverFlag & flag))
    {
        tmp16 |= ENC_CTRL2_ROIRQ_MASK;
    }
    if (_positionRollUnderFlag == (_positionRollUnderFlag & flag))
    {
        tmp16 |= ENC_CTRL2_RUIRQ_MASK;
    }
    if (tmp16 != 0U)
    {
        channel[_encoder_ch].ENC->CTRL2 = (uint16_t)(channel[_encoder_ch].ENC->CTRL2 & (uint16_t)(~ENC_CTRL2_W1C_FLAGS)) & (uint16_t)(~tmp16);
    }
}


void QuadEncoder::clearStatusFlags(uint32_t flag, uint8_t index)
{
    uint32_t tmp16 = 0U;

    /* ENC_CTRL. */
    if (_HOMETransitionFlag == (_HOMETransitionFlag & flag))
    {
        tmp16 |= ENC_CTRL_HIRQ_MASK;
    }
    if (_INDEXPulseFlag == (_INDEXPulseFlag & flag))
    {
        tmp16 |= ENC_CTRL_XIRQ_MASK;
    }
    if (_positionCompareFlag == (_positionCompareFlag & flag))
    {
        tmp16 |= ENC_CTRL_CMPIRQ_MASK;
    }
    if (0U != tmp16)
    {
        channel[index].ENC->CTRL = (channel[index].ENC->CTRL & (uint16_t)(~ENC_CTRL_W1C_FLAGS)) | tmp16;
    }
    /* ENC_CTRL2. */
    tmp16 = 0U;
    if (_positionRollOverFlag == (_positionRollOverFlag & flag))
    {
        tmp16 |= ENC_CTRL2_ROIRQ_MASK;
    }
    if (_positionRollUnderFlag == (_positionRollUnderFlag & flag))
    {
        tmp16 |= ENC_CTRL2_RUIRQ_MASK;
    }
    if (0U != tmp16)
    {
        channel[index].ENC->CTRL2 = (channel[index].ENC->CTRL2 & (uint16_t)(~ENC_CTRL2_W1C_FLAGS)) | tmp16;
    }
}

inline void QuadEncoder::checkAndProcessInterrupt(uint8_t index) 
{
 	list[index]->isr(index);
}


void QuadEncoder::isrEnc1()
{
	checkAndProcessInterrupt(1);
	asm volatile ("dsb");  // wait for clear  memory barrier
}

void QuadEncoder::isrEnc2()
{
	checkAndProcessInterrupt(2);
	asm volatile ("dsb");  // wait for clear  memory barrier

}

void QuadEncoder::isrEnc3()
{
	checkAndProcessInterrupt(3);
	asm volatile ("dsb");  // wait for clear  memory barrier
}
void QuadEncoder::isrEnc4()
{
	checkAndProcessInterrupt(4);
	asm volatile ("dsb");  // wait for clear  memory barrier
}

void QuadEncoder::isr(uint8_t index) 
{	
    if (ENC_CTRL_XIRQ_MASK == (ENC_CTRL_XIRQ_MASK & channel[index].ENC->CTRL) && (ENC_CTRL_XIE_MASK & channel[index].ENC->CTRL))
    {
		indexCounter = indexCounter + 1;
		clearStatusFlags(_INDEXPulseFlag, index);
		
		if(ENC_CTRL2_ROIRQ_MASK == (ENC_CTRL2_ROIRQ_MASK & channel[index].ENC->CTRL2))
		{
			//Serial.println("ROLL OVER INDICATED!!!!!");
			//Serial.println("=========================");
			//Serial.println();
			clearStatusFlags(_positionRollOverFlag, index);
		}
		if(ENC_CTRL2_RUIRQ_MASK == (ENC_CTRL2_RUIRQ_MASK & channel[index].ENC->CTRL2))
		{
			//Serial.println("ROLL UNDER INDICATED!!!!!");
			//Serial.println("=========================");
			//Serial.println();
			clearStatusFlags(_positionRollUnderFlag, index);
		}

	}
	
    if (ENC_CTRL_HIRQ_MASK == (ENC_CTRL_HIRQ_MASK & channel[index].ENC->CTRL) && (ENC_CTRL_HIE_MASK & channel[index].ENC->CTRL))
    {

		homeCounter++;
		clearStatusFlags(_HOMETransitionFlag, index);
    }
	
	if (ENC_CTRL_CMPIRQ_MASK == (ENC_CTRL_CMPIRQ_MASK & channel[index].ENC->CTRL))
    {
		compareValueFlag = 1;
		clearStatusFlags(_positionCompareFlag, index);
	}
}
