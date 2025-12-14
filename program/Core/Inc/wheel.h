// encoder_wheel.h
#ifndef ENCODER_WHEEL_H
#define ENCODER_WHEEL_H

#include "main.h"
#include "tim.h"

typedef struct{
	int64_t p;
	int64_t t;
}wheel_state;

void wheel_init(void);
void wheel_update(void);

#endif
