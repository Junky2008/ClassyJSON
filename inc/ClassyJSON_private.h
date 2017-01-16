#ifndef __CLASSYJSON_PRIVATE_H__
#define __CLASSYJSON_PRIVATE_H__

#include "ClassyJSON.h"

/**
 * find one of the needles in the haystack, when a needle is found, the position is returned
 */
char *findOneOf(char *haystack, char *needles);

/**
 * initialise default JObject
 */
void initObject(JObject *object);

/**
 * get item, an item can be a string, integer, hex value or boolean
 */
int getItem(char **data, char **item);

/**
 * parse key value pair, key will be string, value can be string, integer, hex value or boolean
 * examples:
 *     "data": "data"
 *     "data": 1
 *     "data": 0x1
 *     "data": true
 */
void getKeyValuePair(char **data, JObject *object);

/**
 * parse an array (starting with [, ending with ])
 */
void getArray(char **data, JObject *object);

/**
 * parse an object (starting with {, ending with })
 */
void getObject(char **data, JObject *object);

/**
 * parse data, convert the given string to an integer (if possible), a hex value (if possible) and a boolean (if possible)
 */
void parseData(char *string, long *integer, long *hex, bool *boolean);

/**
 * continue the data character pointer to skip empty characters, boolean to include or exclude a comma as an empty character
 * empty characters:
 * '\t', '\r', '\n', ' '
 */
void readEmptyChars(char **data, bool includeComma);

#endif //__CLASSYJSON_PRIVATE_H__
