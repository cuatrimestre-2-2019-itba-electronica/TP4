//
// Created by mlste on 9/5/2019.
//

#include "evento_queue.h"

#include <stdio.h>

#include "event_sources.h"
#include "waiting_id_suceso_queue.h"
#include "circular_buffer/utringbuffer.h"

#define EVENTO_SOURCES_COUNT_MAX 20
#define EVENTO_QUEUE_SIZE 20

//#define EVENTO_VERBOSE

#ifdef EVENTO_VERBOSE

const char * ev_debug_codes[] = \
{
    "EVENTO_TIMEOUT_TYPE",
    "EVENTO_CANCEL_TYPE",
    "EVENTO_FAILED_NUM_INPUT_TYPE",
    "EVENTO_CORRECT_NUM_INPUT_TYPE",
    "EVENTO_EDIT_PIN",
    "EVENTO_N"
};
#endif


//Para el ring buffer
UT_icd evento_icd = {sizeof(evento_t), NULL, NULL, NULL};

bool validate_ID(uint8_t * ID_buffer, uint8_t ID_buffer_len);
bool show_ID(uint8_t * ID_buffer, uint8_t ID_buffer_len);

typedef struct EVENTO_QUEUE{
    UT_ringbuffer * evento_queue;
    evento_t * last_evento;
    unsigned int evento_queue_size;
    EVENTO_SOURCE * evento_sources[EVENTO_SOURCES_COUNT_MAX];
    unsigned int evento_sources_count;
    SUCESO_QUEUE * suceso_queue;
} EVENTO_QUEUE;

//Constructor (sin allocation)
void evento_queue__init(EVENTO_QUEUE * self)
{
    if(self == NULL)
    {
        return;
    }

    //creacion del buffer circular de sucesos (para la salida)
    evento_icd.sz = sizeof(evento_t);
    utringbuffer_new(self->evento_queue, EVENTO_QUEUE_SIZE, &evento_icd);
    self->last_evento = utringbuffer_front(self->evento_queue);

    self->evento_sources_count = 0;

    self->evento_queue_size = 0;

    self->suceso_queue = suceso_queue__create();
}

// Allocation + initialization (equivalent to "new Point(x, y)")
EVENTO_QUEUE * evento_queue__create() {
    EVENTO_QUEUE * result = (EVENTO_QUEUE *) malloc(sizeof(EVENTO_QUEUE));
    evento_queue__init(result);
    return result;
}

// Destructor (without deallocation)
void evento_queue__reset(EVENTO_QUEUE * self) {

    //destruyo el buffer
    evento_t evento = create_evento(EVENTO_N);
    if(self->evento_queue != NULL)
    {
        utringbuffer_free(self->evento_queue);
    }

    //destruyo las evento sources
    for (int i = 0; i < self->evento_sources_count; ++i) {
        evento_source__destroy(self->evento_sources[i]);
    }

    if(self->suceso_queue != NULL)
    {
        suceso_queue__destroy(self->suceso_queue);
    }
}

void evento_queue__destroy(EVENTO_QUEUE *self) {
    if (self) {
        evento_queue__reset(self);
        free(self);
    }
}

bool evento_queue_push_back(EVENTO_QUEUE *self, evento_t evento){
    if(evento.type != EVENTO_N)
    {
        utringbuffer_push_back(self->evento_queue, &evento);
        self->evento_queue_size++;
        return true;
    }
    return false;
}

evento_t evento_queue_pop_front(EVENTO_QUEUE *self){
    if(self->evento_queue_size)
    {
        self->last_evento = utringbuffer_next(self->evento_queue, self->last_evento);
        self->evento_queue_size--;
        return *(self->last_evento);
    }
    evento_t ev;
    ev.type = EVENTO_N;
    return ev;
}

void evento_queue_update(EVENTO_QUEUE *self)
{
    evento_t evento;
    suceso_queue_update(self->suceso_queue);
    if(!is_suceso_queue_empty(self->suceso_queue))
    {
        suceso_t suceso = get_next_suceso(self->suceso_queue);
        for(int i = 0; i < self->evento_sources_count; i++)
        {
            evento = get_evento(self->evento_sources[i], suceso);
            if(evento.type != EVENTO_N){
                evento_queue_push_back(self, evento);
            }
        }
    }
}

void append_evento_source(EVENTO_QUEUE *self, EVENTO_SOURCE *source)
{
    self->evento_sources[self->evento_sources_count] = source;
    self->evento_sources_count++;
}

void delete_evento_generator(EVENTO_QUEUE *self, EVENTO_SOURCE *source) {
    bool found = false;
    //busco la source
    for(int i = 0; i < self->evento_sources_count; i++)
    {
        if(!found && self->evento_sources[i] == source)
        {
            found = true;
        }
        if(found)
        {
            if( i+1 < self->evento_sources_count)
            {
                self->evento_sources[i] = self->evento_sources[i+1];
            }
            else
            {
                self->evento_sources[i] = NULL;
            }
        }
    }
    if(found)
    {
        self->evento_sources_count--;
    }
}

evento_t get_next_event(EVENTO_QUEUE *self) {
    evento_t ev;
    if( is_evento_queue_empty(self)){
        ev.type = EVENTO_N;
        return ev;
    }

    ev = evento_queue_pop_front(self);
#ifdef EVENTO_VERBOSE
    printf("**EVENTO: %s** \n", ev_debug_codes[ev.type]);
#endif

    return ev;
}

bool is_evento_queue_empty(EVENTO_QUEUE *self) {
    return !(self->evento_queue_size);
}

