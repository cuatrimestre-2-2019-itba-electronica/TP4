//
// Created by mlste on 9/4/2019.
//

#ifndef EVENT_SOURCES_H
#define EVENT_SOURCES_H

#include "waiting_id_suceso_sources.h"
#include <stdlib.h>
#include <stdbool.h>
#include "eventos.h"

typedef enum {
    EVENTO_SOURCE_GET_NUM_BUFF_TYPE,
    EVENTO_SOURCE_GET_TIME_OUT_TYPE,
    EVENTO_SOURCE_GET_CANCEL_TYPE,
    EVENTO_SOURCE_TYPE_N
} EVENTO_SOURCE_TYPE;


//TODO: me falta un event_source o algo que sirva para seleccionar dentro de una lista

// Tipo incomplete para encapsulamiento (gracias Allegro por la inspo)
union EVENTO_SOURCE;
typedef union EVENTO_SOURCE EVENTO_SOURCE;

EVENTO_SOURCE * evento_source__create(EVENTO_SOURCE_TYPE type, void * params);
void evento_source__destroy(EVENTO_SOURCE* self);
void evento_source_restart(EVENTO_SOURCE * self, void * params);
evento_t get_evento(EVENTO_SOURCE * self, suceso_t suceso);


typedef bool (*get_num_buff_validate_cb)(uint8_t * num_buff, uint8_t length);
typedef bool (*get_num_buff_show_cb)(uint8_t * num_buff, uint8_t length, uint8_t cursor_pos);

typedef struct EVENTO_SOURCE_GET_NUM_BUFF_PARAMS{
    uint8_t max_length;                     //maxima longitud del arreglo buscado
    uint8_t min_length;                     //minima longitud del arreglo buscado

    get_num_buff_validate_cb is_num_buff_correct;    //funcion que valida que el arreglo de numeros ingresado sea correcto.
    get_num_buff_show_cb show_num_buff;          //funcion que muestra al usuario el estado del buffer

    bool card_enabled;						//true si quiero leer de la tarjeta

    uint8_t * initial_num_buff;             //buffer con el arreglo inicial predefinido. Si es NULL, se ignora y el arreglo inicia vacio
    uint8_t initial_num_buff_len;           //tamanio del buffer del arreglo inicial predefinido. Si es 0, se ignora y el arreglo inicia vacio
}EVENTO_SOURCE_GET_NUM_BUFF_PARAMS;

#endif //EVENT_SOURCES_H
