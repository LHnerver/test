
#include "adc.h"

/* Note:  MSCR.SSS configuration  is not needed for inputs if there is  */
/*        no SSS value is in signal spreadsheet */
/* Note:  ADC1 channel 2 on port PD2 is connected to pot on AID_GR_CB_V2.0 */
void ADC1_PadConfig_ChanSelect(void)
{
	/* Enable channel  CHn for normal conversion on ADC1 */
	ADC_1.NCMR0.B.CH0  = 1;		// �����Ǳ�
	ADC_1.NCMR0.B.CH1  = 1;		// ������ѹ
	ADC_1.NCMR0.B.CH2  = 1;		// ��������
	ADC_1.NCMR0.B.CH3  = 1 ;    // ����ͨ��3 ģ��ɼ�ADC
}

/* Calibrate to compensate for variations. Steps below are from reference manual. */
void ADC1_Calibration(void)
{
	/* Calibration has not passed yet */
	uint32_t __attribute__ ((unused)) ADC1_Calibration_Failed = 1;

	while (ADC1_Calibration_Failed)
	{
		/* Power down for starting calibration process */
		ADC_1.MCR.B.PWDN = 1;
		/* ADC clock = bus clock/2 (80/2 MHz FS80) */
		/* Note: Calibration requires it to be set at max 40MHz for best results */
		ADC_1.MCR.B.ADCLKSEL = 0;

		/* Enable calibration test */
		ADC_1.CALBISTREG.B.TEST_EN = 1;
		/* Power back up for calibration test to start */
		ADC_1.MCR.B.PWDN = 0;
		/* Wait for calibration to finish */
		while(ADC_1.CALBISTREG.B.C_T_BUSY);
		/* If calibration ran successfully */
		if(ADC_1.MSR.B.CALIBRTD)
		{
			/* Calibration was successful */
			ADC1_Calibration_Failed = 0;
		}
		else
		{
			/* Calibration was not successful */
			ADC1_Calibration_Failed = 1;
		}
	}
}

/* Initialize ADC1 module & start normal scan mode */
void ADC1_Init(void)
{
	/* Power down for starting module initialization */
	ADC_1.MCR.B.PWDN     = 1;
	/* Enable overwriting older conversion results */
	ADC_1.MCR.B.OWREN    = 1;
	/* Scan mode (1) used instead of one shot mode */
	ADC_1.MCR.B.MODE     = 1;
	/* ADC clock = FS80 bus clock (80 MHz here) */
	ADC_1.MCR.B.ADCLKSEL = 1;
	/* ADC_1 ready to receive conversion triggers */
	ADC_1.MCR.B.PWDN     = 0;
	/* Initiate trigger for normal scan */
	ADC_1.MCR.B.NSTART   = 1;
}

//ADC_1_CH0 ����	�����Ǳ�
uint16_t ADC1_Read_CH0 (void)
{
	return ADC_1.CDR[0].B.CDATA;	/* Read channel 0 conversion result data */
}

//ADC_1_CH1 ����	������ѹ
uint16_t ADC1_Read_CH1 (void)
{
	return ADC_1.CDR[1].B.CDATA;	/* Read channel 1 conversion result data */
}

//ADC_1_CH2 ����	��������
uint16_t ADC1_Read_CH2 (void)
{
	return ADC_1.CDR[2].B.CDATA;	/* Read channel 2 conversion result data */
}

//����ADC_1_CH3 �ɼ�ѹ��������������
uint16_t ADC1_Read_CH3 (void)
{
	return ADC_1.CDR[3].B.CDATA;	/* Read channel 2 conversion result data */
}


void Init_ADC(void)
{
	/* Configures ADC pads & select scan channel */
	ADC1_PadConfig_ChanSelect();
	ADC1_Calibration();
	ADC1_Init();
}


// ����ADC1_CH3  ��ȡѹ���������ĵ�ѹֵ�� ѹ������������ֵ��ΧΪ 0-3.3V
// ����оƬLM2904��Ϊ12λ���ΧΪ0-4095
uint16_t get_pressure_voltage_value(void)
{
	uint16_t value;
	/* If normal scan channels finished converting */
	if (ADC_1.ISR.B.ECH)
	{
		/* Read conversion Results */
		value = ADC1_Read_CH3();
		/* Clear End of CHain (ECH) status bit */
		ADC_1.ISR.R = 0x00000001;
	}

	return value;
}

void  get_pressure(uint16_t  *pressure)
{
	  uint16_t   pressure_voltage_tmp = 0 ;
	  uint16_t   voltage =  0 ;
	  pressure_voltage_tmp = get_pressure_voltage_value();


	//  uart8_printf("pressure_voltage_tmp:%d\n",pressure_voltage_tmp);
	  *pressure  = (pressure_voltage_tmp*25*500)/4095;

    //uart8_printf("pressure :%d\n",*pressure);


}

