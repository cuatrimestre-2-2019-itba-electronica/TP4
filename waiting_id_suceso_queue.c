//
// Created by mlste on 9/1/2019.
//

#include "waiting_id_suceso_queue.h"

#include <stdint.h>
#include <stdio.h>

#include "waiting_id_suceso_sources.h"
#include "cqp_queue.h"
#include "circular_buffer/utringbuffer.h"



#define SUCESO_SOURCES_COUNT_MAX 20
#define SUCESO_QUEUE_SIZE 20

//Para el ring buffer
UT_icd suceso_icd;


typedef struct SUCESO_QUEUE {
    UT_ringbuffer * suceso_queue;
    suceso_t * last_suceso;
    unsigned int suceso_queue_size;
    SUCESO_SOURCE * suceso_sources[SUCESO_SOURCES_COUNT_MAX];
    unsigned int suceso_sources_count;
    CQP_QUEUE * cqp_queue;
}SUCESO_QUEUE;

// Constructor (without allocation)
void waiting_id_suceso_queue__init(SUCESO_QUEUE* self)
{
    if(self == NULL)
    {
        return;
    }

    //creacion del buffer circular de sucesos (para la salida)
    suceso_icd.sz = sizeof(suceso_t);
    utringbuffer_new(self->suceso_queue, SUCESO_QUEUE_SIZE, &suceso_icd);
    self->last_suceso = utringbuffer_front(self->suceso_queue);

    self->suceso_sources_count = 0;

    self->suceso_queue_size = 0;

    SUCESO_SOURCE *  source = NULL;

    source = suceso_source__create(SUCESO_SOURCE_GET_NUM_TYPE);
    append_suceso_source(self, source);
    source = suceso_source__create(SUCESO_SOURCE_GET_DELAY_TYPE);
    append_suceso_source(self, source);
    source = suceso_source__create(SUCESO_SOURCE_GET_TAP_TYPE);
    append_suceso_source(self, source);
    source = suceso_source__create(SUCESO_SOURCE_GET_CURSOR_MOVE_TYPE);
    append_suceso_source(self, source);
    source = suceso_source__create(SUCESO_SOURCE_GET_INC_DISP_INT_TYPE);
    append_suceso_source(self, source);

    self->cqp_queue = cqp_queue__create();
}

// Allocation + initialization (equivalent to "new Point(x, y)")
SUCESO_QUEUE* suceso_queue__create() {
    SUCESO_QUEUE * result = (SUCESO_QUEUE*) malloc(sizeof(SUCESO_QUEUE));
    waiting_id_suceso_queue__init(result);
    return result;
}

// Destructor (without deallocation)
void waiting_id_suceso_queue__reset(SUCESO_QUEUE* self) {

    //destruyo el buffer de sucesos
    suceso_t suceso = SUC_N;
    if(self->suceso_queue != NULL)
    {
        utringbuffer_free(self->suceso_queue);
    }

    //destruyo las suceso sources
    for (int i = 0; i < self->suceso_sources_count; ++i) {
        suceso_source__destroy(self->suceso_sources[i]);
    }

    //destruyo la cola de cqp
    if(self->cqp_queue != NULL)
    {
        cqp_queue__destroy(self->cqp_queue);
    }
}

// Destructor + deallocation (equivalent to "delete point")
void suceso_queue__destroy(SUCESO_QUEUE *self) {
    if (self) {
        waiting_id_suceso_queue__reset(self);
        free(self);
    }
}

bool suceso_queue_push_back(SUCESO_QUEUE *self, suceso_t suceso){
    if(suceso != SUC_N)
    {
    	suceso_t s = suceso;
        utringbuffer_push_back(self->suceso_queue, &s);
        self->suceso_queue_size++;
        return true;
    }
    return false;
}

suceso_t suceso_queue_pop_front(SUCESO_QUEUE *self){
    if(self->suceso_queue_size)
    {
        self->last_suceso = utringbuffer_next(self->suceso_queue, self->last_suceso);
        self->suceso_queue_size--;
        return *(self->last_suceso);
    }
    return SUC_N;
}

void suceso_queue_update(SUCESO_QUEUE *self)
{
    suceso_t suceso;
    cqp_queue_update(self->cqp_queue);
    if(!is_cqp_queue_empty(self->cqp_queue))
    {
        cqp_t cqp = get_next_cqp(self->cqp_queue);
        for(int i = 0; i < self->suceso_sources_count; i++)
        {
            suceso = get_suceso(self->suceso_sources[i], cqp);
            if(suceso != SUC_N){
                suceso_queue_push_back(self, suceso);
            }
        }
    }
}

void append_suceso_source(SUCESO_QUEUE *self, SUCESO_SOURCE * source){
    self->suceso_sources[self->suceso_sources_count] = source;
    self->suceso_sources_count++;
}

void delete_suceso_source(SUCESO_QUEUE* self, SUCESO_SOURCE * source){
    bool found = false;
    //busco la source
    for(int i = 0; i < self->suceso_sources_count; i++)
    {
        if(!found && self->suceso_sources[i] == source)
        {
            found = true;
        }
        if(found)
        {
            if( i+1 < self->suceso_sources_count)
            {
                self->suceso_sources[i] = self->suceso_sources[i+1];
            }
            else
            {
                self->suceso_sources[i] = NULL;
            }
        }
    }
    if(found)
    {
        self->suceso_sources_count--;
    }
}

suceso_t get_next_suceso(SUCESO_QUEUE* self)
{
    if( is_suceso_queue_empty(self))
    {
        return SUC_N;
    }

    suceso_t suceso = suceso_queue_pop_front(self);

#ifdef SUCESOS_VERBOSE
    printf("*suceso: %s* \n", suc_debug_codes[suceso]);
#endif
    return suceso;
}

bool is_suceso_queue_empty(SUCESO_QUEUE * self){
    return !(self->suceso_queue_size);
}



