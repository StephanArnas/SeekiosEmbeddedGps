#include <peripheralManager/bma222_adapted.h>
static void stopFifoInterrupt(void);
static bool isSeekiosInMovementFromAccelerations(int8_t accelerations[3][MOTION_DETECTION_SAMPLES]);
static void buildFifo(int8_t accelerations[3][MOTION_DETECTION_SAMPLES]);
static void updateFifo(int8_t accelerations[3][MOTION_DETECTION_SAMPLES]);
static void printAccelerationArray(int8_t fifoAccelerations[][MOTION_DETECTION_SAMPLES]);
static SemaphoreHandle_t	_bmaSemaphore;

static uint8_t getAccX(void);
static uint8_t getAccY(void);
static uint8_t getAccZ(void);
static bool XnewData(void);
static bool YnewData(void);
static bool ZnewData(void);

void IMUManager_init(){
	_bmaSemaphore = xSemaphoreCreateMutex();
	bma222_set_sleep_duration(0b1101); // 100ms sleep interval
	bma222_reg_overwrite(0x10, 0b00001000); // set Bandwidth for acceleration data sampling 1000 = 64ms  1001 = 32ms 1010 = 16ms ...  1111 = 0,5ms
	bma222_reg_overwrite(0x12, 0x00); // Set the LPM1 (0x40) for LPM2
	stopFifoInterrupt();
	bma222_set_sleepmode(BMA_POWERMODE_SUSPEND); // suspend
	DELAY_MS(100);
}

void IMUManager_powerModeNormal()
{
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		bma222_set_sleepmode(BMA_POWERMODE_NORMAL); // low power
		DELAY_MS(100);
		xSemaphoreGive(_bmaSemaphore);
	}
	
}

void IMUManager_powerModeLowPower()
{
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		bma222_set_sleepmode(BMA_POWERMODE_LOW_POWER); // low power
		DELAY_MS(100);
		xSemaphoreGive(_bmaSemaphore);
	}
}

void IMUManager_suspend()
{
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		bma222_set_sleepmode(BMA_POWERMODE_SUSPEND); // suspend
		DELAY_MS(100);
		xSemaphoreGive(_bmaSemaphore);
	}
}

void IMUManager_startSlopeDetection()
{
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		bma222_reset_tap_interrupts();
		IMU_interrupt_config = 0;
		bma222_set_slope_motion_interrupt((uint8_t)SLOPE_DETECTION_SENSITIVITY);
		xSemaphoreGive(_bmaSemaphore);
	}
}

static void stopFifoInterrupt()
{
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		bma222_reg_overwrite(0x17, 0b00000000); // deactivate fifo full interrupt
		bma222_reg_overwrite(0x1A, 0b00000000);// unmap to int2 pin
		xSemaphoreGive(_bmaSemaphore);
	}
}

void IMUManager_printSleepDuration()
{
	uint8_t sleepDuration = 0;
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		volatile uint8_t sleepDuration = bma222_reg_read(0x11);
		UNUSED(sleepDuration);
		xSemaphoreGive(_bmaSemaphore);
	}
	char buff[5];
	stringHelper_intToString(sleepDuration, (uint8_t*)buff);
	USARTManager_printUsbWait("Sleep duration : ");
	USARTManager_printUsbWait(buff);
	USARTManager_printUsbWait("\r\n");
}

void IMUManager_stopSlopeDetection(){
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		bma222_reset_tap_interrupts();
		xSemaphoreGive(_bmaSemaphore);
	}
}

static uint8_t getAccX()
{
	uint8_t accX = 0;
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		accX = bma222_reg_read(0x3);
		xSemaphoreGive(_bmaSemaphore);
	}
	return accX;
}

static uint8_t getAccY()
{
	uint8_t accY = 0;
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		accY = bma222_reg_read(0x5);
		xSemaphoreGive(_bmaSemaphore);
	}
	return accY;
}

static uint8_t getAccZ()
{
	uint8_t accZ = 0;
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		accZ = bma222_reg_read(0x7);
		xSemaphoreGive(_bmaSemaphore);
	}
	return accZ;
}

static bool XnewData()
{
	uint8_t xNewData = 0;
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		xNewData = bma222_reg_read(0x2) & 0b00000001;
		xSemaphoreGive(_bmaSemaphore);
	}
	return xNewData;
}

static bool YnewData()
{
	uint8_t yNewData = 0;
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		yNewData = bma222_reg_read(0x4) & 0b00000001;
		xSemaphoreGive(_bmaSemaphore);
	}
	return yNewData;
}

static bool ZnewData()
{
	uint8_t zNewData = 0;
	if(xSemaphoreTake(_bmaSemaphore, 100)==pdPASS)
	{
		zNewData = bma222_reg_read(0x6) & 0b00000001;
		xSemaphoreGive(_bmaSemaphore);
	}
	return zNewData;
}


void task_detectSignificantMotion(void* param)
{
	UNUSED(param);

	maskUtilities_setRunningMaskBits(RUNNING_BIT_SGNIFICANT_MOVE_DETECTION);

	if(IMUManager_detectSignificantMove())
	{
		IMUManager_stopSlopeDetection();
		IMUManager_suspend();
		maskUtilities_clearInterruptMaskBits(INTERRUPT_BIT_MOTION_DETECTED);
		maskUtilities_setRequestMaskBits(REQUEST_BIT_SIGNIFICANT_MOTION_DETECTED);
	}
	
	volatile uint32_t wm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(wm);
	maskUtilities_clearRunningMaskBits(RUNNING_BIT_SGNIFICANT_MOVE_DETECTION);
	FreeRTOSOverlay_taskDelete(NULL);
}

