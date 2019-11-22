
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsm.h"
#include <stdio.h>
#include "evento_queue.h"
#include "estados.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


typedef struct FSM
{
    EVENTO_QUEUE * evento_q;
    estado_t * cur_state;
}FSM;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void FSM__init(FSM * self);
static void FSM__reset(FSM * self);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

FSM * FSM__create() {
    FSM * result = (FSM *) malloc(sizeof(FSM));
    FSM__init(result);
    return result;
}

void FSM__destroy(FSM *self) {
    if (self) {
        FSM__reset(self);
        free(self);
    }
}

void FSM_restart(FSM *self) {
    FSM__reset(self);
    FSM__init(self);
}

bool FSM_cycle(FSM * self)
{
    bool stop = false;
    if(self->evento_q == NULL){ return false;}
    evento_queue_update(self->evento_q);
    if(!is_evento_queue_empty(self->evento_q))
    {
        evento_t ev = get_next_event(self->evento_q);
        estado_t * next_state = estado_dispatch_evento(self->cur_state, ev);
        if(next_state != NULL) {
            estado__destroy(self->cur_state);
            evento_queue__destroy(self->evento_q);
            self->cur_state = next_state;
            self->evento_q = estado_create_evento_queue(self->cur_state);
            if(self->evento_q == NULL || estado_get_type(self->cur_state) == ESTADO_N){
                stop = true;
            }
        }
    }
    return stop;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void FSM__init(FSM * self){
    self->cur_state = estado__create(EST_WAITING_ID_TYPE, NULL);
    self->evento_q = estado_create_evento_queue(self->cur_state);
}
static void FSM__reset(FSM * self){
    estado__destroy(self->cur_state);
    evento_queue__destroy(self->evento_q);
}

/******************************************************************************/
