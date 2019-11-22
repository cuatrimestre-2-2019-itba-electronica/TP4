//
// Created by mlste on 9/4/2019.
//

#include <stdint-gcc.h>
#include <stdbool.h>
#include <stdlib.h>
#include "event_sources.h"

#include "config.h"
#include "cqp.h"

#define ID_MAX_NUM 9

union EVENTO_SOURCE_DATA;
typedef union EVENTO_SOURCE_DATA EVENTO_SOURCE_DATA;

typedef evento_t(*get_evento_fun_p)(EVENTO_SOURCE_DATA * data, suceso_t suceso);


bool scroll_up_ID_num(uint8_t * num);
bool scroll_down_ID_num(uint8_t * num);

/********************************************************
 * ******************************************************
 * DATOS MIEMBROS
 * ******************************************************
 * ******************************************************/


/* Posibles eventos que devuelve:
    - EVENTO_FAILED_NUM_INPUT_TYPE
    - EVENTO_CORRECT_NUM_INPUT_TYPE */
/****************
 *   GET_NUM_BUFF
 * *************/

typedef struct EVENTO_SOURCE_GET_NUM_BUFF_DATA
{
//    uint8_t num_buff[GET_ID_MAX_LENGTH];
    uint8_t * num_buff;
    uint8_t cursor;
    bool changing_num;  //true si se esta cambiando el valor de un digito y todavia no se confirmo con un tap
    bool swiping;  		//true si se esta cambiando el valor de un digito y todavia no se confirmo con un tap

    bool card_enabled;

    uint8_t max_length;
    uint8_t min_length;
    uint8_t cur_length; //longitud actual del arreglo

    get_num_buff_validate_cb is_num_buff_correct;
    get_num_buff_show_cb show_num_buff;

} EVENTO_SOURCE_GET_NUM_BUFF_DATA;

evento_t
get_num_buff_fun(EVENTO_SOURCE_GET_NUM_BUFF_DATA * data, suceso_t suceso)
{
    evento_t ev;
    ev.type = EVENTO_N;

    bool user_is_done = false;


    switch(suceso)
    {
        // ** si es un numero ** //
        case SUC_NUM_0: case SUC_NUM_1: case SUC_NUM_2:
        case SUC_NUM_3: case SUC_NUM_4: case SUC_NUM_5:
        case SUC_NUM_6: case SUC_NUM_7: case SUC_NUM_8:
        case SUC_NUM_9:
        	if(!data->card_enabled) { break; }
        	if( !data->swiping){
        		data->swiping = true;
        		data->changing_num = false;
        		data->cur_length = 0;
        		data->cursor = 0;
        		for(int i = 0; i < data->max_length; i++){
        			data->num_buff[i];
        		}
        	}
            data->num_buff[data->cursor] = (uint8_t)suceso;
            if(data->cur_length < data->max_length){
            	data->cur_length++;
            	data->cursor++;
            }
            break;

        // ** si es la modificacion de un numero por scroll ** //
            // decremento/incremento sin overflow/underflow.
            // Si estoy en la ultima posicion y borre el numero (es decir, pase
            // de 0 a espacio o de ID_MAX_NUM a espacio), indico que el ID ahora
            // es mas corto
        case SUC_SWIPE_END:
        	if(!data->card_enabled) { break; }
        	user_is_done = true;
        	break;
        case SUC_SCROLL_UP:
            if(scroll_up_ID_num(&(data->num_buff[data->cursor]))
               && (data->cursor + 1 == data->cur_length)){
                data->cur_length--;
            }
            data->changing_num = true;
            if(data->cursor >= data->cur_length) { data->cur_length = data->cursor + 1; }
            break;
        case SUC_SCROLL_DOWN:
            if( scroll_down_ID_num(&(data->num_buff[data->cursor]))
                && (data->cursor + 1 == data->cur_length)){
                data->cur_length--;
            }
            data->changing_num = true;
            if(data->cursor >= data->cur_length) { data->cur_length = data->cursor + 1; }
            break;
        // ** si son gestos de control ** //
        case SUC_TAP_DETECTED:
            //SI NO ESTABA MODIFICANDO UN DIGITO:
            if(!(data->changing_num)) {
                //Hay evento solo si hice tap en el ultimo digito
                // ingresado y ademas la longitud total esta entre
                // los limites permitidos por min_length y
                // max_length
                if(data->cursor == data->cur_length - 1
                && data->cur_length >= data->min_length
                && data->cur_length <= data->max_length){
                    user_is_done = true;
                }
                break;
            }
            //SI ESTABA MODIFICANDO UN DIGITO:
            data->changing_num = false;

            //si puse un espacio en la ultima posicion escrita:
            if(data->cursor == data->cur_length - 1
            && data->num_buff[data->cursor] > ID_MAX_NUM){
            	if(data->cursor) { data->cursor--; }
            	data->cur_length--;
            	break;
            }

            //si estoy en la ultima posicion permitida
            if(data->cur_length == data->max_length) {
                user_is_done = true;
            } else {
                // si no es la ultima posicion permitida
                // y estaba en el medio de una modificacion
                // cuando hago tap finalizo la modificacion
                // del digito y el cursor se adelanta
                (data->cursor)++;
                if(data->cursor == data->cur_length){
                    data->cur_length++;
                    data->num_buff[data->cursor] = 0;
                }
            }
            break;
        case SUC_MOVE_CURSOR_LEFT:
            //decremento sin underflow
            if(data->cursor) { (data->cursor)--; }
            data->changing_num = false;
            break;
        case SUC_MOVE_CURSOR_RIGHT:
            //incremento sin overflow
            if(data->cursor < data->cur_length){ data->cursor++; }
            data->changing_num = false;
            break;

        default:
            break;
    }

    if(user_is_done)
    {
        //La event source cede ownership de num_buff!!
        //Queda a cargo de quien reciba el evento
        if(data->is_num_buff_correct(data->num_buff,data->cur_length))
        {
            //Si el numero es valido
            if((ev.correct_num_input.num_buff = malloc(data->cur_length)) == NULL) {
                ev.type = EVENTO_N;
            } else {
                ev.type = EVENTO_CORRECT_NUM_INPUT_TYPE;
                ev.correct_num_input.num_buff_len = data->cur_length;
                for (int i = 0; i < data->max_length; ++i) {
                    ev.correct_num_input.num_buff[i] = data->num_buff[i];
                }
            }
        } else {
            //Si el numero no es valido
            if((ev.failed_num_input.num_buff = malloc(data->cur_length)) == NULL) {
                ev.type = EVENTO_N;
            } else {
                ev.type = EVENTO_FAILED_NUM_INPUT_TYPE;
                ev.failed_num_input.num_buff_len = data->cur_length;
                for (int i = 0; i < data->max_length; ++i) {
                    ev.failed_num_input.num_buff[i] = data->num_buff[i];
                }
            }
        }

    }
    if(data->show_num_buff != NULL){ data->show_num_buff(data->num_buff, data->cur_length, data->cursor); }
    return ev;
}

