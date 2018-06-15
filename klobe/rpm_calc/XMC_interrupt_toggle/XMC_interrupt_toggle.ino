const byte tickPin = 8;


extern "C" {
  void CCU42_0_IRQHandler(void)
  {
    digitalToggle(LED1);
    //digitalToggle(tickPin);
  }
}


void setup() { 
  pinMode(LED1, OUTPUT);
  pinMode(tickPin, OUTPUT);
  
  XMC_CCU4_SLICE_COMPARE_CONFIG_t pwm_config = {0};
            pwm_config.passive_level = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_HIGH;
            pwm_config.prescaler_initval = XMC_CCU4_SLICE_PRESCALER_1;  // 144 MHZ

  XMC_CCU4_Init(CCU42, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);

  XMC_CCU4_SLICE_CompareInit(CCU42_CC43, &pwm_config);

  XMC_CCU4_EnableClock(CCU42, 3);

  XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU42_CC43, 3600); // 144 MHZ / 7200 = 20KHz

  /* Enable compare match and period match events */
  XMC_CCU4_SLICE_EnableEvent(CCU42_CC43, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);

  /* Connect period match event to SR0 */
  XMC_CCU4_SLICE_SetInterruptNode(CCU42_CC43, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);
  
  /* Configure NVIC */

  /* Set priority */
  NVIC_SetPriority(CCU42_0_IRQn, 10U);

  /* Enable IRQ */
  NVIC_EnableIRQ(CCU42_0_IRQn); 
  XMC_CCU4_EnableShadowTransfer(CCU42, (CCU4_GCSS_S0SE_Msk << (4 * 3)));
  XMC_CCU4_SLICE_StartTimer(CCU42_CC43);
}

void loop() {
  // put your main code here, to run repeatedly:

}

