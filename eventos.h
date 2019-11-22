//
// Created by mlste on 9/5/2019.
//

#ifndef EVENTOS_H
#define EVENTOS_H

#include <stdint.h>
#include "config.h"

typedef enum {
    EVENTO_TIMEOUT_TYPE,
    EVENTO_CANCEL_TYPE,
    EVENTO_FAILED_NUM_INPUT_TYPE,
    EVENTO_CORRECT_NUM_INPUT_TYPE,
    EVENTO_EDIT_PIN,
    EVENTO_N} evento_type_t;

typedef struct EVENTO_TIME_OUT{
    evento_type_t type;
}EVENTO_TIME_OUT;

typedef struct EVENTO_CANCEL{
    evento_type_t type;
}EVENTO_CANCEL;

typedef struct EVENTO_FAILED_NUM_INPUT
{
    evento_type_t type;
    uint8_t * num_buff;
    uint8_t num_buff_len;
}EVENTO_FAILED_NUM_INPUT;

typedef struct EVENTO_FAILED_NUM_INPUT EVENTO_CORRECT_NUM_INPUT;

union evento_t
{
    evento_type_t type;    //para saber el tipo rapido
    EVENTO_TIME_OUT time_out;
    EVENTO_CANCEL cancel;
    EVENTO_CORRECT_NUM_INPUT correct_num_input;
    EVENTO_FAILED_NUM_INPUT failed_num_input;
};

typedef union evento_t evento_t;

evento_t create_evento(evento_type_t type);


#endif //EVENTOS_H
