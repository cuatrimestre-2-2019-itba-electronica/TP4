//
// Created by mlste on 9/9/2019.
//

#include "database.h"

#include <stdint.h>
#include <stdlib.h>

#define DATABASE_MAX_ENTRIES 20


static DATABASE_ENTRY database[DATABASE_MAX_ENTRIES];
//static DATABASE_ENTRY * cursor = NULL; //en desuso. Para ampliar horizontes
static int cursor_pos = -1;
static unsigned int len = 0;



//Hash function sacada de Kernighan & Ritchie 1era edicion
static hash_t BKDRHash(const char* str, unsigned int length);

// bool ID: si es true, devuelve ID_hash, sino devuelve PIN_hash
// Si existe, devuelve el hash (ID o PIN) de la posicion pos
// Si no existe, devuelve 0
static hash_t database_get_hash_at(bool ID, unsigned int pos);

// bool ID: si es true, devuelve ID_hash, sino devuelve PIN_hash
// Si existe, devuelve el hash (ID o PIN) de la posicion pos
// Si no existe, devuelve 0
static bool database_set_hash_at(bool ID, unsigned int pos, hash_t hash);


/**********************
 * FUNCIONES GLOBALES *
 **********************/

//false si out of bounds
bool database_set_cursor_pos(unsigned int _cursor_pos){
    if(cursor_pos >= len){ return  false; }
    cursor_pos = _cursor_pos;
    return true;
}
int database_get_cursor_pos(){
    return cursor_pos;
}

hash_t database_get_ID_hash_at_cursor(){
    return database_get_hash_at(true, cursor_pos);
}

hash_t database_get_PIN_hash_at_cursor(){
    return database_get_hash_at(false, cursor_pos);
}

hash_t database_get_ID_hash_at(unsigned int pos){
    return database_get_hash_at(true, pos);
}

hash_t database_get_PIN_hash_at(unsigned int pos){
    return database_get_hash_at(false, pos);
}

bool database_set_ID_hash_at_cursor(hash_t PIN_hash) {
    return database_set_hash_at(true, cursor_pos, PIN_hash);
}

bool database_set_PIN_hash_at_cursor(hash_t PIN_hash){
    return database_set_hash_at(false, cursor_pos, PIN_hash);
}

//false si out of bounds
bool database_set_ID_hash_at(hash_t ID_hash, unsigned int pos){
    return database_set_hash_at(true, pos, ID_hash);
}

//false si out of bounds
bool database_set_PIN_hash_at(hash_t PIN_hash, unsigned int pos){
    return database_set_hash_at(false, pos, PIN_hash);
}

//no modifica el cursor
void database_append(hash_t ID_hash, hash_t PIN_hash){
    database[len].ID_hash  = ID_hash;
    database[len].PIN_hash = PIN_hash;
    len++;
    //si estoy haciendo append del primer item, habilito el cursor
    if(cursor_pos == -1){ cursor_pos = 0; }
}

unsigned int database_get_size(){
    return len;
}

unsigned int database_empty(){
    len = 0;
    cursor_pos = -1;
}

hash_t database_get_hash(uint8_t * buff, unsigned int _len){
    return  BKDRHash((const char *)buff, _len);
}

int database_get_pos_from_hash(bool ID, hash_t hash){
    int i = 0;

    //Avanzo i hasta el final o hasta encontrar el hash buscado
    for (i = 0;
        (i < len) && (ID ? (hash != database[i].ID_hash) : (hash != database[i].PIN_hash)) ;
        ++i)
    {}
    return i==len ? -1 : i;
}

/*********************
 * FUNCIONES LOCALES *
 *********************/

static hash_t database_get_hash_at(bool ID, unsigned int pos)
{
    if(pos < len){
        return ID ? database[pos].ID_hash : database[pos].PIN_hash;
    }
    return 0;
}

static bool database_set_hash_at(bool ID, unsigned int pos, hash_t hash)
{
    if(pos < len){
        ID ? (database[pos].ID_hash = hash) : (database[pos].PIN_hash = hash);
        return true;
    }
    return false;
}

static hash_t BKDRHash(const char* str, unsigned int length)
{
    unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
    unsigned int hash = 0;
    unsigned int i    = 0;
    for (i = 0; i < length; ++str, ++i){
        hash = (hash * seed) + (*str);
    }
    return hash;
}

void database_populate(){
	uint8_t ID[] = {4,5,1,7,6,6,0,1};
	uint8_t PIN[] = {4,5,9,8};
	database_append(database_get_hash(ID, 8), database_get_hash(PIN,3));
	uint8_t ID1[] = {3,7,6,6,3,6,4,5};
	uint8_t PIN1[] = {3,2,3,2};
	database_append(database_get_hash(ID1, 8), database_get_hash(PIN1,4));
	uint8_t ID2[] = {0,1,2,3,4,5,6,7};
	uint8_t PIN2[] = {5,5,5,5,5};
	database_append(database_get_hash(ID2, 8), database_get_hash(PIN2,5));
}

