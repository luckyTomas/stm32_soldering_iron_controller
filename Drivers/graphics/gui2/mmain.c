#include "mmain.h"
#include "ugui.h"
#include "stm32f1xx_hal.h"
#include "rotary_encoder.h"
#include "tempsensors.h"
#include "ssd1306.h"

#include <stdio.h>

uint16_t getIronTemp() {
	uint16_t temp;
	temp = (uint16_t)(readTipTemperatureCompensated(1)+0.5);
	return temp;
}

void oled_draw() {
	UG_Update();
	update_display();
}
extern IWDG_HandleTypeDef hiwdg;

uint16_t set_temp = 300;
uint16_t show_temp;
uint32_t tmo;
void mmain(void){
	static uint16_t prev_show_temp=-1;
	UG_FontSelect(&FONT_32X53);
	RE_Rotation_t rot = RE_Get(&RE1_Data);
	setCurrentTemperature(set_temp);
	int inc;
	char temp_str[10];
	tmo = 0;
	while(1){
		rot = RE_Get(&RE1_Data);

		if(rot == Rotate_Increment || rot == Rotate_Decrement){
			inc = 5 * RE1_Data.Diff;
			set_temp += inc;
			if((set_temp)>350){
				set_temp = 350;
			}
			tmo = 30; //ticks
		}

		if(tmo){
			tmo--;
			show_temp = set_temp;
			if(!tmo){
				setCurrentTemperature(set_temp);
			};
		}else{
			if( is_iron_connected() ){
				show_temp = getIronTemp();
			}else{
				show_temp = 0;
			}
		}

		if(show_temp != prev_show_temp){
			if(show_temp){
				snprintf(temp_str, 10, "%3u", show_temp);
				UG_PutString(10 , 10 , temp_str);
			}else{
				UG_PutString(10 , 10 , "---");
			}

			oled_draw();
			prev_show_temp = show_temp;
		}else{
			HAL_Delay(100);
		}

		HAL_IWDG_Refresh(&hiwdg);
	}
}

