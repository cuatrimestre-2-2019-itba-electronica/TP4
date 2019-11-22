//
// Created by mlste on 9/1/2019.
//

#ifndef WAITING_ID_SUCESO_SOURCES_H
#define WAITING_ID_SUCESO_SOURCES_H


#include "sucesos.h"
#include "cqp.h"


typedef enum {
    SUCESO_SOURCE_GET_DELAY_TYPE,
    SUCESO_SOURCE_GET_NUM_TYPE,
    SUCESO_SOURCE_GET_TAP_TYPE,
    SUCESO_SOURCE_GET_CURSOR_MOVE_TYPE,
    SUCESO_SOURCE_GET_INC_DISP_INT_TYPE,
    SUCESO_SOURCE_TYPE_N
} SUCESO_SOURCE_TYPE;


union SUCESO_SOURCE;
typedef union SUCESO_SOURCE SUCESO_SOURCE;

SUCESO_SOURCE * suceso_source__create(SUCESO_SOURCE_TYPE type);  // equivalent to "new Point(x, y)"
void suceso_source__destroy(SUCESO_SOURCE* self);  // equivalent to "delete point"
void suceso_source_restart(SUCESO_SOURCE * self);
suceso_t get_suceso(SUCESO_SOURCE * self, cqp_t cqp);



#endif // WAITING_ID_SUCESO_SOURCES_H
