#include "stm8l15x.h"//STM8L051/151���ÿ⺯��
#include <stdlib.h>

//����LED�������˿�
#define LED_PORT  GPIOD
#define LED_PINS  GPIO_Pin_0
#define KEY_PORT  GPIOB
#define KEY_PINS  GPIO_Pin_1


//�������ʾ
uint8_t HexTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

#define uchar  unsigned char  
 unsigned char  fseg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
 unsigned char  segbit[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 unsigned char  disbuf[4]={0,0,0,0};
 
void LED4_Display (void);			// LED��ʾ
void LED_OUT(uchar X);				// LED���ֽڴ�����λ����

unsigned char LED_0F[];		// LED��ģ��
 
unsigned char  LED_0F[] = 
{// 0	 1	  2	   3	4	 5	  6	   7	8	 9	  A	   b	C    d	  E    F    -
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf
};

uchar LED[8];	//����LED��8λ��ʾ����

#define DIO_PORT  GPIOB
#define DIO_PINS  GPIO_Pin_4

#define RCLK_PORT  GPIOB
#define RCLK_PINS  GPIO_Pin_5

#define SCLK_PORT  GPIOB
#define SCLK_PINS  GPIO_Pin_6

#define DIOHigh  GPIO_SetBits(DIO_PORT, DIO_PINS)
#define DIOLow   GPIO_ResetBits(DIO_PORT, DIO_PINS)

#define RCLKHigh  GPIO_SetBits(RCLK_PORT, RCLK_PINS)
#define RCLKLow   GPIO_ResetBits(RCLK_PORT, RCLK_PINS)

#define SCLKHigh  GPIO_SetBits(SCLK_PORT, SCLK_PINS)
#define SCLKLow   GPIO_ResetBits(SCLK_PORT, SCLK_PINS)

/*******************************************************************************
****��ڲ�������
****���ڲ�������
****������ע������ȷ��ʱ����
****��Ȩ��Ϣ������Ƕ��ʽϵͳ
*******************************************************************************/
void Delay(__IO uint16_t nCount)
{
    /* Decrement nCount value */
    while (nCount != 0)
    {
        nCount--;
    }
}
/*******************************************************************************
****��ڲ�������Ҫ���͵��ַ���
****���ڲ�������
****������ע��USART���ͺ���
****��Ȩ��Ϣ������Ƕ��ʽϵͳ
*******************************************************************************/
void USART1_SendStr(unsigned char *Str) 
{
        while(*Str!=0)//��Ϊ����
        {
            USART_SendData8(USART1,*Str);     //�������� 
            while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//�ȴ��������
            Str++;//��һ������
        }
}
/*******************************************************************************
****��ڲ�������Ҫ���͵�16������
****���ڲ�������
****������ע��USART����16���ƺ���
****��Ȩ��Ϣ������Ƕ��ʽϵͳ
*******************************************************************************/
void USART1_SendHex(unsigned char dat)
{
      USART_SendData8(USART1,'0');
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//�ȴ��������
      USART_SendData8(USART1,'x');
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//�ȴ��������
      USART_SendData8(USART1,HexTable[dat>>4]);
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//�ȴ��������
      USART_SendData8(USART1,HexTable[dat&0x0f]);
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//�ȴ��������
      USART_SendData8(USART1,' ');
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//�ȴ��������
}

//----------------------------------------------------------
//adcת��
void  ChangeAD()
{

   ADC_SoftwareStartConv (ADC1);//�������ת��
              
   while(!ADC_GetFlagStatus (ADC1,ADC_FLAG_EOC));//�ȴ�ת������
       ADC_ClearFlag (ADC1,ADC_FLAG_EOC);//�����ر�ʶ
	
}

//------------------------------------------------------------
//��ӡ16����
void DisplayData(u16  data)
{

	 USART1_SendHex((data>>8));  
     USART1_SendHex((data&0xff));
     USART1_SendStr("\r\n");

}

//---------------------------------------------------------------------------
//

void LED4_Display (void)
{
	unsigned char *led_table;          // ���ָ��
	uchar i;
	//��ʾ��1λ
	led_table = LED_0F + LED[3];
	i = *led_table;

	LED_OUT(i);	
	LED_OUT(0x01);		

	RCLKLow;
	RCLKHigh;
	//��ʾ��2λ
	led_table = LED_0F + LED[2];
	i = *led_table;

	LED_OUT(i);		
	LED_OUT(0x02);		

	RCLKLow;
	RCLKHigh;
        
        
#if 1
	//��ʾ��3λ
	led_table = LED_0F + LED[1];
	i = *led_table;

	LED_OUT(i);			
	LED_OUT(0x04);	

	RCLKLow;
	RCLKHigh;
        
        
	//��ʾ��4λ
	led_table = LED_0F + LED[0];
	i = *led_table;

	LED_OUT(i);			
	LED_OUT(0x08);		

	RCLKLow;
	RCLKHigh;
#endif
}


//------------------------------------------
//

void LED_OUT(uchar X)
{
	uchar i;
	for(i=8;i>=1;i--)
	{
		if (X&0x80) DIOHigh; else DIOLow;
		X<<=1;
		SCLKHigh;
		SCLKLow;
	}
}

//��ʱ��2���ã�1�������һ���жϣ�����ϵͳ��ʱ

void TIM2_Init(void)   

{ 
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);//����ʱ���ź��͸���ʱ��4(Lϵ�е�Ƭ������)
  TIM2_TimeBaseInit(TIM2_Prescaler_16,TIM2_CounterMode_Up,0x7D);            //��ʱ��4,����ʱ��16��Ƶ.�Զ�װ��FA=125
  TIM2_SetCounter(0xFA);                                //�趨����ֵFA=125
  TIM2_ITConfig(TIM2_IT_Update,ENABLE);
  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_Cmd(ENABLE);                                    //������ʹ�ܣ���ʼ����   
}
/*******************************************************************************
****����˵����������
****��ڲ���:��
****���ڲ���:��
****������ע:PB0(adc1-18)��ΪADC����ڣ�����ͨ���Ű��߽�3.3V��GND�����˿ڣ�ADCת�����ͨ��
             USART���
********************************************************************************/
extern u16 temp;
void main(void)
{
  u16 u16_adc1_value; 
  u16 u16_adc2_value;
  
  GPIO_Init(LED_PORT,LED_PINS,GPIO_Mode_Out_PP_Low_Slow);//��ʼ��LED�˿�
  GPIO_Init(KEY_PORT,KEY_PINS,GPIO_Mode_In_PU_No_IT);//��ʼ��KEY�˿ڣ����������룬�����ж�
  
  GPIO_Init(DIO_PORT,DIO_PINS,GPIO_Mode_Out_PP_High_Fast);//��ʼ��DIO�˿�
  GPIO_Init(RCLK_PORT,RCLK_PINS,GPIO_Mode_Out_PP_High_Fast);//��ʼ��RCLK�˿�
  GPIO_Init(SCLK_PORT,SCLK_PINS,GPIO_Mode_Out_PP_High_Fast);//��ʼ��RCLK�˿�
  
  
  //SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);//�˿���ӳ�䣬ȥ��ע��֮��USART1ΪPA2-TX��PA3-RX��ע��֮��USART1ΪTX-PC5��RX-PC6����λ֮��USART���Զ��ָ���PC5��PC6
  
  CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);//����USARTʱ�� 
  USART_Init(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx|USART_Mode_Rx);//����USART����9600��8N1������/����
  USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
  USART_Cmd (USART1,ENABLE);//ʹ��USART
  
  CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,ENABLE);//����ADCʱ��
  ADC_Init (ADC1,ADC_ConversionMode_Single,ADC_Resolution_12Bit,ADC_Prescaler_1);//ADC1�����β�����12λ��1��Ƶ
  ADC_Cmd(ADC1,ENABLE);//ADC1ʹ��
  //ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);//ADC1 18ͨ��ʹ��
  
   LED[0]=7;
   LED[1]=2;
   LED[2]=3;
   LED[3]=4;
   
   TIM2_Init();
   
   asm("rim"); 
   
  while (1)
  {
       #if 1
       ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);//ADC1 18ͨ��ʹ��
       USART1_SendStr("ADC1ת�����Ϊ: ");
       ChangeAD();
       u16_adc1_value=ADC_GetConversionValue (ADC1);//��ȡת��ֵ
       ADC_ChannelCmd (ADC1,ADC_Channel_18,DISABLE);//ADC1 18ͨ��ʹ��

      // Delay(0xFFFF);
       u16_adc1_value = (u16_adc1_value*33000)>>12;
      
       DisplayData(u16_adc1_value);
       
        
       ADC_ChannelCmd (ADC1,ADC_Channel_4,ENABLE);//ADC1 17ͨ��ʹ��
       USART1_SendStr("ADC2ת�����Ϊ: ");
       ChangeAD();
       u16_adc2_value=ADC_GetConversionValue (ADC1);//��ȡת��ֵ
       ADC_ChannelCmd (ADC1,ADC_Channel_4,DISABLE);//ADC1 17ͨ��ʹ��

      // Delay(0xFFFF);
       u16_adc2_value = (u16_adc2_value*33000)>>12;

       DisplayData(u16_adc2_value);
        
       #endif

       if(GPIO_ReadInputDataBit(KEY_PORT,KEY_PINS)==0)//��GPB1����״̬
       {
             Delay(0x3FFF);  //�������
             if(GPIO_ReadInputDataBit(KEY_PORT,KEY_PINS)==0)  //��GPB1����״̬
                GPIO_ToggleBits(LED_PORT, LED_PINS);//��תLED���״̬
       }
     
        //if (temp >= 0)
       // {
        //   LED4_Display ();
        
       // }
       
      
  }
}





#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
