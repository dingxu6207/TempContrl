#include "stm8l15x.h"//STM8L051/151���ÿ⺯��
#include <stdlib.h>

//����LED�������˿�
#define LED_PORT  GPIOD
#define LED_PINS  GPIO_Pin_0
#define KEY_PORT  GPIOB
#define KEY_PINS  GPIO_Pin_1

uint8_t HexTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
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
/*******************************************************************************
****����˵����������
****��ڲ���:��
****���ڲ���:��
****������ע:PB0(adc1-18)��ΪADC����ڣ�����ͨ���Ű��߽�3.3V��GND�����˿ڣ�ADCת�����ͨ��
             USART���
********************************************************************************/

void main(void)
{
  u16 u16_adc1_value; 
  u16 u16_adc2_value;
  
  GPIO_Init(LED_PORT,LED_PINS,GPIO_Mode_Out_PP_Low_Slow);//��ʼ��LED�˿�
  GPIO_Init(KEY_PORT,KEY_PINS,GPIO_Mode_In_PU_No_IT);//��ʼ��KEY�˿ڣ����������룬�����ж�
  
  //SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);//�˿���ӳ�䣬ȥ��ע��֮��USART1ΪPA2-TX��PA3-RX��ע��֮��USART1ΪTX-PC5��RX-PC6����λ֮��USART���Զ��ָ���PC5��PC6
  
  CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);//����USARTʱ�� 
  USART_Init(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx|USART_Mode_Rx);//����USART����9600��8N1������/����
  USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
  USART_Cmd (USART1,ENABLE);//ʹ��USART
  
  CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,ENABLE);//����ADCʱ��
  ADC_Init (ADC1,ADC_ConversionMode_Single,ADC_Resolution_12Bit,ADC_Prescaler_1);//ADC1�����β�����12λ��1��Ƶ
  ADC_Cmd(ADC1,ENABLE);//ADC1ʹ��
  //ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);//ADC1 18ͨ��ʹ��
  while (1)
  {
       #if 1
       ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);//ADC1 18ͨ��ʹ��
       USART1_SendStr("ADC1ת�����Ϊ: ");
       ChangeAD();
       u16_adc1_value=ADC_GetConversionValue (ADC1);//��ȡת��ֵ
       ADC_ChannelCmd (ADC1,ADC_Channel_18,DISABLE);//ADC1 18ͨ��ʹ��

       Delay(100000);

       DisplayData(u16_adc1_value);
       
        
       ADC_ChannelCmd (ADC1,ADC_Channel_17,ENABLE);//ADC1 17ͨ��ʹ��
       USART1_SendStr("ADC2ת�����Ϊ: ");
       ChangeAD();
       u16_adc2_value=ADC_GetConversionValue (ADC1);//��ȡת��ֵ
       ADC_ChannelCmd (ADC1,ADC_Channel_17,DISABLE);//ADC1 17ͨ��ʹ��

       Delay(100000);

       DisplayData(u16_adc2_value);
       
  
       #endif
    
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
