//
// Created by mlste on 9/8/2019.
//

#ifndef ESTADOS_H
#define ESTADOS_H

#include "eventos.h"
#include "evento_queue.h"

union estado_t;
typedef union estado_t estado_t;

typedef enum estado_type_t {EST_WAITING_ID_TYPE, EST_WAITING_PIN_TYPE, EST_GOT_ACCESS_TYPE, EST_EDIT_PIN_TYPE, ESTADO_N} estado_type_t;

estado_t * estado__create(estado_type_t type, void * params);
void estado__destroy(estado_t * self);
void estado_restart(estado_t * self, void * params);
estado_t * estado_dispatch_evento(estado_t *self, evento_t evento);
EVENTO_QUEUE * estado_create_evento_queue(estado_t * self);
estado_type_t estado_get_type(estado_t * self);

#endif //ESTADOS_H
