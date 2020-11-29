/*
 * adc_global.h
 *
 *  Created on: Jul 27, 2017
 *      Author: jose
 */

#ifndef GENERALIO_ADC_GLOBAL_H_
#define GENERALIO_ADC_GLOBAL_H_

#include "stm32f1xx_hal.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

void MX_ADC2_Init(ADC_HandleTypeDef * hadc2);
void MX_ADC1_Init(ADC_HandleTypeDef * hadc1);

#ifndef LEAVE_ONLY_IRON_MEAS
typedef struct {
	uint32_t iron;
	uint32_t cold_junction;
	uint32_t supply;
} adc_measures_t;
#else

typedef struct {
	uint16_t iron[ADC_MEASURES_LEN];
//	uint32_t cold_junction;
//	uint32_t supply;
} adc_measures_t;
#endif


extern adc_measures_t adc_measures;


extern volatile uint16_t iron_temp_adc_avg;
#endif /* GENERALIO_ADC_GLOBAL_H_ */
