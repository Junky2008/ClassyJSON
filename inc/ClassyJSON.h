#ifndef __CLASSYJSON_H__
#define __CLASSYJSON_H__

#include <stdbool.h>
#include <math.h>
/**
 * The JObject structure defines a JObject.
 * A JObject can be an Array, an Object or just a simple value.
 * When an object is a value, the values (as string, as int16, as hex and as bool) will be filled and the key will represent the name of the data.
 * When an object is an array or an object, the size represents the amount of objects in the object and the key will represent the name of the array or object.
 * When an object is an array or an object, every object inside it can be an array, an object or a value.
 * When an object is an object, the objects inside it have a key.
 * When an object is an array, the objects inside it do not have a key.
 */
typedef struct Jobject
{
	char *key;
	char *valueAsString;
	long valueAsInt16;
	unsigned long valueAsHex;
	bool valueAsBool;
	bool isArray;
	bool isObject;
	int size;

	struct Jobject *objects;
} JObject;

/**
 * parses the JSON data into an JObject, data is required to start with {
 */
void CJ_parse(char *JSONdata, JObject *object);

/**
 * checks if the given key exists in the given object
 */
bool CJ_exists(JObject *object, const char *key);

/**
 * gets the given key from the object, if the key does not exist, NULL is returned
 */
JObject *CJ_getKey(JObject *object, const char *key);

/**
 * gets the object at location i, if i is bigger than size, NULL is returned
 */
JObject *CJ_getIndex(JObject *object, int i);

/**
*
*/
bool CJ_addKeyInt(JObject *object, char *key, int keyLength, long value);

/**
*
*/
bool CJ_addKeyHex(JObject *object, char *key, int keyLength, unsigned long value);

/**
*
*/
bool CJ_addKeyBool(JObject *object, char *key, int keyLength, bool value);

/**
*
*/
bool CJ_addKeyString(JObject *object, char *key, int keyLength, char *value, int length);
/**
*
*/
bool CJ_addInt(JObject *object, long value);

/**
*
*/
bool CJ_addHex(JObject *object, unsigned long value);

/**
*
*/
bool CJ_addBool(JObject *object, bool value);

/**
*
*/
bool CJ_addString(JObject *object, char *value, int length);

#endif //__CLASSYJSON_H__
