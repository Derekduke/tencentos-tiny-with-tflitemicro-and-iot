#include "tos_k.h"

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_tim.h"
#include "stm32l4xx_hal_rtc.h"

#if TOS_CFG_TICKLESS_EN > 0u

/*
    systick����Ҳ��ʵ��Ϊtickless�Ļ������ӣ�ʵ��ԭ���ǣ���ϵͳҪ����ticklessģʽʱ��
    ��systick�Ĵ���ʱ������
    ����˵����һ��������������ʱ����6000ms��ִ�У���ô��6000ms��systickû�б�Ҫһֱ����
    ���԰�systick���жϼ������Ϊ6000ms��������ʵ����systick�ڲ��ļ�ʱ�Ĵ���ֻ��24λ��systick
    �����жϼ����ʵ��С�����Բο�tickless_systick_wkup_alarm_max_delayʵ�֣�������ʹ��������
    ������Ȼ����ڽ���idleʱ����ʹ��cpu����sleepģʽҪ�ͣ���Ϊsystick���������ˣ�cpu������sleep
    ģʽ��ͣ�����ã�

    ʵ���ϣ�tickless_systick_wkup_alarm_dismiss��tickless_wkup_alarm_dismiss���ӿ��������ȱ�ݵģ�
    ��ΪCPU��tickless�����������ܲ�������Ϊ����ʱ�䵽�ˣ��п������ⲿ�ж������CPU���ѣ����һ����
    Ϊ�걸��ʵ��Ӧ���ǣ���tickless_wkup_alarm_dismiss�ӿڷ���ʵ��˯�ߵ�ʱ�䣨����ͨ�����ӵļ�ʱ�Ĵ�
    ����ֵ�������������ȱ�ݺ��ڻ������޸����ֽ׶ζ�Ӳ���в���Ϥ��

    ע�⣬systickֻ����Ϊsleepģʽ�µĻ������ӡ����ο�tos_pm.h��

    Ŀǰ��һ��ʵ���в��걸���кܶ��ⷽ������ѻ�ӭ�ͽ�~
 */
static void tickless_systick_suspend(void)
{
    cpu_systick_suspend();
    cpu_systick_pending_reset();
}

static void tickless_systick_resume(void)
{
    cpu_systick_resume();
}

static void tickless_systick_wkup_alarm_expires_set(k_time_t millisecond)
{
    cpu_systick_expires_set(millisecond);
}

static int tickless_systick_wkup_alarm_setup(k_time_t millisecond)
{
    tickless_systick_suspend();
    tickless_systick_wkup_alarm_expires_set(millisecond);
    tickless_systick_resume();
    return 0;
}

static int tickless_systick_wkup_alarm_dismiss(void)
{
    // TODO:
    // if not wakeup by systick(that's say another interrupt), need to identify this and fix
    return 0;
}

static k_time_t tickless_systick_wkup_alarm_max_delay(void)
{
    return cpu_systick_max_delay_millisecond();
}

k_tickless_wkup_alarm_t tickless_wkup_alarm_systick = {
    .init       = K_NULL,
    .setup      = tickless_systick_wkup_alarm_setup,
    .dismiss    = tickless_systick_wkup_alarm_dismiss,
    .max_delay  = tickless_systick_wkup_alarm_max_delay,
};


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/*
   ����timer6ʵ�ֵ�tickless�������ӣ�������ΪSLEEPģʽ�µĻ���Դ�����ο�tos_pm.h��
   Ŀǰ��һ��ʵ���в��걸���кܶ��ⷽ������ѻ�ӭ�ͽ�~
 */
