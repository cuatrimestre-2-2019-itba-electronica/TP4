//
// Created by mlste on 9/8/2019.
//


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "estados.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "event_sources.h"

#include "database.h"

#include"8DigitsDisplay.h"
#include "3LedDsip.h"




/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define EST_WAITING_ID_NUM_BUFF_MAX_LENGTH  ID_MAX_LENGTH
#define EST_WAITING_ID_NUM_BUFF_MIN_LENGTH  ID_MIN_LENGTH
#define EST_WAITING_PIN_NUM_BUFF_MAX_LENGTH  PIN_MAX_LENGTH
#define EST_WAITING_PIN_NUM_BUFF_MIN_LENGTH  PIN_MIN_LENGTH

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct state_transition_array_entry_t{
    evento_type_t event_type;
    //recibe el ultimo evento por si necesita usar su info extra:
    estado_t * (*callback)(evento_t * ev);
}state_transition_array_entry_t;

/***********
 * ESTADOS *
 * *********/

//ESTADO_ANY (PARA PODER ACCEDER A LOS CAMPOS SIN SABER EL TIPO)

typedef struct estado_any_t{
    estado_type_t type;
    bool event_queue_created;
}estado_any_t;

//ESTADO_WAITING_ID

typedef estado_any_t estado_waiting_ID_t;

//ESTADO_WAITING_PIN

typedef struct estado_waiting_PIN_data_t{
    uint8_t * ID_buffer;
    uint8_t ID_buffer_len;
    uint8_t failed_attempts;    // En desuso. Para ampliar horizontes.
}estado_waiting_PIN_data_t;

typedef struct estado_waiting_PIN_t{
    estado_type_t type;
    bool event_queue_created;
    estado_waiting_PIN_data_t data;
}estado_waiting_PIN_t;

//ESTADO_GOT_ACCESS

typedef estado_any_t estado_got_access_t;

//ESTADO_EDITING_PIN

typedef struct estado_editing_PIN_data_t{
    uint8_t * PIN_buffer;
    uint8_t PIN_buffer_len;
}estado_editing_PIN_data_t;

typedef struct estado_editing_pin_t{
    estado_type_t type;
    bool event_queue_created;
    estado_editing_PIN_data_t data;
}estado_editing_pin_t;

//UNION DE TODOS LOS ESTADOS:

typedef union estado_t{
    estado_type_t type;
    estado_any_t any;
    estado_waiting_ID_t waiting_ID;
    estado_waiting_PIN_t waiting_PIN;
    estado_got_access_t got_access;
    estado_editing_pin_t editing_PIN;
}estado_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void estado__init(estado_t * self, estado_type_t type, void * params);
static void estado__reset(estado_t * self);


/********************
 * CALLBACKS DE FSM *
 * ******************/

static estado_t * dummy (evento_t * ev){ return NULL; }

//PARA LOS ESTADOS QUE TENGAN TIMEOUT:
static estado_t * est_xx_ev_timeout(evento_t * ev);

//ESTADO_WAITING_ID
static estado_t * est_waiting_ID_ev_cor_num_input(evento_t * ev);
static estado_t * est_waiting_ID_ev_fail_num_input(evento_t * ev);

//ESTADO_WAITING_PIN
static estado_t * est_waiting_PIN_ev_cor_num_input(evento_t * ev);
static estado_t * est_waiting_PIN_ev_fail_num_input(evento_t * ev);

//ESTADO_GOT_ACCESS
static estado_t * est_got_access_ev_edit_pin_input(evento_t * ev);

//ESTADO_EDIT_PIN
static estado_t * est_editing_PIN_ev_cor_num_input(evento_t * ev);
static estado_t * est_editing_PIN_ev_fail_num_input(evento_t * ev);

/*******************************
 * CALLBACKS DE EVENTO_SOURCES *
 * *****************************/

//ESTADO_WAITING_ID
static bool validate_ID(uint8_t * ID_buffer, uint8_t ID_buffer_len);
static bool validate_PIN(uint8_t * PIN_buffer, uint8_t PIN_buffer_len);
static bool show_ID(uint8_t * ID_buffer, uint8_t ID_buffer_len, uint8_t cursor_pos);
static bool show_PIN(uint8_t * ID_buffer, uint8_t ID_buffer_len, uint8_t cursor_pos);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

