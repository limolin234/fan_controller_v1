// encoder_wheel.c
#include "wheel.h"
#include <stdbool.h>

static wheel_state position_n={0},position_o={0};
extern int volt;

void wheel_init(){
	position_n.p=__HAL_TIM_GetCounter(&htim21);
	position_n.t=HAL_GetTick();
}
void wheel_update(void){
	position_o.p=position_n.p;
	position_o.t=position_n.t;
	position_n.p=__HAL_TIM_GetCounter(&htim21);
	position_n.t=HAL_GetTick();
	if(position_n.t-position_o.t<0) position_o.t-=65532;
	if(position_n.p-position_o.p<-32766) position_o.p-=65532;
	if(position_n.p-position_o.p>32766) position_o.p+=65532;
	int deeps=(position_n.t-position_o.t)/(position_n.p-position_o.p);
	if(deeps<5&&deeps>-5) volt+=(position_n.p-position_o.p)*(-15);
	volt+=(position_n.p-position_o.p)*(-5);
	if(volt<0)volt=0;
	if(volt>1200)volt=1200;
}

