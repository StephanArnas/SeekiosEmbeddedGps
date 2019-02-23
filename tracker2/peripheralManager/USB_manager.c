#include <peripheralManager/USB_manager.h>

static bool rxCallback(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count);
static bool txCallback(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count);
static bool lineStateChangeCallback(usb_cdc_control_signal_t state);

/* After the USB is configured, starts the USB start and waits for it to be enabled. */

/** Buffers to receive and echo the communication bytes. */
static uint32_t USBBuffer[64];
#define USB_BUF_LEN 512
uint8_t usb_buf[USB_BUF_LEN];
static SemaphoreHandle_t _usbBufMutex;

void USBManager_init()
{
	_usbBufMutex = xSemaphoreCreateMutex();
}

bool USBManager_start()
{
	cdc_device_acm_init();

	while (!cdcdf_acm_is_enabled()) {
		// wait cdc acm to be installed
	};

	cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)rxCallback);
	cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)txCallback);
	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)lineStateChangeCallback);
	return true;
}

/* The  cdcdf_acm_write function doesn't allow const char* strings, which is why you have to copy inside a RAM buffer (usb_buf) the
const char* string so it can be used with the function. */
void USBManager_print(const char* string)
{
	if(xSemaphoreTake(_usbBufMutex, 100) == pdPASS) // Semaphore released in the TX callbacks
	{
		strcpy(usb_buf, string);
		uint16_t len = strlen(usb_buf);
		if(len > 0)
		{
			int32_t result = cdcdf_acm_write(usb_buf, len);
			if(result != USB_OK)
			{
				xSemaphoreGive(_usbBufMutex);
			}
		}
	}
}

void USBManager_println(const char* string)
{
	if(xSemaphoreTake(_usbBufMutex, 100) == pdPASS) // Semaphore released in the TX callback
	{
		//memset(usb_buf, 0, USB_BUF_LEN);
		strcpy(usb_buf, string);
		strcat(usb_buf, "\r\n");
		uint16_t len = strlen(usb_buf);
		if(len > 0)
		{
			int32_t result = cdcdf_acm_write(usb_buf, len);
			if(result != USB_OK)
			{
				xSemaphoreGive(_usbBufMutex);
			}
		}
	}
}



bool USBManager_stop()
{
	cdc_device_acm_deinit();
	
	usbdc_reset();
	
	while (cdcdf_acm_is_enabled()) {
		// wait cdc acm to be disabled
	};
	
	return true;
}

/**
 * \brief Callback invoked when bulk OUT data received
 */
static bool rxCallback(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	return false;
}

/**
 * \brief Callback invoked when bulk IN data received
 */
static bool txCallback(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( _usbBufMutex, &xHigherPriorityTaskWoken);
	return false;
}

/**
 * \brief Callback invoked when Line State Change
 */
static bool lineStateChangeCallback(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR) {
		/* Start Rx */
	}

	/* No error. */
	return false;
}
void example()
{
	cdcd_acm_example();
}

void USBManager_test()
{
	USBManager_start();
	USBManager_print("Premier message\r\n");
	USBManager_stop();
	sleep_deep();
	USBManager_start();
	USBManager_print("Second message\r\n");
	USBManager_stop();
	sleep_deep();
	USBManager_start();
	vTaskDelay(2000);
	USBManager_print("Troisième message\r\n");
	vTaskDelay(2000);
	USBManager_stop();
	USARTManager_printUsbWait("Finish\r\n");
}