state_transition_array_entry_t state_transition_array_waiting_ID[] = \
{
    {EVENTO_CORRECT_NUM_INPUT_TYPE, est_waiting_ID_ev_cor_num_input},
    {EVENTO_FAILED_NUM_INPUT_TYPE,  est_waiting_ID_ev_fail_num_input},
    {EVENTO_CANCEL_TYPE, est_xx_ev_timeout},
    {EVENTO_TIMEOUT_TYPE, est_xx_ev_timeout},
    {EVENTO_N, NULL}
};

state_transition_array_entry_t state_transition_array_waiting_PIN[] = \
{
    {EVENTO_CORRECT_NUM_INPUT_TYPE, est_waiting_PIN_ev_cor_num_input},
    {EVENTO_FAILED_NUM_INPUT_TYPE,  est_waiting_PIN_ev_fail_num_input},
    {EVENTO_CANCEL_TYPE, est_xx_ev_timeout},
    {EVENTO_TIMEOUT_TYPE, est_xx_ev_timeout},
    {EVENTO_N, NULL}
};


state_transition_array_entry_t state_transition_array_got_access[] = \
{
        {EVENTO_CANCEL_TYPE, est_xx_ev_timeout},
        {EVENTO_TIMEOUT_TYPE, est_xx_ev_timeout},
        {EVENTO_N, NULL}
};
  //todo: crear eventos de seleccionar de lista para got_access

state_transition_array_entry_t state_transition_array_editing_PIN[] = \
{
        {EVENTO_CORRECT_NUM_INPUT_TYPE, est_editing_PIN_ev_cor_num_input},
        {EVENTO_FAILED_NUM_INPUT_TYPE,  est_editing_PIN_ev_fail_num_input},
        {EVENTO_CANCEL_TYPE, est_xx_ev_timeout},
        {EVENTO_TIMEOUT_TYPE, est_xx_ev_timeout},
        {EVENTO_N, NULL}
};

state_transition_array_entry_t state_transition_array_empty[] = \
{
        {EVENTO_N, NULL}
};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

//Constructor + allocation (equivalent to "new estado")
estado_t * estado__create(estado_type_t type, void *params) {
    estado_t * result = (estado_t *) malloc(sizeof(estado_t));
    estado__init(result, type, params);
    return result;
}

// Destructor + deallocation (equivalent to "delete estado")
void estado__destroy(estado_t *self) {
    if (self) {
        estado__reset(self);
        free(self);
    }
}

void estado_restart(estado_t *self, void *params) {
    estado_type_t type = self->type;
    estado__reset(self);
    estado__init(self, type, params);
}

estado_t * estado_dispatch_evento(estado_t *self, evento_t evento) {
    state_transition_array_entry_t * entry;
    switch (self->type)
    {
        case EST_WAITING_ID_TYPE:
            entry = state_transition_array_waiting_ID;
            break;
        case EST_WAITING_PIN_TYPE:
            entry = state_transition_array_waiting_PIN;
            break;
        case EST_GOT_ACCESS_TYPE:
            entry = state_transition_array_got_access;
            break;
        case EST_EDIT_PIN_TYPE:
            entry = state_transition_array_editing_PIN;
            break;
        case EVENTO_N:
        	default:
            entry = state_transition_array_empty;
            break;
    }

    //Si el evento esta registrado en el state transition array correspondiente,
    //llamo al callback y devuelvo el nuevo estado.
    //Sino, NULL
    for ( ; entry->event_type != evento.type && entry->event_type != EVENTO_N ; entry++){}
    if(entry->event_type != EVENTO_N) {
        return entry->callback(&evento);
    }
    return NULL;
}

