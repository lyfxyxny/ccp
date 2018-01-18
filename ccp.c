#include "ccp.h"
TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;
uint32_t CcpLowCont=0;
uint32_t CcpHighCont=0;
uint32_t CcpHighAndLowCont=0;
uint32_t DutyTrtion=0;    //占空比
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
延时函数
*/
void delay()
{
  int i,j;
  for(i=0;i<10000;i++)
    for(j=0;j<1000;j++);
}


/*
管脚初始化  把1.26配置成CAP0.0
初始化定时器0 配置为捕捉模式 上升沿模式

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
  TIM_ConfigStruct.PrescaleValue	= 1;   //分频为1US计数一次
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
定时器0中断 用于捕捉功能
twocap   这个值是0则为上升沿 为1为下降沿捕捉

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
      riscont=TIM_GetCaptureValue(LPC_TIM0,0);  //上升沿捕捉到的值
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
        fallcont=TIM_GetCaptureValue(LPC_TIM0,0);  //下降沿捕捉到的值
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
