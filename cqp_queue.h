//
// Created by mlste on 9/5/2019.
//

#ifndef CQP_QUEUE_H
#define CQP_QUEUE_H

#include <stdbool.h>

#include "cqp.h"



struct CQP_QUEUE;
typedef struct CQP_QUEUE CQP_QUEUE;

CQP_QUEUE * cqp_queue__create();  // equivalent to "new Point(x, y)"
void cqp_queue__destroy( CQP_QUEUE* self);  // equivalent to "delete point"

void cqp_queue_update(CQP_QUEUE * self);
bool is_cqp_queue_empty(CQP_QUEUE * self);
cqp_t get_next_cqp(CQP_QUEUE * self);

//void delete_cqp_generator(CQP_QUEUE * self, SUCESO_SOURCE * source);
//void append_cqp_source(CQP_QUEUE * self, SUCESO_SOURCE * source);




#endif //CQP_QUEUE_H
