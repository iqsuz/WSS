#include "WSS.h"

extern WSSControllerStruct WSSController;

void initWSS(){
	initUART();
	UARTSendString("[INFO] Processor is awake!. Initializations have been started.\r\n");
	UARTSendString("[INFO] UART is ready to use!.\r\n");
	UARTSendString("[SETUP] RX --> C4, TX --> C5.\r\n\r\n");

	UARTSendString("[SETUP] LEDs are being initialized.\r\n");
	//initLEDs(RED_LED | BLUE_LED | GREEN_LED);
	UARTSendString("[INFO] LEDs are ready to use!.\r\n\r\n");

	UARTSendString("[SETUP] ADC Unit is being initialized.\r\n");
	initADC();
	UARTSendString("[INFO] ADC Unit is ready to use!.\r\n\r\n");
	UARTSendString("[SETUP] PIN E3.\r\n\r\n");

	UARTSendString("[SETUP] External interrupt for WSS signal is being initialized.\r\n");
	initSignalInterrupt();
	UARTSendString("[INFO] External interrupt for WSS signal is ready!\r\n");
	UARTSendString("[SETUP] uC waits signal from D0 pin.\r\n\r\n");

	UARTSendString("[SETUP] Timer Unit is being initialized.\r\n");
	initTimer();
	UARTSendString("[INFO] Timer Unit is ready to use!.\r\n\r\n");

	UARTSendString("[INFO] Initialization is done! Ready to use!\r\n\r\n");
}

void initLEDs(uint8_t pins){
	SysCtlPeripheralEnable(LEDS_GPIO_CLK);
	GPIOPinTypeGPIOOutput(LEDS_GPIO_BASE, pins);
	GPIOPinWrite(LEDS_GPIO_BASE, pins , pins);
}

void turnOnLED(uint8_t pins, uint8_t val){
	//Write this code.
}

void initButtonInterrupt(){
	SysCtlPeripheralEnable(START_BUTTON_GPIO_CLK);
	GPIOPinTypeGPIOInput(START_BUTTON_PORT, START_BUTTON_PIN);
	GPIOPadConfigSet(START_BUTTON_PORT, START_BUTTON_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOIntTypeSet(START_BUTTON_PORT, START_BUTTON_PIN, GPIO_BOTH_EDGES);
	GPIOIntRegister(START_BUTTON_PORT, intButtonHandler);
	GPIOIntEnable(START_BUTTON_PORT, START_BUTTON_PIN);

}

void intButtonHandler(){
	uint32_t status = GPIOIntStatus(START_BUTTON_PORT, true);
	GPIOIntClear(START_BUTTON_PORT, status);

	if((status & START_BUTTON_PIN) == START_BUTTON_PIN){
		UARTSendString("Signal has been triggered2.\r\n");
	}
}

void initUART(){
	SysCtlPeripheralEnable(WSS_UART_CLK);
	SysCtlPeripheralEnable(WSS_UART_GPIO_CLK);

	GPIOPinConfigure(WSS_UART_RX_PIN_CONFIG);
	GPIOPinConfigure(WSS_UART_TX_PIN_CONFIG);

	GPIOPinTypeUART(WSS_UART_PORT_BASE, WSS_UART_RX_PIN | WSS_UART_TX_PIN);

	UARTConfigSetExpClk(WSS_UART_BASE, SysCtlClockGet(), WSS_UART_BAUDRATE,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	UARTStdioConfig(1, WSS_UART_BAUDRATE, SysCtlClockGet());
}

void UARTSendString(char *s){
	while(*s){
		UARTCharPut(WSS_UART_BASE, *s);
		s++;
	}
}

void initTimer(){
	SysCtlPeripheralEnable(WSS_TIMER_CLK);

	TimerConfigure(WSS_TIMER_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(WSS_TIMER_BASE, TIMER_A, WSS_TIMER_LOAD_VAL);	//Set count down value.

	TimerIntRegister(WSS_TIMER_BASE, TIMER_A, intTimerHandler);
	TimerIntEnable(WSS_TIMER_BASE, TIMER_TIMA_TIMEOUT);		//Enable interrupt for timeout.

	TimerDisable(WSS_TIMER_BASE, TIMER_A);	//Do not count yet.
}

void intTimerHandler(){
	WSSController.isTimeout = true;
	TimerDisable(WSS_TIMER_BASE, TIMER_A);

	UARTprintf("Timeout!\r\n");

	TimerIntClear(WSS_TIMER_BASE, TIMER_TIMA_TIMEOUT); //Interrupt flag clear. It's necessary.
}

void initADC(){
	SysCtlPeripheralEnable(ADC_CLK);
	SysCtlPeripheralEnable(ADC_GPIO_CLK);

	GPIOPinTypeADC(ADC_GPIO_PORT, ADC_PIN);

	ADCSequenceConfigure(ADC_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

	ADCIntRegister(ADC_BASE, 3, intADCHandler);
	ADCIntEnable(ADC_BASE, 3);

	ADCSequenceEnable(ADC_BASE, 3);		//Activate ADC unit.
	ADCIntClear(ADC_BASE, 3);		//Enable conversion done interrupt.
}

void intADCHandler(){
	uint32_t signalVoltageLevel;
	ADCSequenceDataGet(ADC_BASE, 3, &signalVoltageLevel);

	if(signalVoltageLevel >= MIN_VOLTAGE_LEVEL){
		WSSController.isWSSConnected = true;
		ADCSequenceDisable(ADC_BASE, 3);
	}else{
		WSSController.isWSSConnected = false;
	}

	ADCIntClear(ADC_BASE, 3);
}

void initSignalInterrupt(){
	SysCtlPeripheralEnable(SENSOR_PORT_GPIO_CLK);

	GPIOPinTypeGPIOInput(SENSOR_PORT, SENSOR_PIN);
	GPIOPadConfigSet(SENSOR_PORT, SENSOR_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

	GPIOIntTypeSet(SENSOR_PORT, SENSOR_PIN, GPIO_BOTH_EDGES);
	GPIOIntRegister(SENSOR_PORT, intSignalHandler);

	GPIOIntDisable(SENSOR_PORT, SENSOR_PIN);	//Do not enable interrupt yet.
}


void intSignalHandler(){
	uint32_t status = GPIOIntStatus(SENSOR_PORT, true);

	if((status & SENSOR_PIN) == SENSOR_PIN){		//Check if interrupt from correct source.
		bool pinStatus = ((GPIOPinRead(SENSOR_PORT, SENSOR_PIN) & SENSOR_PIN) == SENSOR_PIN) ? true: false;	// Determine whether it is falling or rising edge.

		WSSController.isFallingEdge = (pinStatus == false) ? true: false;
		WSSController.isRisingEdge = (pinStatus == true) ? true: false;

		if(pinStatus){
			TimerLoadSet(WSS_TIMER_BASE, TIMER_A, WSS_TIMER_LOAD_VAL);
		}else{
			TimerDisable(WSS_TIMER_BASE, TIMER_A);
			GPIOIntDisable(SENSOR_PORT, SENSOR_PIN);
		}
	}

	GPIOIntClear(SENSOR_PORT, status);		//Clear interrupt flag.
}
