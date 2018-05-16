#include "stm8l15x.h"//STM8L051/151公用库函数
#include <stdlib.h>

//定义LED及按键端口
#define LED_PORT  GPIOD
#define LED_PINS  GPIO_Pin_0
#define KEY_PORT  GPIOB
#define KEY_PINS  GPIO_Pin_1


//数码管显示
uint8_t HexTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

#define uchar  unsigned char  
 unsigned char  fseg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
 unsigned char  segbit[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 unsigned char  disbuf[4]={0,0,0,0};
 
void LED4_Display (void);			// LED显示
void LED_OUT(uchar X);				// LED单字节串行移位函数

unsigned char LED_0F[];		// LED字模表
 
unsigned char  LED_0F[] = 
{// 0	 1	  2	   3	4	 5	  6	   7	8	 9	  A	   b	C    d	  E    F    -
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf
};

uchar LED[8];	//用于LED的8位显示缓存

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
****入口参数：无
****出口参数：无
****函数备注：不精确延时函数
****版权信息：蓝旗嵌入式系统
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
****入口参数：需要发送的字符串
****出口参数：无
****函数备注：USART发送函数
****版权信息：蓝旗嵌入式系统
*******************************************************************************/
void USART1_SendStr(unsigned char *Str) 
{
        while(*Str!=0)//不为结束
        {
            USART_SendData8(USART1,*Str);     //发送数据 
            while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//等待发送完毕
            Str++;//下一个数据
        }
}
/*******************************************************************************
****入口参数：需要发送的16进制数
****出口参数：无
****函数备注：USART发送16进制函数
****版权信息：蓝旗嵌入式系统
*******************************************************************************/
void USART1_SendHex(unsigned char dat)
{
      USART_SendData8(USART1,'0');
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//等待发送完毕
      USART_SendData8(USART1,'x');
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//等待发送完毕
      USART_SendData8(USART1,HexTable[dat>>4]);
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//等待发送完毕
      USART_SendData8(USART1,HexTable[dat&0x0f]);
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//等待发送完毕
      USART_SendData8(USART1,' ');
      while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//等待发送完毕
}

//----------------------------------------------------------
//adc转换
void  ChangeAD()
{

   ADC_SoftwareStartConv (ADC1);//开启软件转换
              
   while(!ADC_GetFlagStatus (ADC1,ADC_FLAG_EOC));//等待转换结束
       ADC_ClearFlag (ADC1,ADC_FLAG_EOC);//清除相关标识
	
}

//------------------------------------------------------------
//打印16进制
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
	unsigned char *led_table;          // 查表指针
	uchar i;
	//显示第1位
	led_table = LED_0F + LED[3];
	i = *led_table;

	LED_OUT(i);	
	LED_OUT(0x01);		

	RCLKLow;
	RCLKHigh;
	//显示第2位
	led_table = LED_0F + LED[2];
	i = *led_table;

	LED_OUT(i);		
	LED_OUT(0x02);		

	RCLKLow;
	RCLKHigh;
        
        
#if 1
	//显示第3位
	led_table = LED_0F + LED[1];
	i = *led_table;

	LED_OUT(i);			
	LED_OUT(0x04);	

	RCLKLow;
	RCLKHigh;
        
        
	//显示第4位
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

//定时器2配置，1毫秒产生一次中断，用于系统计时

void TIM2_Init(void)   

{ 
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);//将主时钟信号送给定时器4(L系列单片机必需)
  TIM2_TimeBaseInit(TIM2_Prescaler_16,TIM2_CounterMode_Up,0x7D);            //定时器4,将主时钟16分频.自动装载FA=125
  TIM2_SetCounter(0xFA);                                //设定计数值FA=125
  TIM2_ITConfig(TIM2_IT_Update,ENABLE);
  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_Cmd(ENABLE);                                    //计数器使能，开始计数   
}
/*******************************************************************************
****函数说明：主函数
****入口参数:无
****出口参数:无
****函数备注:PB0(adc1-18)作为ADC输入口，可以通过杜邦线将3.3V或GND连至此口，ADC转换结果通过
             USART输出
********************************************************************************/
extern u16 temp;
void main(void)
{
  u16 u16_adc1_value; 
  u16 u16_adc2_value;
  
  GPIO_Init(LED_PORT,LED_PINS,GPIO_Mode_Out_PP_Low_Slow);//初始化LED端口
  GPIO_Init(KEY_PORT,KEY_PINS,GPIO_Mode_In_PU_No_IT);//初始化KEY端口，带上拉输入，不带中断
  
  GPIO_Init(DIO_PORT,DIO_PINS,GPIO_Mode_Out_PP_High_Fast);//初始化DIO端口
  GPIO_Init(RCLK_PORT,RCLK_PINS,GPIO_Mode_Out_PP_High_Fast);//初始化RCLK端口
  GPIO_Init(SCLK_PORT,SCLK_PINS,GPIO_Mode_Out_PP_High_Fast);//初始化RCLK端口
  
  
  //SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);//端口重映射，去掉注释之后USART1为PA2-TX、PA3-RX；注释之后USART1为TX-PC5、RX-PC6；复位之后USART会自动恢复至PC5、PC6
  
  CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);//开启USART时钟 
  USART_Init(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx|USART_Mode_Rx);//设置USART参数9600，8N1，接收/发送
  USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);//使能接收中断
  USART_Cmd (USART1,ENABLE);//使能USART
  
  CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,ENABLE);//开启ADC时钟
  ADC_Init (ADC1,ADC_ConversionMode_Single,ADC_Resolution_12Bit,ADC_Prescaler_1);//ADC1，单次采样，12位，1分频
  ADC_Cmd(ADC1,ENABLE);//ADC1使能
  //ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);//ADC1 18通道使能
  
   LED[0]=7;
   LED[1]=2;
   LED[2]=3;
   LED[3]=4;
   
   TIM2_Init();
   
   asm("rim"); 
   
  while (1)
  {
       #if 1
       ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);//ADC1 18通道使能
       USART1_SendStr("ADC1转换结果为: ");
       ChangeAD();
       u16_adc1_value=ADC_GetConversionValue (ADC1);//获取转换值
       ADC_ChannelCmd (ADC1,ADC_Channel_18,DISABLE);//ADC1 18通道使能

      // Delay(0xFFFF);
       u16_adc1_value = (u16_adc1_value*33000)>>12;
      
       DisplayData(u16_adc1_value);
       
        
       ADC_ChannelCmd (ADC1,ADC_Channel_4,ENABLE);//ADC1 17通道使能
       USART1_SendStr("ADC2转换结果为: ");
       ChangeAD();
       u16_adc2_value=ADC_GetConversionValue (ADC1);//获取转换值
       ADC_ChannelCmd (ADC1,ADC_Channel_4,DISABLE);//ADC1 17通道使能

      // Delay(0xFFFF);
       u16_adc2_value = (u16_adc2_value*33000)>>12;

       DisplayData(u16_adc2_value);
        
       #endif

       if(GPIO_ReadInputDataBit(KEY_PORT,KEY_PINS)==0)//读GPB1输入状态
       {
             Delay(0x3FFF);  //软件防抖
             if(GPIO_ReadInputDataBit(KEY_PORT,KEY_PINS)==0)  //读GPB1输入状态
                GPIO_ToggleBits(LED_PORT, LED_PINS);//翻转LED输出状态
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
