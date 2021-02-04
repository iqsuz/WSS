#ifndef INC_WSS_H_
#define INC_WSS_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"

#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

enum LED_DEF{
	RED_LED = GPIO_PIN_1,
	BLUE_LED = GPIO_PIN_2,
	GREEN_LED = GPIO_PIN_3
};


void initWSS(void);
static void initLEDs(uint8_t);

#endif /* INC_WSS_H_ */
