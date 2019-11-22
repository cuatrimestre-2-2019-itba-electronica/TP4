//
// Created by mlste on 9/1/2019.
//

#ifndef WAITING_ID_SUCESO_QUEUE_H
#define WAITING_ID_SUCESO_QUEUE_H

#include "stdbool.h"
#include "stdlib.h"
#include "waiting_id_suceso_sources.h"

// Header
struct SUCESO_QUEUE;  // forward declared for encapsulation
typedef struct SUCESO_QUEUE SUCESO_QUEUE;

// Creador y destructor
SUCESO_QUEUE * suceso_queue__create();  // equivalent to "new Point(x, y)"
void suceso_queue__destroy(SUCESO_QUEUE *self);  // equivalent to "delete point"

// Busca la proxima cqp. Si existe, la manda a las fuentes de sucesos,
// y encola cualquier suceso que se genere
void suceso_queue_update(SUCESO_QUEUE *self);

// Indica si hay sucesos en la cola o no
// Importante: si no se llama a suceso_queue_update, la cola nunca
bool is_suceso_queue_empty(SUCESO_QUEUE * self);

// Si hay sucesos encolados, devuelve el primero.
// Importante: tiene que llamarse luego de suceso_queue_update para que
// funcione correctamente
suceso_t get_next_suceso(SUCESO_QUEUE * self);

void append_suceso_source(SUCESO_QUEUE * self, SUCESO_SOURCE * source);
void delete_suceso_source(SUCESO_QUEUE * self, SUCESO_SOURCE * source);




#endif //CODIGO_BORRADOR_