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

uint16_t set_temp = 320;
uint16_t show_temp;
uint32_t tmo;

#define LIM_HIGH(x, lim) (((x)>(lim))?(lim):(x))
#define ABS(x) (((x)<0)?(-1*(x)):(x))
void mmain(void){
	static uint16_t prev_show_temp=-1;
	static uint16_t prev_show_power_len;
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
			if((set_temp)>400){
				set_temp = 400;
			}
			tmo = 10; //ticks
		}

		if(tmo){
			tmo--;
			show_temp = set_temp;
			if(!tmo){
				//UG_SetForecolor(//0xFFF)
				setCurrentTemperature(set_temp);
			};
		}else{
			if( is_iron_connected() ){
				show_temp = getIronTemp();
			}else{
				show_temp = 0;
			}
		}

		uint16_t show_power = getCurrentPower();
		int diff = (int)show_power- prev_show_power_len;
		if( ABS(diff) >=1){
			UG_FillFrame(0,55,128,64,0);
			if(show_power){
				UG_DrawMesh(0,55, LIM_HIGH(show_power*2,128),64, C_GREEN_YELLOW);
				oled_draw();
			}
			prev_show_power_len = show_power;
		}



		if(show_temp != prev_show_temp){
			if(show_temp){
				snprintf(temp_str, 10, "%3u", show_temp);
				UG_PutString(10 , 0 , temp_str);
			}else{
				UG_PutString(10 , 0 , "---");
			}

			oled_draw();
			prev_show_temp = show_temp;
		}else{
			HAL_Delay(100);
		}

		HAL_IWDG_Refresh(&hiwdg);
	}
}

