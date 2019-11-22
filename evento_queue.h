//
// Created by mlste on 9/5/2019.
//

#ifndef EVENTO_QUEUE_H
#define EVENTO_QUEUE_H

#include <stdbool.h>

#include "event_sources.h"

// Header
struct EVENTO_QUEUE;  // forward declared for encapsulation
typedef struct EVENTO_QUEUE EVENTO_QUEUE;

EVENTO_QUEUE * evento_queue__create();  // equivalent to "new Point(x, y)"
void evento_queue__destroy(EVENTO_QUEUE* self);  // equivalent to "delete point"

void evento_queue_update(EVENTO_QUEUE * self);
bool is_evento_queue_empty(EVENTO_QUEUE * self);
evento_t get_next_event(EVENTO_QUEUE * self);

void append_evento_source(EVENTO_QUEUE * self, EVENTO_SOURCE * source);
void delete_evento_generator(EVENTO_QUEUE * self, EVENTO_SOURCE * source);


#endif //EVENTO_QUEUE_H
