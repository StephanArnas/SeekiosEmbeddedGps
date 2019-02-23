#include "port_sgs.h"
#include "usb_main.h"
#include "sleep_manager_main.h"
#include "helper_sgs.h"
#include "hal_usart_async.h"
#include <seekiosCore/start.h>

int main(void)
{
	seekiosManagerStarted = false;
	system_init();
	serial_io_init();
	hardware_setup();
	uint16_t buffer[2];
 	ADC_read_BAT_USB(buffer);
	delay_ms(4);

	send_debug("Build-Date:   ");
	delay_ms(4);
    send_debug(__TIMESTAMP__);
	send_debug("\r\nInitialization SGS done.\r\n");
	//main_menu();
	start_start();
 	while(1) 
	 {
		send_debug("Going to sleep...");
 		_go_to_sleep();
	}
}
