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
//interrupts
#define ENC_CTRL_HIE_MASK                        (0x4000U)
#define ENC_CTRL_XIE_MASK                        (0x80U)
#define ENC_CTRL_CMPIE_MASK                      (0x1U)

#define ENC_CTRL2_SABIRQ_MASK                    (0x800U)
#define ENC_CTRL2_ROIRQ_MASK                     (0x80U)
#define ENC_CTRL2_RUIRQ_MASK                     (0x20U)
#define ENC_CTRL2_OUTCTL_MASK                    (0x200U)
#define ENC_CTRL2_REVMOD_MASK                    (0x100U)
#define ENC_CTRL2_MOD_MASK                       (0x4U)
#define ENC_CTRL2_UPDHLD_MASK                    (0x1U)
#define ENC_CTRL2_UPDPOS_MASK                    (0x2U)
//interrupts
//#define ENC_CTRL2_ROIE_MASK                     (0x80U)
//#define ENC_CTRL2_RUIE_MASK                     (0x20U)


#define ENC_CTRL_W1C_FLAGS (ENC_CTRL_HIRQ_MASK | ENC_CTRL_XIRQ_MASK | ENC_CTRL_DIRQ_MASK | ENC_CTRL_CMPIRQ_MASK)
#define ENC_CTRL2_W1C_FLAGS (ENC_CTRL2_SABIRQ_MASK | ENC_CTRL2_ROIRQ_MASK | ENC_CTRL2_RUIRQ_MASK)

#define ENC_FILT_FILT_PER(x)		(((uint16_t)(((uint16_t)(x)) << 0U)) & 0xFFU)
#define ENC_FILT_FILT_CNT(x)		(((uint16_t)(((uint16_t)(x)) << 8U)) & 0x700U)

#define DISABLE				0
#define ENABLE				1
#define FALLING_EDGE		2
#define RISING_EDGE			1

#define PHASEA 1
#define PHASEB 2
#define INDEX 3
#define HOME 4
#define TRIGGER 5
#define PULLUPS 0  //set to 1 if pullups are needed


#define CORE_XIO_PIN0 IOMUXC_XBAR1_IN17_SELECT_INPUT	//ALT1
#define CORE_XIO_PIN1 IOMUXC_XBAR1_IN16_SELECT_INPUT	//ALT1
#define CORE_XIO_PIN2 IOMUXC_XBAR1_IN06_SELECT_INPUT	//ALT3
#define CORE_XIO_PIN3 IOMUXC_XBAR1_IN07_SELECT_INPUT	//ALT3
#define CORE_XIO_PIN4 IOMUXC_XBAR1_IN08_SELECT_INPUT	//ALT3
#define CORE_XIO_PIN5 IOMUXC_XBAR1_IN17_SELECT_INPUT	//ALT3
#define CORE_XIO_PIN7 IOMUXC_XBAR1_IN15_SELECT_INPUT	//ALT1
#define CORE_XIO_PIN8 IOMUXC_XBAR1_IN14_SELECT_INPUT   	//ALT1
#define CORE_XIO_PIN30 IOMUXC_XBAR1_IN23_SELECT_INPUT	//ALT1,0
#define CORE_XIO_PIN31 IOMUXC_XBAR1_IN22_SELECT_INPUT	//ALT1,0
#define CORE_XIO_PIN33 IOMUXC_XBAR1_IN09_SELECT_INPUT   //ALT3,0

#ifdef ARDUINO_TEENSY41
#define CORE_XIO_PIN36 IOMUXC_XBAR1_IN16_SELECT_INPUT
#define CORE_XIO_PIN37 IOMUXC_XBAR1_IN17_SELECT_INPUT
#endif


enum _flags
{
    _HOMETransitionFlag = (1 << 0),
    _INDEXPulseFlag = (1 << 1),
    _positionCompareFlag = (1 << 3),
    _positionRollOverFlag = (1 << 5),
    _positionRollUnderFlag = (1 << 6),
    _lastDirectionFlag = (1 << 7),
	
};

enum _interrupts
{
    _HOMETransitionEnable = (1 << 0),
    _INDEXPulseEnable = (1 << 1), 
    _positionCompareEnable = (1 << 3),
    _positionROEnable = (1 << 5),
    _positionRUEnable = (1 << 6),
};


class QuadEncoder
{
public:
	void isr(uint8_t index);

	typedef struct
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
		
		/*Position modulus value. This value would be available only when	"enableModuloCountMode" = true. The available value is a 32-bit number. */
		uint32_t positionModulusValue;  
		
		//Position initial value. The available value is a 32-bit number. */
		uint32_t positionInitialValue; 
		
		//Position Roll Over or Roll Under Interrupt Enable
		uint8_t positionROIE;
		uint8_t positionRUIE;
		
		bool IndexTrigger;
		bool HomeTrigger;
	} enc_config_t;
	
	//encoder
	typedef struct {
		uint8_t			enc_ch;		
		volatile IMXRT_ENC_t* 	ENC;
		IRQ_NUMBER_t	interrupt;
		void     		(*isr)();
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
		uint8_t			xbarMUX;
	} ENC_Hardware_t;
	
	enc_config_t EncConfig;
	static const ENC_Channel_t channel[];
	static const ENC_Hardware_t hardware[];
	static const uint8_t _channel_count;
	static const uint8_t _hardware_count;
	

public:
	QuadEncoder(uint8_t encoder_ch = 255, uint8_t PhaseA_pin = 255, uint8_t PhaseB_pin = 255, uint8_t pin_pus = 0, uint8_t index_pin = 255, uint8_t home_pin = 255, uint8_t trigger_pin = 255);
	void getConfig1(enc_config_t *config);
	void setConfigInitialPosition();
	void setInitConfig();
	void init();
	void Init(const enc_config_t *config);
	int32_t read();
	void write(uint32_t value);
	uint32_t getHoldPosition();
	uint16_t getPositionDifference();
	uint16_t getHoldDifference();
	uint16_t getRevolution();
	uint16_t getHoldRevolution();
	void printConfig(enc_config_t *config);
	
	//xbara1 configuration
	void enc_xbara_mapping(uint8_t pin, uint8_t PHASE, uint8_t PUS);
	void xbar_connect(unsigned int input, unsigned int output);
	
	//interrupts
	void enableInterrupts(const enc_config_t *config);
	void disableInterrupts(uint32_t flag);
	void clearStatusFlags(uint32_t flag, uint8_t index);
	
	// static class functions
	static void isrEnc1();
	static void isrEnc2();
	static void isrEnc3();
	static void isrEnc4();
	static inline void checkAndProcessInterrupt(uint8_t index);
	
	//counters
	static uint32_t homeCounter;
	static uint32_t indexCounter;
	static uint8_t compareValueFlag;
private:
	static QuadEncoder *list[5];
	volatile uint16_t _encoder_ch, _pin_idx;

};

#endif