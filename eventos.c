//
// Created by mlste on 9/5/2019.
//
#include "eventos.h"
#include "stdlib.h"

evento_t create_evento(evento_type_t type) {
    evento_t ev;
    ev.type = type;

//Inicializo los campos en valor inicial o valor invalido
    switch (type) {
        case EVENTO_FAILED_NUM_INPUT_TYPE:
        case EVENTO_CORRECT_NUM_INPUT_TYPE:
            ev.correct_num_input.num_buff = NULL;
            ev.correct_num_input.num_buff_len = 0;
            break;
        case EVENTO_TIMEOUT_TYPE:
            break;
        case EVENTO_CANCEL_TYPE:
            break;
        default:
            ev.type = EVENTO_N;
            break;
    }
    return ev;
}