EVENTO_QUEUE * estado_create_evento_queue(estado_t *self) {
    if(self->any.event_queue_created){ return NULL; }
    self->any.event_queue_created = true;

    EVENTO_QUEUE * q = evento_queue__create();
    if(q == NULL) { return NULL; }

    //Configuro y registro las fuentes de eventos relevantes
    EVENTO_SOURCE * evento_source;

    //Para todos los eventos:
    evento_source = evento_source__create(EVENTO_SOURCE_GET_TIME_OUT_TYPE, NULL);
    append_evento_source(q,evento_source);
    evento_source = evento_source__create(EVENTO_SOURCE_GET_CANCEL_TYPE, NULL);
    append_evento_source(q,evento_source);

    //Para eventos especificos:
    EVENTO_SOURCE_GET_NUM_BUFF_PARAMS num_buff_params;
    switch (self->type)
    {
        case EST_WAITING_ID_TYPE:
        	_3LedDisp_setLed(0, true);
        	_3LedDisp_setLed(1, false);
        	_3LedDisp_setLed(2, false);
        	num_buff_params.card_enabled = true;
            num_buff_params.initial_num_buff = NULL;
            num_buff_params.initial_num_buff_len = 0;
            num_buff_params.max_length = EST_WAITING_ID_NUM_BUFF_MAX_LENGTH;
            num_buff_params.min_length = EST_WAITING_ID_NUM_BUFF_MIN_LENGTH;
            num_buff_params.show_num_buff = show_ID;
            num_buff_params.is_num_buff_correct = validate_ID;
            evento_source = evento_source__create(EVENTO_SOURCE_GET_NUM_BUFF_TYPE, (void *)&num_buff_params );
            append_evento_source(q,evento_source);
            break;
        case EST_WAITING_PIN_TYPE:
        	_3LedDisp_setLed(0, false);
        	_3LedDisp_setLed(1, true);
        	_3LedDisp_setLed(2, false);
        	num_buff_params.card_enabled = false;
            num_buff_params.initial_num_buff = NULL;
            num_buff_params.initial_num_buff_len = 0;
            num_buff_params.max_length = EST_WAITING_PIN_NUM_BUFF_MAX_LENGTH;
            num_buff_params.min_length = EST_WAITING_PIN_NUM_BUFF_MIN_LENGTH;
            num_buff_params.show_num_buff = show_PIN;
            num_buff_params.is_num_buff_correct = validate_PIN;
            evento_source = evento_source__create(EVENTO_SOURCE_GET_NUM_BUFF_TYPE, (void *)&num_buff_params );
            append_evento_source(q,evento_source);
            break;
        case EST_GOT_ACCESS_TYPE:
        	_3LedDisp_setLed(0, false);
        	_3LedDisp_setLed(1, false);
        	_3LedDisp_setLed(2, true);
        	break;
        case EST_EDIT_PIN_TYPE:
            num_buff_params.initial_num_buff_len = EST_WAITING_PIN_NUM_BUFF_MAX_LENGTH;
            num_buff_params.initial_num_buff = malloc(num_buff_params.initial_num_buff_len);
            if(num_buff_params.initial_num_buff == NULL){ break; }
            for (int i = 0; i < num_buff_params.initial_num_buff_len; ++i) {
                num_buff_params.initial_num_buff[i] = i;
            }
            num_buff_params.max_length = EST_WAITING_PIN_NUM_BUFF_MAX_LENGTH;
            num_buff_params.min_length = EST_WAITING_PIN_NUM_BUFF_MIN_LENGTH;
            num_buff_params.show_num_buff = show_ID;
            num_buff_params.is_num_buff_correct = validate_ID;
            evento_source = evento_source__create(EVENTO_SOURCE_GET_NUM_BUFF_TYPE, (void *)&num_buff_params );
            append_evento_source(q,evento_source);
            break;
    }

    return q;
}

