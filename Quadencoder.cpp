#include "Arduino.h"
#include "QuadEncoder.h"

//#define DEBUG_OUTPUT

const QuadEncoder::ENC_Channel_t QuadEncoder::channel[] = {	
	{0,&IMXRT_ENC1, IRQ_ENC1, 66, 67, 68, 69, 70,&CCM_CCGR4,CCM_CCGR4_ENC1(CCM_CCGR_ON)},  //this is a dummy entry - use 1-4 for channels
	{1, &IMXRT_ENC1, IRQ_ENC1, 66, 67, 68, 69, 70,&CCM_CCGR4,CCM_CCGR4_ENC1(CCM_CCGR_ON)},
	{2, &IMXRT_ENC2, IRQ_ENC2, 71, 72, 73, 74, 75,&CCM_CCGR4,CCM_CCGR4_ENC2(CCM_CCGR_ON)},
	{3, &IMXRT_ENC3, IRQ_ENC3, 76, 77, 78, 79, 80,&CCM_CCGR4,CCM_CCGR4_ENC3(CCM_CCGR_ON)},
	{4, &IMXRT_ENC4, IRQ_ENC4, 81, 82, 83, 84, 95,&CCM_CCGR4,CCM_CCGR4_ENC4(CCM_CCGR_ON)}
};
const uint8_t QuadEncoder::_channel_count =  (sizeof(QuadEncoder::channel)/sizeof(QuadEncoder::channel[0]));

//xbara1 pin config
//pin, idx, *reg, alt
const  QuadEncoder::ENC_Hardware_t QuadEncoder::hardware[] = {	
	{0, 0, &CORE_XIO_PIN0, 1, 17},	{1, 1, &CORE_XIO_PIN1, 1, 16},
	{2, 2, &CORE_XIO_PIN2, 3, 6},	{3, 3, &CORE_XIO_PIN3, 3, 7},
	{4, 4, &CORE_XIO_PIN4,3, 8},		{5, 5, &CORE_XIO_PIN5, 3, 17},
	{7, 6, &CORE_XIO_PIN7, 1, 15},	{8, 7, &CORE_XIO_PIN8, 1, 14}
};
const uint8_t QuadEncoder::_hardware_count =  (sizeof(QuadEncoder::hardware)/sizeof(QuadEncoder::hardware[0]));

void QuadEncoder::setInitConfig() {
  getConfig1(&EncConfig);
}

void QuadEncoder::init()
{
  Init(&EncConfig);
  setInitialPosition();
}


QuadEncoder::QuadEncoder(uint8_t encoder_ch, uint8_t PhaseA_pin, uint8_t PhaseB_pin, uint8_t pin_pus){
	_encoder_ch = encoder_ch;
	
  CCM_CCGR2 |= CCM_CCGR2_XBAR1(CCM_CCGR_ON);   //turn clock on for xbara1
  
#ifdef DEBUG_OUTPUT
  Serial.printf("begin: encoder channel-> %d\n",_encoder_ch);
  Serial.printf("begin: pinA-> %d, pinB-> %d\n", PhaseA_pin, PhaseB_pin);
#endif
  enc_xbara_mapping(PhaseA_pin, PHASEA, pin_pus);
  enc_xbara_mapping(PhaseB_pin, PHASEB, pin_pus);
}

void QuadEncoder::getConfig1(enc_config_t *config)
{
    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableReverseDirection = false;
    config->decoderWorkMode = false;
    config->HOMETriggerMode = DISABLE;
    config->INDEXTriggerMode = DISABLE;
    config->clearCounter = false;
    config->clearHoldCounter = false;
    config->filterCount = 0U;
    config->filterSamplePeriod = 0U;
    config->positionMatchMode = false;
    config->positionCompareValue = 0xFFFFFFFFU;
    config->revolutionCountCondition = false;
    config->enableModuloCountMode = false;
    config->positionModulusValue = 0U;
    config->positionInitialValue = 0U;
}

