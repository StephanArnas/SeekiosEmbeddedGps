#include <seekiosCore/start.h>

static void startSeekiosManager(void);
static void startTasks(void);

/*
In this file are the functions that are necessary to the core of SeekiOS : configuration, start-up
etc.
*/

int start_start(void)
{
	init_init(); // initialisation du seekios
	startTasks();
	while(1){}
}

static void startTasks(){
	#if (POWER_TESTS_ACTIVATED==1)
	taskManagementUtilities_startPowerTestTask();
	#else
	startSeekiosManager();
	#endif
}

static void startSeekiosManager(){
	if(xTaskCreate(
	task_seekiosManager,
	"Seekios manager task",
	STACK_SIZE_SEEKIOS_MANAGER_TASK,
	NULL,
	TASK_DEFAULT_PRIORITY,
	NULL
	) == pdPASS){
		vTaskStartScheduler();
	}
	else
	{
		send_debug("Failure starting seekios manager.\r\n");
	}
}



void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ){
	// se déclenche en cas de stack overflow sur une task ?
	send_debug("Tasks stack overflow on task ");
	send_debug(pcTaskName);
	UNUSED(xTask);
	UNUSED(pcTaskName);
	NVIC_SystemReset();
}
