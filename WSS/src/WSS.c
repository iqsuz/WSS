#include "WSS.h"

void initWSS(){
	initLEDs(RED_LED | BLUE_LED | GREEN_LED); //LEDs are being initialed.

	//Init interrupt for button
    //Init interrupt for signal
    //Init timer
}

static void initLEDs(uint8_t pins){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, pins);
	GPIOPinWrite(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED ,0x08);
}

static void initButtonInterrupt(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOInput(START_BUTTON_PORT, START_BUTTON_PIN);

}