static bool isSeekiosInMovementFromAccelerations(int8_t accelerations[3][MOTION_DETECTION_SAMPLES]){
	bool axisStabilities[3] = {false};

	for(int i = 0;i<3;i++)
	{
		uint8_t j=0;
		while(j<MOTION_DETECTION_SAMPLES && axisStabilities[i]==false)
		{
			int8_t val = accelerations[i][j];
			uint8_t occurences=0;
			for(uint8_t k=0;k<MOTION_DETECTION_SAMPLES;k++)
			{
				if(val<=accelerations[i][k]+1
				&& val>=accelerations[i][k]-1)
				//if(val==accelerations[i][k])
				{
					occurences++;
				}
				if(occurences>=MOTION_DETECTION_NB_SIMILAR_VALUES)
				{
					axisStabilities[i]=true;
					break;
				}
			}
			j++;
		}
	}

	//USARTManager_printUsbWait("Calculations done.");
	return !axisStabilities[0] || !axisStabilities[1] || !axisStabilities[2];
}

static void buildFifo(int8_t accelerations[3][MOTION_DETECTION_SAMPLES]){
	uint8_t xValuesCount = 0;
	uint8_t yValuesCount = 0;
	uint8_t zValuesCount = 0;

	while(xValuesCount < MOTION_DETECTION_SAMPLES
	|| yValuesCount < MOTION_DETECTION_SAMPLES
	|| zValuesCount < MOTION_DETECTION_SAMPLES)
	{
		if(XnewData() && xValuesCount < MOTION_DETECTION_SAMPLES)
		{
			accelerations[0][xValuesCount++] = getAccX();
		}
		if(YnewData() && yValuesCount < MOTION_DETECTION_SAMPLES)
		{
			accelerations[1][yValuesCount++] = getAccY();
		}
		if(ZnewData() && zValuesCount < MOTION_DETECTION_SAMPLES)
		{
			accelerations[2][zValuesCount++] = getAccZ();
		}
		vTaskDelay(MOTION_DETECTION_REFRESH_RATE_MS);
	}
}

static void updateFifo(int8_t accelerations[3][MOTION_DETECTION_SAMPLES]){

	if(XnewData())
	{
		for(uint8_t j=0;j<MOTION_DETECTION_SAMPLES-1;j++){
			accelerations[0][j] = accelerations[0][j+1];
		}
		accelerations[0][MOTION_DETECTION_SAMPLES-1] = getAccX();
	}

	if(YnewData())
	{
		for(uint8_t j=0;j<MOTION_DETECTION_SAMPLES-1;j++){
			accelerations[1][j] = accelerations[1][j+1];
		}
		accelerations[1][MOTION_DETECTION_SAMPLES-1] = getAccY();
	}

	if(ZnewData())
	{
		for(uint8_t j=0;j<MOTION_DETECTION_SAMPLES-1;j++){
			accelerations[2][j] = accelerations[2][j+1];
		}
		accelerations[2][MOTION_DETECTION_SAMPLES-1] = getAccZ();
	}
}

static void printAccelerationArray(int8_t fifoAccelerations[][MOTION_DETECTION_SAMPLES])
{
	USARTManager_printUsbWait("Accelerations : \r\n");
	uint8_t i=0;
	for(i=0;i<MOTION_DETECTION_SAMPLES; i++)
	{
		char buff[5];
		USARTManager_printUsbWait("\tx:");
		stringHelper_intToString(fifoAccelerations[0][i],(unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(" - y:");
		stringHelper_intToString(fifoAccelerations[1][i],(unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait(" - z:");
		stringHelper_intToString(fifoAccelerations[2][i],(unsigned char*)buff);
		USARTManager_printUsbWait(buff);
		USARTManager_printUsbWait("\r\n");
	}
}

bool IMUManager_detectSignificantMove()
{
	USARTManager_printUsbWait("Starting SIGNIFICANT MOVE detection.\r\n");

	IMUManager_powerModeNormal(); // We start the normal mode, because in low power it is only one sample per 100ms

	uint32_t i=0;
	bool isTimeout = false;
	bool significantMoveDetected = false;
	int8_t fifoAccelerations[3][MOTION_DETECTION_SAMPLES];

	buildFifo(fifoAccelerations);

	while(!isTimeout && !significantMoveDetected){
		if(isSeekiosInMovementFromAccelerations(fifoAccelerations)){
			printAccelerationArray(fifoAccelerations);
			significantMoveDetected = true;
		}
		updateFifo(fifoAccelerations);
//		#if (DEBUG_MODE == 1)
// 		char buff[5];
//  		USARTManager_printUsbWait("\tx:");
//  		stringHelper_intToString(fifoAccelerations[0][MOTION_DETECTION_SAMPLES-1],(unsigned char*)buff);
//  		USARTManager_printUsbWait(buff);
//  		USARTManager_printUsbWait(" - y:");
//  		stringHelper_intToString(fifoAccelerations[1][MOTION_DETECTION_SAMPLES-1],(unsigned char*)buff);
//  		USARTManager_printUsbWait(buff);
//  		USARTManager_printUsbWait(" - z:");
//  		stringHelper_intToString(fifoAccelerations[2][MOTION_DETECTION_SAMPLES-1],(unsigned char*)buff);
//  		USARTManager_printUsbWait(buff);
//  		USARTManager_printUsbWait("\r\n");
// 		#endif
		i++;
		isTimeout = i*MOTION_DETECTION_REFRESH_RATE_MS > MOTION_DETECTION_TIMEOUT;
		vTaskDelay(MOTION_DETECTION_REFRESH_RATE_MS);
	}
	return significantMoveDetected;
}
