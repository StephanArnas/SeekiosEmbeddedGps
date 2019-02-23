#include <peripheralManager/USART_manager.h>

static E_SERIAL_ANSWER waitAnswer(unsigned int timeout, void (*validatationCallback)(void));
void timeoutTimeCallback(void);
static void checkATAnswerStatus(void);
static void initReception(void);
static void initSerialAnswerFlag(void);
static bool sendGsmModemSync(char* string);
static bool waitGsmTxComplete(void);

static SemaphoreHandle_t _debugUsartMutex;
static EventGroupHandle_t _gsmUsartMaskHandle;

static bool _USBOutputActivated = false;

void USARTManager_init(){
	_debugUsartMutex = xSemaphoreCreateMutex();
	_gsmUsartMaskHandle = xEventGroupCreate();
}

void USARTManager_activateUSBOutput()
{
	_USBOutputActivated = true;
}

void USARTManager_deactivateUSBOutput()
{
	_USBOutputActivated = false;
}

void USARTManager_gsmUsartMaskSetBitsFromISR(EventBits_t bitsToSet)
{
	BaseType_t xHigherPriorityTaskWoken, xResult;

	xHigherPriorityTaskWoken = pdFALSE;

	xResult = xEventGroupSetBitsFromISR(
	_gsmUsartMaskHandle,  /* The event group being updated. */
	bitsToSet, /* The bits being set. */
	&xHigherPriorityTaskWoken );

	if( xResult != pdFAIL )
	{
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

void USARTManager_gsmUsartMaskClearBits(EventBits_t bitsToClear)
{
	xEventGroupClearBits(_gsmUsartMaskHandle, bitsToClear);
}

static bool waitGsmTxComplete()
{
	EventBits_t uxBits = xEventGroupWaitBits(	_gsmUsartMaskHandle,	GSM_USART_BIT_TRANSFER_COMPLETE,	pdTRUE,	pdFALSE,  60000 );
	return (uxBits & GSM_USART_BIT_TRANSFER_COMPLETE)!=0;
}

/*
Réinitialise le rx_buffer et set les bons paramètres
*/
static void initReception(){
	memset(gsm_buf, '\0', GSM_BUF_SIZE);
	gsm_answer = 1; // notify the usart interrupt that we wait for a new answer
	initSerialAnswerFlag();
}

static void initSerialAnswerFlag()
{
	serialAnswerFlag = SERIAL_ANSWER_NONE;
}

/* Envoie une commande AT à un certain usart_device,
renvoie une USARTResponse avec le statut TIMEOUT, ERROR ou OK
Il est possible d'envoyer plusieurs strings d'un coup, ce qui permet de ne pas
avoir à les concaténer avant l'envoi. Exemple pour AT+HTTPPARA URL, on peut
envoyer AT+HTTPPARA et l'URL séparément */
E_SERIAL_ANSWER USARTManager_sendATCommand(unsigned int timeout, uint8_t nbStrings, ...){
	initReception();
	va_list stringList;
	va_start(stringList, nbStrings);
	for(uint8_t i=0;i<nbStrings;i++)
	{
		volatile char* string = va_arg(stringList, char*);
		if(!sendGsmModemSync(string)){
			va_end(stringList);
			return SERIAL_ANSWER_NONE;
		}
	}
	va_end(stringList);
	return waitAnswer(timeout, &checkATAnswerStatus);
}

/* Repeats an AT Command numberTries times until the function successes */
bool USARTManager_repeatATCommandUntilSuccess(uint8_t numberTries, char* ATcommand, unsigned int timeout){
	for(int i=0; i<numberTries; i++){
		if(USARTManager_sendATCommand(timeout, 1, ATcommand) == SERIAL_ANSWER_OK){
			return true;
		}
	}
	return false;
}


static bool sendGsmModemSync(char* string)
{
	USARTManager_gsmUsartMaskClearBits(GSM_USART_BIT_TRANSFER_COMPLETE);
	send_gsm_modem(string);
	return waitGsmTxComplete();
}

/* Lit un message sur le serial, et le valide avec la fonction de validation validate passée en paramètre
La fonction validate DOIT et le answerFlag à OK, ERROR, TIMEOUT ou NONE*/
E_SERIAL_ANSWER USARTManager_readAnswerAndValidate(unsigned int timeout, void (*validatationCallback)(void)){
	initSerialAnswerFlag();
	return waitAnswer(timeout, validatationCallback);
}

/* Attend une réponse pendant un timeout défini, et
la retourne, avec le status OK, ERROR ou TIMEOUT
validate: fonction de validation du message écouté
retours : 1 = OK | 2 = ERROR | 0 = RAS
The validate function MUST set the answerFlag to one of those 3 values
*/
static E_SERIAL_ANSWER waitAnswer(unsigned int timeout, void (*validatationCallback)(void)){
	uint32_t i = 0;
	while(serialAnswerFlag==SERIAL_ANSWER_NONE){
		validatationCallback();
		vTaskDelay(50);
		i++;
		if(timeout / 50 < i) timeoutTimeCallback();
	}
	USARTManager_printUsbWait(gsm_buf);
	return serialAnswerFlag;
}

void timeoutTimeCallback(void){
	serialAnswerFlag = SERIAL_ANSWER_TIMEOUT;
}

/* Checks the GSM_buf on a regular basis, waiting for the OK or ERROR string */
static void checkATAnswerStatus(){

	if(strstr(gsm_buf,"OK"))
	{
		serialAnswerFlag = SERIAL_ANSWER_OK;
	}
	else if (strstr(gsm_buf,"ERROR"))
	{
		serialAnswerFlag = SERIAL_ANSWER_ERROR;
	}
	else
	{
		return;
	}
}

/* Prints a string on the USART debug. Also prints on the USB port, if USB debugging is activated */
void USARTManager_printUsbWait(const char* string){
	#if (ACTIVATE_DEBUG_LOGS == 1)
	if((string == NULL) || (strlen(string) <= 0)) return;
	if(xSemaphoreTake( _debugUsartMutex, ( TickType_t ) 10000 ) == pdTRUE){
		send_debug(string);
		xSemaphoreGive(_debugUsartMutex);
	}
	#endif
	if(_USBOutputActivated)
	{
		USBManager_print(string);
	}
}

void USARTManager_printUsbWaitN(const char* text, uint16_t length)
{
	#if (ACTIVATE_DEBUG_LOGS == 1)

	if(xSemaphoreTake( _debugUsartMutex, ( TickType_t ) 10000 ) == pdTRUE){
		io_write(usart_debug_io, (const uint8_t *) text, length);
		xSemaphoreGive(_debugUsartMutex);
	}
	#endif
}

/* Encapsulation de la fonction usart_write_buffer_wait pour l'usb */
void USARTManager_printByte(const uint8_t byte){
	#if (ACTIVATE_DEBUG_LOGS == 1)
	if(xSemaphoreTake( _debugUsartMutex, ( TickType_t ) 10000 ) == pdTRUE){
		char buff[10];
		stringHelper_byteToHexString(byte, buff);
		send_debug(buff);
		xSemaphoreGive(_debugUsartMutex);
	}
	#endif
}

void USARTManager_printInt(const int integer){
	#if (ACTIVATE_DEBUG_LOGS == 1)
	if(xSemaphoreTake( _debugUsartMutex, ( TickType_t ) 10000 ) == pdTRUE){
		char buff[16];
		stringHelper_intToString(integer, (uint8_t*)buff);
		send_debug(buff);
		xSemaphoreGive(_debugUsartMutex);
	}
	#endif
}
