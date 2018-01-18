#include "ccp.h"
TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;
uint32_t CcpLowCont=0;
uint32_t CcpHighCont=0;
uint32_t CcpHighAndLowCont=0;
uint32_t DutyTrtion=0;    //ռ�ձ�
static void delay_ms(uint16_t time)
{
  uint16_t i=0,j;
  for(i=time; i>0; i--)
    for(j=1000; j>0; j--);
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
��ʱ����
*/
void delay()
{
  int i,j;
  for(i=0;i<10000;i++)
    for(j=0;j<1000;j++);
}


/*
�ܽų�ʼ��  ��1.26���ó�CAP0.0
��ʼ����ʱ��0 ����Ϊ��׽ģʽ ������ģʽ

*/
void boardinit(void)
{
  
  
  PINSEL_CFG_Type PinCfg;
  TIM_TIMERCFG_Type TIM_ConfigStruct;
  
  NVIC_SetPriorityGrouping(0x05);
  PinCfg.Funcnum = 3;
  PinCfg.OpenDrain = 0;
  PinCfg.Pinmode = 0;
  PinCfg.Portnum = 1;
  PinCfg.Pinnum = 26;
  PINSEL_ConfigPin(&PinCfg);
  
  TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
  TIM_ConfigStruct.PrescaleValue	= 1;   //��ƵΪ1US����һ��
  TIM_CaptureConfigStruct.CaptureChannel = 0;
  TIM_CaptureConfigStruct.RisingEdge = ENABLE;
  TIM_CaptureConfigStruct.FallingEdge = DISABLE;
  TIM_CaptureConfigStruct.IntOnCaption = ENABLE;
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
  TIM_ConfigCapture(LPC_TIM0, &TIM_CaptureConfigStruct);
  TIM_ResetCounter(LPC_TIM0);
  NVIC_SetPriority(TIMER0_IRQn, 1);
  NVIC_EnableIRQ(TIMER0_IRQn);
  TIM_Cmd(LPC_TIM0,ENABLE);
  NVIC_SetPriority(TIMER0_IRQn, 1);
  NVIC_EnableIRQ(TIMER0_IRQn);
  TIM_Cmd(LPC_TIM0,ENABLE);
}


/*
��ʱ��0�ж� ���ڲ�׽����
twocap   ���ֵ��0��Ϊ������ Ϊ1Ϊ�½��ز�׽

*/
void TIMER0_IRQHandler(void)
{

  static char twocap=0; 
  static uint32_t riscont=0;
  static uint32_t fallcont=0;
  if (TIM_GetIntCaptureStatus(LPC_TIM0,0))
  {
    TIM_ClearIntCapturePending(LPC_TIM0,0);
    if(twocap==0){
      twocap=1;
      TIM_CaptureConfigStruct.RisingEdge =DISABLE ;
      TIM_CaptureConfigStruct.FallingEdge = ENABLE;
      TIM_ConfigCapture(LPC_TIM0, &TIM_CaptureConfigStruct);   
      riscont=TIM_GetCaptureValue(LPC_TIM0,0);  //�����ز�׽����ֵ
      if(riscont-fallcont<0) {
          CcpLowCont=riscont-fallcont+0xfffffffff;
      }
      else {
        CcpLowCont=riscont-fallcont;
      }

    }
    else{
      if(twocap==1){
        twocap=0;
        fallcont=TIM_GetCaptureValue(LPC_TIM0,0);  //�½��ز�׽����ֵ
        //TIM_ResetCounter(LPC_TIM0);
        TIM_CaptureConfigStruct.RisingEdge =ENABLE ;        
        TIM_CaptureConfigStruct.FallingEdge = DISABLE;
        TIM_ConfigCapture(LPC_TIM0, &TIM_CaptureConfigStruct); 
              if(fallcont-riscont<0) {
          CcpHighCont=fallcont-riscont+0xfffffffff;
      }
      else {
        CcpHighCont=fallcont-riscont;
      }
        CcpHighAndLowCont=CcpHighCont+CcpLowCont;
        
        DutyTrtion=(CcpHighCont*100)/CcpHighAndLowCont;       
      }
    }
    /*  if(twocap==0){
    twocap=1;
    CCPCONT=TIM_GetCaptureValue(LPC_TIM0,0);
  }else{
    if(twocap==1){
    twocap=0;
    CCPCONT1=TIM_GetCaptureValue(LPC_TIM0,0);
    CCPCONT1=CCPCONT1-CCPCONT;
    
  }
  }   
    */     
  }
}
