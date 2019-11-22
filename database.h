//
// Created by mlste on 9/9/2019.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>
#include <stdint-gcc.h>

typedef unsigned int hash_t;

typedef struct DATABASE_ENTRY {
    hash_t ID_hash;
    hash_t PIN_hash;
} DATABASE_ENTRY;

//false si out of bounds
bool database_set_cursor_pos(unsigned int cursor_pos);

//Si esta vacio, devuelve -1.
//Si hay elementos, devuelve la posicion del cursor
int database_get_cursor_pos();

//Si esta vacio, devuelve 0.
//Si hay elementos, devuelve el ID_hash del elemento al cual
//apunta el cursor
hash_t database_get_ID_hash_at_cursor();

//Si esta vacio, devuelve 0.
//Si hay elementos, devuelve el PIN_hash del elemento al cual
//apunta el cursor
 hash_t database_get_PIN_hash_at_cursor();

//Si esta vacio o si la posicion excede a la longitud, devuelve 0.
//Si hay elementos, devuelve el ID_hash del elemento de la
// posicion pos
hash_t database_get_ID_hash_at(unsigned int pos);

//Si esta vacio o si la posicion excede a la longitud, devuelve 0.
//Si hay elementos, devuelve el PIN_hash del elemento de la
// posicion pos
hash_t database_get_PIN_hash_at(unsigned int pos);

//false si out of bounds
bool database_set_ID_hash_at_cursor(hash_t PIN_hash);

//false si out of bounds
bool database_set_PIN_hash_at_cursor(hash_t PIN_hash);

//false si out of bounds
bool database_set_ID_hash_at(hash_t PIN_hash, unsigned int pos);

//false si out of bounds
bool database_set_PIN_hash_at(hash_t PIN_hash, unsigned int pos);

//no modifica el cursor
void database_append(hash_t ID_hash, hash_t PIN_hash);

unsigned int database_get_size();

unsigned int database_empty();

hash_t database_get_hash(uint8_t * buff, unsigned int len);

//Devuelve la posicion en la que se hallo el hash especificado.
//Si no se hallo, devuelve -1
int database_get_pos_from_hash(bool ID, hash_t hash);

void database_populate();

#endif //DATABASE_H