static TIM_HandleTypeDef tim6;

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_handler)
{
    if (tim_handler->Instance == TIM6) {
        __HAL_RCC_TIM6_CLK_ENABLE();

        /* TIM6 interrupt Init */
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_handler)
{
    if (tim_handler->Instance == TIM6) {
        /* Peripheral clock disable */
        __HAL_RCC_TIM6_CLK_DISABLE();

        /* TIM6 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
    }
}

static int tickless_tim6_wkup_alarm_init(void)
{
    tim6.Instance = TIM6;
    tim6.Init.Prescaler = 0;
    tim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim6.Init.Period = 0;
    tim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&tim6);
    return 0;
}

static int tickless_tim6_wkup_alarm_setup(k_time_t millisecond)
{
    tim6.Init.Prescaler = 8000 - 1;
    tim6.Init.Period = (millisecond * 10) - 1;

    HAL_TIM_Base_Stop(&tim6);
    __HAL_TIM_CLEAR_IT(&tim6, TIM_IT_UPDATE);

    HAL_TIM_Base_Init(&tim6);
    HAL_TIM_Base_Start_IT(&tim6);
    return 0;
}

static int tickless_tim6_wkup_alarm_dismiss(void)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_CPU_INT_DISABLE();

    HAL_TIM_Base_Stop(&tim6);
    HAL_TIM_Base_Stop_IT(&tim6);

    TOS_CPU_INT_ENABLE();
    return 0;
}

static k_time_t tickless_tim6_wkup_alarm_max_delay(void)
{
    k_time_t millisecond;
    uint32_t max_period;

    max_period = ~((uint32_t)0u);
    millisecond = (max_period - 1) / 10;
    return millisecond;
}

void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&tim6);
}

k_tickless_wkup_alarm_t tickless_wkup_alarm_tim = {
    .init       = tickless_tim6_wkup_alarm_init,
    .setup      = tickless_tim6_wkup_alarm_setup,
    .dismiss    = tickless_tim6_wkup_alarm_dismiss,
    .max_delay  = tickless_tim6_wkup_alarm_max_delay,
};


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/*
   ����rtc wakeup�ж�ʵ�ֵ�tickless�������ӣ�������ΪSLEEP��STOPģʽ�µĻ���Դ�����ο�tos_pm.h��
   Ŀǰ��һ��ʵ���в��걸���кܶ��ⷽ������ѻ�ӭ�ͽ�~
 */
static RTC_HandleTypeDef rtc_handler;

static HAL_StatusTypeDef tickless_rtc_time_set(uint8_t hour, uint8_t minu, uint8_t sec, uint8_t format)
{
    RTC_TimeTypeDef rtc_time;

    rtc_time.Hours = hour;
    rtc_time.Minutes = minu;
    rtc_time.Seconds = sec;
    rtc_time.TimeFormat = format;
    rtc_time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    rtc_time.StoreOperation = RTC_STOREOPERATION_RESET;
    return HAL_RTC_SetTime(&rtc_handler, &rtc_time, RTC_FORMAT_BIN);
}

static HAL_StatusTypeDef tickless_rtc_date_set(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    RTC_DateTypeDef rtc_date;

    rtc_date.Date = date;
    rtc_date.Month = month;
    rtc_date.WeekDay = week;
    rtc_date.Year = year;
    return HAL_RTC_SetDate(&rtc_handler, &rtc_date, RTC_FORMAT_BIN);
}

static int tickless_rtc_wkup_alarm_init(void)
{
    rtc_handler.Instance = RTC;
    rtc_handler.Init.HourFormat = RTC_HOURFORMAT_24;
    rtc_handler.Init.AsynchPrediv = 0X7F;
    rtc_handler.Init.SynchPrediv = 0XFF;
    rtc_handler.Init.OutPut = RTC_OUTPUT_DISABLE;
    rtc_handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    rtc_handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&rtc_handler) != HAL_OK) {
        return -1;
    }

    if (HAL_RTCEx_BKUPRead(&rtc_handler, RTC_BKP_DR0) != 0X5050) {
        tickless_rtc_time_set(23, 59, 56, RTC_HOURFORMAT12_PM);
        tickless_rtc_date_set(15, 12, 27, 7);
        HAL_RTCEx_BKUPWrite(&rtc_handler, RTC_BKP_DR0,0X5050);
    }

    return 0;
}

static int tickless_rtc_wkupirq_wkup_alarm_setup(k_time_t millisecond)
{
    uint32_t wkup_clock = RTC_WAKEUPCLOCK_CK_SPRE_16BITS;
    if (millisecond < 1000) {
        millisecond = 1000;
    }
    uint32_t wkup_count = (millisecond / 1000) - 1;

    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&rtc_handler, RTC_FLAG_WUTF);

    HAL_RTCEx_SetWakeUpTimer_IT(&rtc_handler, wkup_count, wkup_clock);

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x02, 0x02);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    return 0;
}

static int tickless_rtc_wkupirq_wkup_alarm_dismiss(void)
{
#if defined(STM32F4) || defined(STM32L4)
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
#endif

    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&rtc_handler, RTC_FLAG_WUTF);

    if (HAL_RTCEx_DeactivateWakeUpTimer(&rtc_handler) != HAL_OK) {
        return -1;
    }

    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
    return 0;
}

static k_time_t tickless_rtc_wkupirq_wkup_alarm_max_delay(void)
{
    return 0xFFFF * K_TIME_MILLISEC_PER_SEC;
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *rtc_handler)
{
    RCC_OscInitTypeDef rcc_osc;
    RCC_PeriphCLKInitTypeDef periph_clock;

    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    rcc_osc.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    rcc_osc.PLL.PLLState = RCC_PLL_NONE;
    rcc_osc.LSEState = RCC_LSE_ON;
    HAL_RCC_OscConfig(&rcc_osc);

    periph_clock.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    periph_clock.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&periph_clock);

    __HAL_RCC_RTC_ENABLE();
}

void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&rtc_handler);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *rtc_handler)
{
}

k_tickless_wkup_alarm_t tickless_wkup_alarm_rtc_wkupirq = {
    .init       = tickless_rtc_wkup_alarm_init,
    .setup      = tickless_rtc_wkupirq_wkup_alarm_setup,
    .dismiss    = tickless_rtc_wkupirq_wkup_alarm_dismiss,
    .max_delay  = tickless_rtc_wkupirq_wkup_alarm_max_delay,
};



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/*
   ����rtc alarm�ж�ʵ�ֵ�tickless�������ӣ�������ΪSLEEP��STOP��STANDBYģʽ�µĻ���Դ�����ο�tos_pm.h��
   Ŀǰ��һ��ʵ���в��걸���кܶ��ⷽ������ѻ�ӭ�ͽ�~
 */
