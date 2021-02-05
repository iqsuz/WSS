#include "inc/WSS.h"

structController WSSController = {.isButton = false};

int main(void)
{

	initWSS();

	while(1){
		while(WSSController.isButton);
	}

	return 0;
}
