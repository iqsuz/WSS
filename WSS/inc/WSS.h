/*
**************PINS AND PORTS*************
--------------      UART    -------------
PORT: PORT C
PINS: RX --> C4, TX --> C5

--------------    BUTTON    -------------
PORT: PORT F
PIN: PIN F4

--------------      LEDs    -------------
PORT: PORT F
PINS: RED --> F1, BLUE --> F2, GREEN --> F3


--------------      ADC     -------------
PORT: PORT E
PIN: PIN E3

-------------   EXT SIGNAL  -------------
PORT: PORT D
PIN: PIN D0


*/
#ifndef INC_WSS_H_
#define INC_WSS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_adc.h"

#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"

#include "utils/uartstdio.h"

#define ADC_RESOLUTION 0.0008056640625

enum WSS_LED_DEF{
	RED_LED = GPIO_PIN_1,
	BLUE_LED = GPIO_PIN_2,
	GREEN_LED = GPIO_PIN_3,
	LEDS_GPIO_CLK = SYSCTL_PERIPH_GPIOF,
	LEDS_GPIO_BASE = GPIO_PORTF_BASE
};

enum WSS_BUTTON_DEF{
	START_BUTTON_PORT = GPIO_PORTF_BASE,
	START_BUTTON_PIN = GPIO_PIN_4,
	START_BUTTON_GPIO_CLK = SYSCTL_PERIPH_GPIOF,
};

enum WSS_SIGNAL_DEF{
	SENSOR_PORT_GPIO_CLK = SYSCTL_PERIPH_GPIOD,
	SENSOR_PORT = GPIO_PORTD_BASE,
	SENSOR_PIN = GPIO_PIN_0,
};

enum WSS_UART_DEF{
	WSS_UART_BAUDRATE = 9600,
	WSS_UART_PORT_BASE = GPIO_PORTC_BASE,
	WSS_UART_BASE = UART1_BASE,
	WSS_UART_CLK = SYSCTL_PERIPH_UART1,
	WSS_UART_GPIO_CLK = SYSCTL_PERIPH_GPIOC,
	WSS_UART_RX_PIN_CONFIG = GPIO_PC4_U1RX,
	WSS_UART_TX_PIN_CONFIG = GPIO_PC5_U1TX,
	WSS_UART_RX_PIN = GPIO_PIN_4,
	WSS_UART_TX_PIN = GPIO_PIN_5
};

enum WSS_ADC_DEF{
	ADC_GPIO_CLK = SYSCTL_PERIPH_GPIOE,
	ADC_CLK = SYSCTL_PERIPH_ADC0,
	ADC_GPIO_PORT = GPIO_PORTE_BASE,
	ADC_BASE = ADC0_BASE,
	ADC_PIN = GPIO_PIN_3,
};

enum WSS_TIMER_DEF{
	WSS_TIMER_CLK = SYSCTL_PERIPH_TIMER0,
	WSS_TIMER_BASE = TIMER0_BASE,
	WSS_TIMER_LOAD_VAL = (uint32_t)160000000	//12.5n Second * 40,000,000 = 5sn.

};

enum SIGNAL_THRESHOLD{
	MIN_VOLTAGE_LEVEL = 500,
	RSPA_DUTY_UPPER = 135000,
	RSPA_DUTY_LOWER = 100000,
	nRSPA_DUTY_TOLERANCE = 15999850
};

typedef enum{
	nRSPA,
	RSPA,
	NAType
}WSSTypeEnum;

typedef struct{
	bool isWSSConnected;
	bool isRisingEdge;
	bool isFallingEdge;
	bool isTimeout;
	uint32_t WSSTimerValue;
}WSSControllerStruct;


void initWSS(void);
void initLEDs(uint8_t);
void turnOnLED(uint8_t, uint8_t);
void initButtonInterrupt(void);
void intButtonHandler(void);
void initUART();
void UARTSendString(char *s);
void initTimer(void);
void intTimerHandler(void);
void initADC(void);
void intADCHandler(void);
void initSignalInterrupt(void);
void intSignalHandler(void);

#endif /* INC_WSS_H_ */
