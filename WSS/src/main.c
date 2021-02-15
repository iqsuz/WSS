#include "inc/WSS.h"

bool initSystemClockFreq(void);
volatile WSSControllerStruct WSSController = {.isWSSConnected = false, .isFallingEdge = false, .isRisingEdge = false, .isTimeout = false, .WSSTimerValue = 0};

int main(){
	initSystemClockFreq();
	initWSS();

	//while(1){
		while(!(WSSController.isWSSConnected)){
			ADCProcessorTrigger(ADC_BASE, 3);
		}

		UARTprintf("[INFO] WSS has been connected!\r\n");

		TimerEnable(WSS_TIMER_BASE, TIMER_A);
		GPIOIntEnable(SENSOR_PORT, SENSOR_PIN);

		while(!WSSController.isTimeout && !WSSController.isRisingEdge);

		if(WSSController.isTimeout){
			uint32_t signalVoltageLevel;

			GPIOIntDisable(SENSOR_PORT, SENSOR_PIN);
			TimerDisable(WSS_TIMER_BASE, TIMER_A);

			//ADCSequenceEnable(ADC_BASE, 3);
			//ADCProcessorTrigger(ADC_BASE, 3);

			//while(!ADCIntStatus(ADC_BASE, 3, false));
			//ADCIntClear(ADC_BASE, 3);

			//ADCSequenceDataGet(ADC_BASE, 3, &signalVoltageLevel);

			//if(signalVoltageLevel >= MIN_VOLTAGE_LEVEL){
				UARTprintf("WSS doesn't have RSPA feature.\r\n");
				initLEDs(BLUE_LED);
			//}else{
			//	UARTprintf("Device has been disconnected before test completion!");
			//}
		}

		if(WSSController.isRisingEdge){
			while(!WSSController.isTimeout && !WSSController.isFallingEdge);

			if(WSSController.isTimeout){
				UARTprintf("It's RSPA is flagged RSPA type but test couldn't completed.");
				GPIOIntDisable(SENSOR_PORT, SENSOR_PIN);
				TimerDisable(WSS_TIMER_BASE, TIMER_A);
			}

			if(WSSController.isFallingEdge){
				uint32_t timerDiff;

				WSSController.WSSTimerValue = TimerValueGet(WSS_TIMER_BASE, TIMER_A);
				timerDiff = WSS_TIMER_LOAD_VAL - WSSController.WSSTimerValue;

				if(timerDiff >= RSPA_DUTY_LOWER && timerDiff <= RSPA_DUTY_UPPER){
					UARTprintf("WSS has RSPA feature.\r\n");
					initLEDs(GREEN_LED);

				}else{
					UARTprintf("RSPA spec error!.\r\n");
				}
				GPIOIntDisable(SENSOR_PORT, SENSOR_PIN);
				TimerDisable(WSS_TIMER_BASE, TIMER_A);
			}
		}


	//}

	return 1;
}

/*************SYSTEM CLOCK SET 80MHZ***************/
bool initSystemClockFreq(){
	SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_SYSDIV_2_5);
	return (SysCtlClockGet() == 80000000) ? true: false;
}


//ADC Conversion check
//ADCProcessorTrigger(ADC_BASE, 3);
//while(!ADCIntStatus(ADC_BASE, 3, false));