typedef struct EVENTO_SOURCE_GET_NUM_BUFF
{
    EVENTO_SOURCE_TYPE type;
    get_evento_fun_p fun;
    EVENTO_SOURCE_GET_NUM_BUFF_DATA data;
} EVENTO_SOURCE_GET_NUM_BUFF;


/* Posibles eventos que devuelve:
    - EVENTO_TIMEOUT_TYPE */
/****************
 *   GET_TIME_OUT
 * *************/

typedef struct EVENTO_SOURCE_GET_TIME_OUT_DATA
{

} EVENTO_SOURCE_GET_TIME_OUT_DATA;

evento_t get_time_out_fun(EVENTO_SOURCE_GET_TIME_OUT_DATA * data, suceso_t suceso)
{
    evento_t ev;
    ev.type = (suceso == SUC_DELAY) ? EVENTO_TIMEOUT_TYPE : EVENTO_N;
    return ev;
}

typedef struct EVENTO_SOURCE_GET_TIME_OUT
{
    EVENTO_SOURCE_TYPE type;
    get_evento_fun_p fun;
    EVENTO_SOURCE_GET_TIME_OUT_DATA data;
} EVENTO_SOURCE_GET_TIME_OUT;


/* Posibles eventos que devuelve:
    - EVENTO_CANCEL_TYPE */
/****************
 *   GET_CANCEL_DATA
 * *************/

typedef struct EVENTO_SOURCE_GET_CANCEL_DATA
{
    unsigned int taps;
} EVENTO_SOURCE_GET_CANCEL_DATA;

evento_t get_cancel_fun(EVENTO_SOURCE_GET_CANCEL_DATA * data, suceso_t suceso)
{
    // Se genera evento CANCEL despues de 3 taps
    evento_t ev;
    ev.type = EVENTO_N;

    if(suceso != SUC_TAP_DETECTED)
    {
        data->taps = 0;
    }
    else if(++(data->taps) == TAPS_TO_CANCEL) {
        data->taps = 0;
        ev.type = EVENTO_CANCEL_TYPE;
    }
    return ev;
}

typedef struct EVENTO_SOURCE_GET_CANCEL
{
    EVENTO_SOURCE_TYPE type;
    get_evento_fun_p fun;
    EVENTO_SOURCE_GET_CANCEL_DATA data;
} EVENTO_SOURCE_GET_CANCEL;


/****************
 *   EVENTO_SOURCE
 * *************/

union EVENTO_SOURCE_DATA
{
    EVENTO_SOURCE_GET_NUM_BUFF_DATA get_id_data;
    EVENTO_SOURCE_GET_TIME_OUT_DATA get_time_out_data;
    EVENTO_SOURCE_GET_CANCEL_DATA get_cancel_data;
};

//para acceder a los campos fun y data sin analizar type:
typedef struct EVENTO_SOURCE_ANY
{
    EVENTO_SOURCE_TYPE type;
    get_evento_fun_p fun;
    EVENTO_SOURCE_DATA data;
} EVENTO_SOURCE_ANY;


