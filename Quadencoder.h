#ifndef __QUADENCODER_H__
#define __QUADENCODER_H__
 
#ifndef __IMXRT1062__
#error "Sorry, Quad Encoder only works on Teensy 4.x boards"
#endif

#include <Arduino.h>

#define ENC_CTRL_HIRQ_MASK                       (0x8000U)
#define ENC_CTRL_XIRQ_MASK                       (0x100U)
#define ENC_CTRL_DIRQ_MASK                       (0x10U)
#define ENC_CTRL_CMPIRQ_MASK                     (0x2U)
#define ENC_CTRL_HIP_MASK                        (0x2000U)
#define ENC_CTRL_HNE_MASK                        (0x1000U)
#define ENC_CTRL_REV_MASK                        (0x400U)
#define ENC_CTRL_PH1_MASK                        (0x200U)
#define ENC_CTRL_XIP_MASK                        (0x40U)
#define ENC_CTRL_XNE_MASK                        (0x20U)
#define ENC_CTRL_WDE_MASK                        (0x4U)
#define ENC_CTRL_WDE_MASK                        (0x4U)
#define ENC_CTRL_SWIP_MASK                       (0x800U)

#define ENC_CTRL2_SABIRQ_MASK                    (0x800U)
#define ENC_CTRL2_ROIRQ_MASK                     (0x80U)
#define ENC_CTRL2_RUIRQ_MASK                     (0x20U)
#define ENC_CTRL2_OUTCTL_MASK                    (0x200U)
#define ENC_CTRL2_REVMOD_MASK                    (0x100U)
#define ENC_CTRL2_MOD_MASK                       (0x4U)
#define ENC_CTRL2_UPDHLD_MASK                    (0x1U)
#define ENC_CTRL2_UPDPOS_MASK                    (0x2U)

#define ENC_CTRL_W1C_FLAGS (ENC_CTRL_HIRQ_MASK | ENC_CTRL_XIRQ_MASK | ENC_CTRL_DIRQ_MASK | ENC_CTRL_CMPIRQ_MASK)
#define ENC_CTRL2_W1C_FLAGS (ENC_CTRL2_SABIRQ_MASK | ENC_CTRL2_ROIRQ_MASK | ENC_CTRL2_RUIRQ_MASK)

#define ENC_FILT_FILT_PER(x)		(((uint16_t)(((uint16_t)(x)) << 0U)) & 0xFFU)
#define ENC_FILT_FILT_CNT(x)		(((uint16_t)(((uint16_t)(x)) << 8U)) & 0x700U)

#define DISABLE				0
#define FALLING_EDGE		2
#define RISING_EDGE			1

#define PHASEA 1
#define PHASEB 2
#define PULLUPS 0  //set to 1 if pullups are needed

#define CORE_XIO_PIN0 IOMUXC_XBAR1_IN17_SELECT_INPUT	//ALT1
#define CORE_XIO_PIN1 IOMUXC_XBAR1_IN16_SELECT_INPUT	//ALT1
#define CORE_XIO_PIN2 IOMUXC_XBAR1_IN06_SELECT_INPUT	//ALT3
#define CORE_XIO_PIN3 IOMUXC_XBAR1_IN07_SELECT_INPUT	//ALT3
#define CORE_XIO_PIN4 IOMUXC_XBAR1_IN08_SELECT_INPUT	//ALT3
#define CORE_XIO_PIN5 IOMUXC_XBAR1_IN17_SELECT_INPUT	//ALT3
#define CORE_XIO_PIN7 IOMUXC_XBAR1_IN15_SELECT_INPUT	//ALT1
#define CORE_XIO_PIN8 IOMUXC_XBAR1_IN14_SELECT_INPUT   	//ALT1

	
class QuadEncoder
{
public:
	typedef struct _enc_config
	{
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
	} enc_config_t;
	
	//encoder
	typedef struct {
		uint8_t			enc_ch;		
		volatile IMXRT_ENC_t* 	ENC;
		IRQ_NUMBER_t	interrupt;
		uint16_t		phaseA;
		uint16_t		phaseB;
		uint16_t		index;
		uint16_t		home;
		uint16_t		trigger;
		volatile uint32_t *clock_gate_register;
		uint32_t 		clock_gate_mask;
	} ENC_Channel_t;
	
	//Encoder channel structure
	typedef struct {
		uint8_t 		idx;
		uint8_t			pin;
		volatile uint32_t *reg; // Which register controls the selection
		const uint32_t		select_val;	// Value for that selection
		uint16_t		xbarIO;
	} ENC_Hardware_t;
	
	enc_config_t mEncConfigStruct;
	static const ENC_Channel_t channel[];
	static const ENC_Hardware_t hardware[];
	static const uint8_t _channel_count;
	static const uint8_t _hardware_count;
public:
	QuadEncoder(uint8_t encoder_ch = 1);
	void getConfig(enc_config_t *config);
	void setInitialPosition();
	void begin(uint8_t PhaseA_pin, uint8_t PhaseB_pin, uint8_t pin_pus);
	void Init(const enc_config_t *config);
	uint32_t getPosition();
	void setPosition(uint32_t value);
	uint32_t ENC_getHoldPosition();
	uint16_t getPositionDifference();
	uint16_t getHoldDifference();
	uint16_t getRevolution();
	uint16_t getHoldRevolution();
	void enc_xbara_mapping(uint8_t pin, uint8_t PHASE, uint8_t PUS);
	void xbar_connect(unsigned int input, unsigned int output);
	
private:
	uint16_t _encoder_ch, _pin_idx;


};

#endif