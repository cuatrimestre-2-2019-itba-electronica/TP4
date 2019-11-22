#include "cqp_queue.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "encoder.h"
#include "config.h"
#include "circular_buffer/utringbuffer.h"
#include "magnetic_band.h"
#include "timer.h"

#define CQP_QUEUE_SIZE 30
#define DELAY_US 1000000

time_t timer;

static unsigned int front = 0;
static unsigned int cqp_queue_size = 0;

static unsigned int push_CW = 0;
static unsigned int push_CCW = 0;
static unsigned int push_PRESS = 0;
static unsigned int push_RELEASE = 0;

static UT_icd cqp_icd = {sizeof(cqp_t), NULL, NULL, NULL};


typedef struct CQP_QUEUE {
    UT_ringbuffer * CQP_queue;
    cqp_t * last_cqp;
    unsigned int cqp_queue_size;
}CQP_QUEUE;

CQP_QUEUE q;


static cqp_t cqp_queue_pop_front(CQP_QUEUE *self);
static bool cqp_queue_push_back(CQP_QUEUE *self, cqp_t cqp);

static void cqp_queue_input_CW();
static void cqp_queue_input_CCW();
static void cqp_queue_input_PRESS();
static void cqp_queue_input_RELEASE();



bool is_cqp_queue_empty(CQP_QUEUE *self) {
    return !self->cqp_queue_size;
}

cqp_t get_next_cqp(CQP_QUEUE *self) {

    if(is_cqp_queue_empty(self))
    {
        return CQP_N;
    }

    cqp_t cqp = cqp_queue_pop_front(self);

    return cqp;
}

void cqp_queue__init(CQP_QUEUE * self)
{
	if(self == NULL){ return; }

	timerInit();

	//Para el ring buffer
	//creacion del buffer circular de sucesos (para la salida)
	cqp_icd.sz = sizeof(cqp_t);
	utringbuffer_new(self->CQP_queue, CQP_QUEUE_SIZE, &cqp_icd);
	self->last_cqp = utringbuffer_front(self->CQP_queue);
	self->cqp_queue_size = 0;


	encoder_set_callback_CW(cqp_queue_input_CW);
	encoder_set_callback_CCW(cqp_queue_input_CCW);
	encoder_set_callback_SW_PRESS(cqp_queue_input_PRESS);
	encoder_set_callback_SW_RELEASE(cqp_queue_input_RELEASE);

}

CQP_QUEUE *cqp_queue__create() {
    cqp_queue__init(&q);
    return &q;
}

void cqp_queue__destroy(CQP_QUEUE *self) {

}

void cqp_queue_update(CQP_QUEUE *self) {
    if(is_loaded()){
    	uint8_t data = get_data();
    	if(data == UINT8_MAX){
    		cqp_queue_push_back(&q, PASA_SWIPE_END);
    	} else {
    		cqp_queue_push_back(&q, (cqp_t) data);
    	}
    	timer =  timerStart(TIMER_US2TICKS(DELAY_US));
    }
    if(push_CW){
		cqp_queue_push_back(&q, PASA_RIGHT);
		push_CW = 0;
    }
    if(push_CCW){
		cqp_queue_push_back(&q, PASA_LEFT);
    	push_CCW = 0;
    }
    if(push_PRESS){
		cqp_queue_push_back(&q, PASA_PRESS);
    	push_PRESS = 0;
    }
    if(push_RELEASE){
		cqp_queue_push_back(&q, PASA_RELEASE);
    	push_RELEASE = 0;
    }
    if(timerExpired(timer)){
    	cqp_queue_push_back(self, PASA_DELAY);
    	timer = timerStart(TIMER_US2TICKS(DELAY_US));
    }
}


static bool cqp_queue_push_back(CQP_QUEUE *self, cqp_t cqp ){
    if(cqp != CQP_N)
    {
        utringbuffer_push_back(self->CQP_queue, &cqp);
        self->cqp_queue_size++;
        return true;
    }
    return false;
}

cqp_t cqp_queue_pop_front(CQP_QUEUE *self){
    if(self->cqp_queue_size)
    {
        self->last_cqp = utringbuffer_next(self->CQP_queue, self->last_cqp);
        self->cqp_queue_size--;
        return *(self->last_cqp);
    }
    return CQP_N;
}

static void cqp_queue_input_CW(){
	push_CW = 1;
	timer =  timerStart(TIMER_US2TICKS(DELAY_US));
}

static void cqp_queue_input_CCW(){
	push_CCW = 1;
	timer =  timerStart(TIMER_US2TICKS(DELAY_US));
}

static void cqp_queue_input_PRESS(){
	push_PRESS = 1;
	timer =  timerStart(TIMER_US2TICKS(DELAY_US));
}

static void cqp_queue_input_RELEASE(){
	push_RELEASE = 1;
	timer =  timerStart(TIMER_US2TICKS(DELAY_US));
}