union EVENTO_SOURCE
{
    EVENTO_SOURCE_TYPE type;    //para saber el tipo rapido
    EVENTO_SOURCE_ANY any;
    EVENTO_SOURCE_GET_NUM_BUFF get_id;
    EVENTO_SOURCE_GET_TIME_OUT get_time_out;
    EVENTO_SOURCE_GET_CANCEL get_cancel;
};




/********************************************************
 * ******************************************************
 * FUNCIONES MIEMBRO
 * ******************************************************
 * ******************************************************/


// Constructor (without allocation for evento_source)
static void evento_source__init(EVENTO_SOURCE* self, EVENTO_SOURCE_TYPE type, void * _params)
{
    self->type = type;
    switch (type)
    {
        case EVENTO_SOURCE_GET_NUM_BUFF_TYPE:
            self->get_id.fun = (get_evento_fun_p)get_num_buff_fun;

            EVENTO_SOURCE_GET_NUM_BUFF_PARAMS * params = (EVENTO_SOURCE_GET_NUM_BUFF_PARAMS *)_params;
            self->get_id.data.cursor = 0;
            self->get_id.data.changing_num = true;
        	self->get_id.data.card_enabled = params->card_enabled;
            self->get_id.data.max_length = params->max_length;
            self->get_id.data.min_length = params->min_length;
            self->get_id.data.is_num_buff_correct = params->is_num_buff_correct;
            self->get_id.data.show_num_buff = params->show_num_buff;
            self->get_id.data.cur_length = 0;
            self->get_id.data.num_buff = malloc(params->max_length + 1);
            if(params->initial_num_buff && params->initial_num_buff_len){
                //si se indica un arreglo inicial especifico, lo copio
                //y guardo su longitud
                int i;
                for(i = 0; i < params->initial_num_buff_len && i < params->max_length; i++){
                    self->get_id.data.num_buff[i] = params->initial_num_buff[i];
                }
                self->get_id.data.cur_length = i;
            } else {
                //si no se indica ningun arreglo inicial, pongo 0 y
                //todas las demas posiciones en UINT8_MAX para
                //indicar que estan vacias
                self->get_id.data.num_buff[0] = 0;
                for(int i = 1; i < params->max_length; i++){
                    self->get_id.data.num_buff[i] = UINT8_MAX;
                }
            }
            break;
        case EVENTO_SOURCE_GET_TIME_OUT_TYPE:
            self->get_time_out.fun = (get_evento_fun_p)get_time_out_fun;
            break;
        case EVENTO_SOURCE_GET_CANCEL_TYPE:
            self->get_cancel.fun = (get_evento_fun_p)get_cancel_fun;
            self->get_cancel.data.taps = 0;
            break;
        default:
            self->type = EVENTO_SOURCE_TYPE_N;
            self->any.fun = NULL;
    }
}

// new evento_source(type);
EVENTO_SOURCE * evento_source__create(EVENTO_SOURCE_TYPE type, void * params) {
    EVENTO_SOURCE * result = (EVENTO_SOURCE *) malloc(sizeof(EVENTO_SOURCE));
    evento_source__init(result, type, params);
    return result;
}

// Destructor (without deallocation)
void evento_source__reset(EVENTO_SOURCE* self)
{
    if(self->type == EVENTO_SOURCE_GET_NUM_BUFF_TYPE)
    {
        if(self->get_id.data.num_buff) {
            free(self->get_id.data.num_buff);
            self->get_id.data.num_buff = NULL;
        }
    }
    self->type = EVENTO_SOURCE_TYPE_N;
}

// Destructor + deallocation (equivalent to "delete evento_source")
void evento_source__destroy(EVENTO_SOURCE* self) {
    if (self) {
        evento_source__reset(self);
        free(self);
    }
}

void evento_source_restart(EVENTO_SOURCE *self, void * params) {
    EVENTO_SOURCE_TYPE type = self->type;
    evento_source__reset(self);
    evento_source__init(self, type, params);
}

evento_t get_evento(EVENTO_SOURCE *self, suceso_t suceso) {
    if(self->any.fun == NULL){
        evento_t ev;
        ev.type = EVENTO_N;
        return ev;
    }
    return self->any.fun(&self->any.data, suceso);
}

//devuelve true si el resultado es un espacio (el "numero" despues de ID_MAX_NUM y antes de 0)
bool scroll_up_ID_num(uint8_t * num){
    if(*num > ID_MAX_NUM){
    //si estaba en espacio, paso a 0.
        *num = 0;
        return false;
    } else if(*num == ID_MAX_NUM){
    //si estaba en ID_MAX_NUM, paso a espacio
        *num = UINT8_MAX;
        return true;
    } else {
        (*num)++;
        return false;
    }

}

//devuelve true si el resultado es un espacio (el "numero" despues de ID_MAX_NUM y antes de 0)
bool scroll_down_ID_num(uint8_t * num){
    if(*num > ID_MAX_NUM){
        //si estaba en espacio, paso a ID_MAX_NUM.
        *num = ID_MAX_NUM;
        return false;
    } else if(*num == 0){
        //si estaba en 0, paso a espacio
        *num = UINT8_MAX;
        return true;
    } else {
        (*num)--;
        return false;
    }

}