void QuadEncoder::setInitialPosition()
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
    if (DISABLE != config->HOMETriggerMode)
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
    tmp16 = channel[_encoder_ch].ENC->CTRL2 & (uint16_t)(~(ENC_CTRL2_W1C_FLAGS | ENC_CTRL2_OUTCTL_MASK | ENC_CTRL2_REVMOD_MASK | ENC_CTRL2_MOD_MASK | ENC_CTRL2_UPDPOS_MASK | ENC_CTRL2_UPDHLD_MASK));
	
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

uint32_t QuadEncoder::getPosition()
{
    uint32_t ret32;

    ret32 = channel[_encoder_ch].ENC->UPOS; /* Get upper 16 bits and make a snapshot. */
    ret32 <<= 16U;
    ret32 |= channel[_encoder_ch].ENC->LPOSH; /* Get lower 16 bits from hold register. */

    return ret32;
}

void QuadEncoder::setPosition(uint32_t value)
{
    channel[_encoder_ch].ENC->UINIT = (uint16_t)(value >> 16U); /* Set upper 16 bits. */
    channel[_encoder_ch].ENC->LINIT = (uint16_t)(value);        /* Set lower 16 bits. */
}

uint32_t QuadEncoder::ENC_getHoldPosition()
{
    uint32_t ret32;

    ret32 = channel[_encoder_ch].ENC->UPOSH; /* Get upper 16 bits and make a snapshot. */
    ret32 <<= 16U;
    ret32 |= channel[_encoder_ch].ENC->LPOSH; /* Get lower 16 bits from hold register. */

    return ret32;
}

/*!
 * @brief  Get the position difference counter's value.
 *
 * @param  base ENC peripheral base address.
 *
 * @return     The position difference counter's value.
 */
uint16_t QuadEncoder::getPositionDifference()
{
    return channel[_encoder_ch].ENC->POSD;
}

/*!
 * @brief  Get the hold position difference counter's value.
 *
 * When any of the counter registers is read, the contents of each counter register is written to the corresponding hold
 * register. Taking a snapshot of the counters' values provides a consistent view of a system position and a velocity to
 * be attained.
 *
 * @param  base ENC peripheral base address.
 *
 * @return      Hold position difference counter's value.
 */
uint16_t QuadEncoder::getHoldDifference()
{
    return channel[_encoder_ch].ENC->POSDH;
}

/*!
 * @brief  Get the position revolution counter's value.
 *
 * @param  base ENC peripheral base address.
 *
 * @return     The position revolution counter's value.
 */
uint16_t QuadEncoder::getRevolution()
{
    return channel[_encoder_ch].ENC->REV;
}
/*!
 * @brief  Get the hold position revolution counter's value.
 *
 * When any of the counter registers is read, the contents of each counter register is written to the corresponding hold
 * register. Taking a snapshot of the counters' values provides a consistent view of a system position and a velocity to
 * be attained.
 *
 * @param  base ENC peripheral base address.
 *
 * @return      Hold position revolution counter's value.
 */
uint16_t QuadEncoder::getHoldRevolution()
{
    return channel[_encoder_ch].ENC->REVH;
}

void QuadEncoder::enc_xbara_mapping(uint8_t pin, uint8_t PHASE, uint8_t PUS){ 

  const struct digital_pin_bitband_and_config_table_struct *p;
  uint32_t pinmode, mask;

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
  
  uint8_t xbara1_mux[] = {1, 0, 0, 0, 0, 0, 1, 1};
  
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
  *hardware[_pin_idx].reg = xbara1_mux[_pin_idx];
#ifdef DEBUG_OUTPUT
  if(PHASE == 1) Serial.printf("xbarIO-> %d, PhaseA-> %d\n",hardware[_pin_idx].xbarIO, channel[_encoder_ch].phaseA);
  if(PHASE == 2)Serial.printf("xbarIO-> %d, PhaseB-> %d\n",hardware[_pin_idx].xbarIO, channel[_encoder_ch].phaseB);
#endif

  //XBARA1 Connection to encoder
  if(PHASE == 1) xbar_connect(hardware[_pin_idx].xbarIO, channel[_encoder_ch].phaseA);
  if(PHASE == 2) xbar_connect(hardware[_pin_idx].xbarIO, channel[_encoder_ch].phaseB);

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