static int tickless_rtc_alarmirq_wkup_alarm_setup(k_time_t millisecond)
{
    uint8_t hour, minute, second, subsecond, date;

    RTC_AlarmTypeDef rtc_alarm;
    RTC_TimeTypeDef rtc_time;
    RTC_DateTypeDef rtc_date;

    HAL_RTC_GetTime(&rtc_handler, &rtc_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&rtc_handler, &rtc_date, RTC_FORMAT_BIN);

    hour = rtc_time.Hours;
    minute = rtc_time.Minutes;
    second = rtc_time.Seconds;
#if 0
    date = rtc_date.Date;
#else
    date = rtc_date.WeekDay;
#endif

    printf("before >>>  %d  %d %d %d\n", date, hour, minute, second);

    /* I know it's ugly, I will find a elegant way. Welcome to tell me, 3ks~ */
    second += millisecond / K_TIME_MILLISEC_PER_SEC;
    if (second >= 60) {
        minute += 1;
        second -= 60;
    }
    if (minute >= 60) {
        hour += 1;
        minute -= 60;
    }
    if (hour >= 24) {
        date += 1;
        hour -= 24;
    }

    printf("after >>>  %d  %d %d %d\n", date, hour, minute, second);

    rtc_alarm.AlarmTime.Hours = hour;
    rtc_alarm.AlarmTime.Minutes = minute;
    rtc_alarm.AlarmTime.Seconds = second;
    rtc_alarm.AlarmTime.SubSeconds = 0;
    rtc_alarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;

    rtc_alarm.AlarmMask = RTC_ALARMMASK_NONE;
    rtc_alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    rtc_alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY; // RTC_ALARMDATEWEEKDAYSEL_DATE; // RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    rtc_alarm.AlarmDateWeekDay = date;
    rtc_alarm.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&rtc_handler, &rtc_alarm, RTC_FORMAT_BIN);

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x01, 0x02);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);


    // __HAL_PWR_GET_FLAG(PWR_FLAG_WU)


    __HAL_RCC_AHB1_FORCE_RESET();       //��λ����IO��
    __HAL_RCC_PWR_CLK_ENABLE();         //ʹ��PWRʱ��

    // __HAL_RCC_BACKUPRESET_FORCE();      //��λ��������
    HAL_PWR_EnableBkUpAccess();         //���������ʹ��

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&rtc_handler);//�ر�RTCд����

    //�ر�RTC����ж�
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&rtc_handler,RTC_IT_WUT);
#if 0
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&rtc_handler,RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&rtc_handler,RTC_IT_ALRA|RTC_IT_ALRB);
#endif

    //���RTC����жϱ�־λ
    __HAL_RTC_ALARM_CLEAR_FLAG(&rtc_handler,RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&rtc_handler,RTC_FLAG_TSF);
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&rtc_handler,RTC_FLAG_WUTF);

    // __HAL_RCC_BACKUPRESET_RELEASE();                    //��������λ����
    __HAL_RTC_WRITEPROTECTION_ENABLE(&rtc_handler);     //ʹ��RTCд����


#ifdef STM32F4
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //���Wake_UP��־
#endif

#ifdef STM32F7
    // __HAL_PWR_CLEAR_WAKEUP_FLAG(PWR_WAKEUP_PIN_FLAG1);  //���Wake_UP��־
#endif

    // HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //����WKUP���ڻ���

    return 0;
}

static int tickless_rtc_alarmirq_wkup_alarm_dismiss(void)
{
#if 1
    // __HAL_PWR_GET_FLAG(PWR_FLAG_WU);

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    // __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&rtc_handler, RTC_FLAG_ALRAF);

    __HAL_RTC_ALARM_CLEAR_FLAG(&rtc_handler, RTC_FLAG_ALRAF);

#if 0
    if (HAL_RTCEx_DeactivateWakeUpTimer(&rtc_handler) != HAL_OK) {
        return -1;
    }
#endif

    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
    return 0;
#endif
}

static k_time_t tickless_rtc_alarmirq_wkup_alarm_max_delay(void)
{
    return 0xFFFF; // just kidding, I will fix it out. Welcome to tell me, 3ks~ */
}

void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&rtc_handler);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *rtc_handler)
{
}

k_tickless_wkup_alarm_t tickless_wkup_alarm_rtc_alarmirq = {
    .init       = tickless_rtc_wkup_alarm_init,
    .setup      = tickless_rtc_alarmirq_wkup_alarm_setup,
    .dismiss    = tickless_rtc_alarmirq_wkup_alarm_dismiss,
    .max_delay  = tickless_rtc_alarmirq_wkup_alarm_max_delay,
};

#endif