estado_type_t estado_get_type(estado_t *self) {
    return self->type;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



void estado__init(estado_t *self, estado_type_t type, void *params) {
    self->type = type;
    self->any.event_queue_created = false;
    switch (type)
    {
        case EST_WAITING_ID_TYPE:
            break;
        case EST_WAITING_PIN_TYPE:
            self->waiting_PIN.data = *(estado_waiting_PIN_data_t *)params;
            break;
        case EST_GOT_ACCESS_TYPE:
            break;
        case EST_EDIT_PIN_TYPE:
            self->editing_PIN.data = *(estado_editing_PIN_data_t *)params;
            break;
        default:
            break;
    }

}


// Destructor (without deallocation)
static void estado__reset(estado_t * self){
    switch (self->type)
    {
        case EST_WAITING_ID_TYPE:
            break;
        case EST_WAITING_PIN_TYPE:
            free(self->waiting_PIN.data.ID_buffer);
            break;
        case EST_GOT_ACCESS_TYPE:
            break;
        case EST_EDIT_PIN_TYPE:
            break;
        default:
            break;
    }
}

//PARA LOS ESTADOS QUE TENGAN TIMEOUT:
static estado_t * est_xx_ev_timeout(evento_t * ev){
    _8DigitDisplay_reset();
    return estado__create(EST_WAITING_ID_TYPE, NULL);
}

//ESTADO_WAITING_ID
static estado_t * est_waiting_ID_ev_cor_num_input(evento_t * ev) {
	_8DigitDisplay_reset();
    hash_t hash = database_get_hash(ev->correct_num_input.num_buff, ev->correct_num_input.num_buff_len);
    database_set_cursor_pos(database_get_pos_from_hash(true, hash));

    //nadie hace free de la memoria del evento? le puedo pasar el puntero directo?
    //rta: si!! evento_queue se desentendio de esa memoria una vez generado
    estado_waiting_PIN_data_t data = {ev->correct_num_input.num_buff,
                                      ev->correct_num_input.num_buff_len,
                                      0};
    return estado__create(EST_WAITING_PIN_TYPE, &data);
}

static estado_t * est_waiting_ID_ev_fail_num_input(evento_t * ev) {
    _8DigitDisplay_reset();
    return estado__create(EST_WAITING_ID_TYPE, NULL);
}

//ESTADO_WAITING_PIN
static estado_t * est_waiting_PIN_ev_cor_num_input(evento_t * ev){
    _8DigitDisplay_reset();
	_3LedDisp_setLed(0, true);
	_7SegDisp_clearDisplay();
	return estado__create(EST_GOT_ACCESS_TYPE, NULL);
}

static estado_t * est_waiting_PIN_ev_fail_num_input(evento_t * ev){
    return estado__create(EST_WAITING_ID_TYPE, NULL);
}

//ESTADO_GOT_ACCESS
static estado_t * est_got_access_ev_edit_pin_input(evento_t * ev){
    _8DigitDisplay_reset();
	uint8_t PIN_buffer_len = PIN_MAX_LENGTH;
    uint8_t * PIN_buffer = malloc(PIN_buffer_len);
    if(PIN_buffer != NULL){
        for (int i = 0; i < PIN_buffer_len; ++i) {
            PIN_buffer[i] = UINT8_MAX;
        }
    }
    estado_editing_PIN_data_t data = {PIN_buffer, PIN_buffer_len};
    return estado__create(EST_EDIT_PIN_TYPE, &data);
}

//ESTADO_EDIT_PIN
static estado_t * est_editing_PIN_ev_cor_num_input(evento_t * ev){
    database_get_PIN_hash_at_cursor(database_get_hash( \
            ev->correct_num_input.num_buff, \
            ev->correct_num_input.num_buff_len) \
            );
    return estado__create(EST_GOT_ACCESS_TYPE, NULL);
}
static estado_t * est_editing_PIN_ev_fail_num_input(evento_t * ev){
    return estado__create(EST_WAITING_ID_TYPE, NULL);
}


bool validate_ID(uint8_t * ID_buffer, uint8_t ID_buffer_len){
    hash_t hash = database_get_hash(ID_buffer, ID_buffer_len);
    int pos = database_get_pos_from_hash(true, hash);
    return pos != -1;
}

bool validate_PIN(uint8_t * ID_buffer, uint8_t ID_buffer_len){
    hash_t hash = database_get_hash(ID_buffer, ID_buffer_len);
    return hash == database_get_PIN_hash_at_cursor();
}

bool show_ID(uint8_t * ID_buffer, uint8_t ID_buffer_len, uint8_t cursor_pos)
{
	_8DigitDisplay_reset();
    _8DigitDisplay_cursorOn();
	for (int i = 0; i < ID_buffer_len; ++i) {
        _8DigitDisplay_append(ID_buffer[i] == UINT8_MAX ? _8DIGIT_BLANK :ID_buffer[i]);
    }
    _8DigitDisplay_SetCursorPos(cursor_pos);
    _8DigitDisplay_SetCursorPosOnScrenn();
//    _8DigitDisplay_SetCursorPos(cursor_pos);
//    _8DigitDisplay_SetCursorPosOnScrenn();
}


static bool show_PIN(uint8_t * ID_buffer, uint8_t ID_buffer_len, uint8_t cursor_pos)
{

	_8DigitDisplay_reset();
	_8DigitDisplay_PinMode(true);
    _8DigitDisplay_cursorOn();
	for (int i = 0; i < ID_buffer_len; ++i) {
        _8DigitDisplay_append(ID_buffer[i] == UINT8_MAX ? _8DIGIT_BLANK :ID_buffer[i]);
    }
    _8DigitDisplay_SetCursorPos(cursor_pos);
    _8DigitDisplay_SetCursorPosOnScrenn();
//    _8DigitDisplay_SetCursorPos(cursor_pos);
//    _8DigitDisplay_SetCursorPosOnScrenn();
}




/******************************************************************************/
