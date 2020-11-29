#include "irq.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "init.h"
#include "adc_global.h"
#include "iron.h"
#include "string.h" //memset
#include "filtrai.h"
#include "config.h"

#define IRQ_CB_LAG 1
#define CNDTR_IDX  POS( ADC_MEASURES_LEN - CNDTR )

volatile uint16_t iron_temp_adc_avg = 0;
uint16_t new_len;
volatile int good_vals;

uint16_t new_pwm_cc = 0;
extern TIM_HandleTypeDef tim3_pwm;
void tim3_pulseFinishCb(TIM_HandleTypeDef *htim){
	static uint16_t prev_pwm_cc = 0;
	if(prev_pwm_cc != new_pwm_cc){
		__HAL_TIM_SET_COMPARE(&tim3_pwm, TIM_CHANNEL_3, new_pwm_cc);
		prev_pwm_cc = new_pwm_cc;
	}

}


void flawless_adc_ConvCpltCb(ADC_HandleTypeDef *htim){
		static uint8_t was_iron_con = -1;
		static uint8_t is_iron_con;
		TICK;
		HAL_ADCEx_MultiModeStop_DMA(&hadc1);
		arr_set_zeros_above_threshold(&adc_measures.iron[0],ADC_MEASURES_LEN, 4000, 10, 35 );

		iron_temp_adc_avg = arr_u16_avg_ignore_val(0, &adc_measures.iron[30], ADC_MEASURES_LEN-30, &new_len);
		if(new_len <=2){
			iron_temp_adc_avg = 4055;
		}
		
		is_iron_con = (iron_temp_adc_avg > 4000)?0:1;
		
		new_pwm_cc = update_pwm();
		if(is_iron_con != was_iron_con){
			if(is_iron_con){
				__HAL_TIM_SET_COMPARE(&tim3_pwm, TIM_CHANNEL_3, 0);
				HAL_TIM_PWM_Start(&tim3_pwm, TIM_CHANNEL_3);
				set_iron_con(1);
			}else{
				__HAL_TIM_SET_COMPARE(&tim3_pwm, TIM_CHANNEL_3, 0);
				HAL_TIM_PWM_Stop(&tim3_pwm, TIM_CHANNEL_3);
				set_iron_con(0);
			}
			was_iron_con = is_iron_con;
		}



		HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*) &adc_measures.iron, ADC_MEASURES_LEN ); /* never stop adc */
		Benchmark._08_adc_proc_dur = TOCK;
}

void HardFault_Handler(void){
	HAL_GPIO_WritePin(T12PWM_GPIO_Port,T12PWM_Pin, 0);
	HAL_GPIO_DeInit(T12PWM_GPIO_Port, T12PWM_Pin);
	if(DWT->CYCCNT){
		__asm("BKPT 0");
	}


}






