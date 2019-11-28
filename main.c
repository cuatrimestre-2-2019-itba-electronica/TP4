#include "hardware.h"
#include  <os.h>
#include "board.h"
#include"gpio.h"


//defines del  tp 1 ***************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
//#include <gpio.h>

#include"gateway_handler.h"

#include "fsm.h"

#include "database.h"

#include"uart.h"


//#include "board.h"
//#include "gpio.h"
#include "8DigitsDisplay.h"
#include "3LedDsip.h"
#include "SysTick.h"
#include "encoder.h"
#include "magnetic_band.h"

//fin defines del  tp 1 ***************************************************************************











/* LEDs */
#define LED_R_PORT            PORTB
#define LED_R_GPIO            GPIOB
#define LED_G_PORT            PORTE
#define LED_G_GPIO            GPIOE
#define LED_B_PORT            PORTB
#define LED_B_GPIO            GPIOB
#define LED_R_PIN             22
#define LED_G_PIN             26
#define LED_B_PIN             21
#define LED_B_ON()           (LED_B_GPIO->PCOR |= (1 << LED_B_PIN))
#define LED_B_OFF()          (LED_B_GPIO->PSOR |= (1 << LED_B_PIN))
#define LED_B_TOGGLE()       (LED_B_GPIO->PTOR |= (1 << LED_B_PIN))
#define LED_G_ON()           (LED_G_GPIO->PCOR |= (1 << LED_G_PIN))
#define LED_G_OFF()          (LED_G_GPIO->PSOR |= (1 << LED_G_PIN))
#define LED_G_TOGGLE()       (LED_G_GPIO->PTOR |= (1 << LED_G_PIN))
#define LED_R_ON()           (LED_R_GPIO->PCOR |= (1 << LED_R_PIN))
#define LED_R_OFF()          (LED_R_GPIO->PSOR |= (1 << LED_R_PIN))
#define LED_R_TOGGLE()       (LED_R_GPIO->PTOR |= (1 << LED_R_PIN))

/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Task 2 */
#define TASK2_STK_SIZE			3000u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];



/* Task 3 */
#define TASK3_STK_SIZE			256u
#define TASK3_STK_SIZE_LIMIT	(TASK3_STK_SIZE / 10u)
#define TASK3_PRIO              4u
static OS_TCB Task3TCB;
static CPU_STK Task3Stk[TASK3_STK_SIZE];

//cola de eventos
OS_Q colaEv;
CPU_INT08U buffEv[100];


extern OS_MUTEX	mutexGentePorPiso;



/* Example semaphore */
static OS_SEM semTest;


void App_Init (void);
void App_Run (void);

extern int16_t gentePiso[3];
extern OS_MUTEX mutexGentePorPiso;
extern CPU_TS counterTS;

static void Task3(void *p_arg){
	OS_ERR  err;
	OS_ERR os_err;
	uint8_t dataRecived[6];
	uint8_t temp;
	gateway_sendData(0, gentePiso, 3);//envio los datos a la nube
	while(1){


		for(int i=0;i<5;i++){
			uartReadMsg(0,&temp,1);
			dataRecived[i]=temp;
		}
		//semaforo uart -> llego el ack
		OSTimeDlyHMSM(0u, 0u, 30u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);//espero los 15 segs minimos
		OSMutexPend((OS_MUTEX *)&mutexGentePorPiso,(OS_TICK   )0,(OS_OPT    )OS_OPT_PEND_BLOCKING,(CPU_TS   *)&counterTS,(OS_ERR   *)&err);
		gateway_sendData(0, gentePiso, 3);//envio los datos a la nube
		OSMutexPost((OS_MUTEX *)&mutexGentePorPiso, (OS_OPT    )OS_OPT_POST_NONE,(OS_ERR   *)&err);
	}
}



static void Task2(void *p_arg) {
//    (void)p_arg;
    OS_ERR os_err;
//    OS_ERR oos_err;
//    OS_ERR err;
//    OSQCreate(&colaEv,"colores",50,&oos_err);
//    while (1) {
//      //  OSSemPost(&semTest, OS_OPT_POST_1, &os_err);
//    	//OSQPost(&colaEv,&buffEv[0], sizeof(buffEv),OS_OPT_POST_FIFO,&err);
//        OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &os_err);
//        buffEv[0]++;
//        buffEv[0]%=3;
//       // LED_R_TOGGLE();
//    }
	while(1){
		App_Run();
		OSTimeDlyHMSM(0u, 0u, 0u, 250u, OS_OPT_TIME_HMSM_STRICT, &os_err);
	}

}


static void TaskStart(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    /* Initialize the uC/CPU Services. */
    CPU_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    /* (optional) Compute CPU capacity with no task running */
    OSStatTaskCPUUsageInit(&os_err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    /* Create semaphore */
    OSSemCreate(&semTest, "Sem Test", 0u, &os_err);

    /* Create Task2 */
    OSTaskCreate(&Task2TCB, 			//tcb
                 "Task 2",				//name
                  Task2,				//func
                  0u,					//arg
                  TASK2_PRIO,			//prio
                 &Task2Stk[0u],			//stack
                  TASK2_STK_SIZE_LIMIT,	//stack limit
                  TASK2_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);


    /* Create Task3 */
       OSTaskCreate(&Task3TCB, 				//tcb
                    "Task 3",				//name
                     Task3,					//func
                     0u,					//arg
                     TASK3_PRIO,			//prio
                    &Task3Stk[0u],			//stack
                     TASK3_STK_SIZE_LIMIT,	//stack limit
                     TASK3_STK_SIZE,		//stack size
                     0u,
                     0u,
                     0u,
                    (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    &os_err);

    while (1) {
    	OSTimeDlyHMSM(0u, 0u, 0u, 999u, OS_OPT_TIME_HMSM_STRICT, &os_err);
       // LED_G_TOGGLE();
    }
}

int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();

    App_Init(); /* Program-specific setup */



    OSInit(&err);
 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	 /* Enable task round robin. */
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);

    OSTaskCreate(&TaskStartTCB,
                 "App Task Start",
                  TaskStart,
                  0u,
                  TASKSTART_PRIO,
                 &TaskStartStk[0u],
                 (TASKSTART_STK_SIZE / 10u),
                  TASKSTART_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);

    OSStart(&err);

	/* Should Never Get Here */
    while (1) {

    }
}

uart_cfg_t uart_config;


void App_Init (void)
{
	uart_config.baudrate = 1200;
	uart_config.parity = odd_parity;
	uart_config.stopBit = 0;

	SysTick_Init();
	//_7SegDisp_display_init();
	_8DigitDisplay_init();
	_3LedDisp_dispInit();
	encoder_init();
	init_mag_card();
	database_populate();

	uartInit(0, uart_config);

}
void App_Run (void)
{
	FSM * machine  = FSM__create();
	while(!FSM_cycle(machine));
}
