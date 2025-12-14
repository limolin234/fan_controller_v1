#include <stdint.h>
#include "tim.h"
#include "resistor_calculate.h"

extern uint8_t data;
extern int volt;

struct pair{
	uint16_t v;
	uint8_t d;
};

void resistor_calculate(){
	static int volt_reserved=0;
	if(volt==volt_reserved)return;
	volt_reserved=volt;
	__HAL_TIM_SET_COUNTER(&htim2,0);
	if(volt<200){
		data=255;
		return;
	}
	if(volt>1101||volt<599){
		data=14715/(volt-122)+1;
		return;
	}
	static const pair list[17]={
		{1101,15},{1041,16},{986,17},{939,18},{897,19},{859,20},{823,21},
		{792,22},{763,23},{737,24},{711,25},{689,26},{669,27},{650,28},{631,29},
		{615,30},{599,31}			
	};
	if(volt>=599){
		int ptr0=0;
		int ptr1=17;
		int ave;
		int temp;
		while(1){
			ave=(ptr0+ptr1)/2;
			temp=list[ave].v;
			if(volt>temp) ptr1=ave;
			else ptr0=ave;
			if(ptr1-ptr0==1){
				if(list[ptr0].v<=volt){
					data=list[ptr0].d;
					return;
				}else{
					data=list[ptr1].d;
					return;
				}
			}
		}
	}
}
