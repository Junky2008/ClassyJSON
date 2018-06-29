#include "ClassyJSON.h"
#include "ClassyJSON_private.h"
#include "debug.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

/**
 * Public Functions (declared in ClassyJSON.h)
 */
void CJ_parse(char *JSONdata, JObject *object)
{
	//LOG(LOG_INFO, "CJ_parse");
	if (JSONdata[0] != '{')
	{
		LOG(LOG_ERR, "Error\n");
		object = NULL;
		return;
	}
	char *data = JSONdata;
	CJ_initObject(object);
	object->isObject = true;

	getObject(&data, object);
}

bool CJ_exists(JObject *object, const char *key)
{
	//LOG(LOG_DEBUG, "CJ_exists");
	int i;
	for (i = 0; i < object->size; i++)
	{
		if (strcmp(object->objects[i].key, key) == 0)
		{
			return true;
		}
	}
	return false;
}

JObject *CJ_getKey(JObject *object, const char *key)
{
	//LOG(LOG_DEBUG, "CJ_getKey");
	int i;
	for (i = 0; i < object->size; i++)
	{
		if (strcmp(object->objects[i].key, key) == 0)
		{
			return &object->objects[i];
		}
	}
	return NULL;
}

JObject *CJ_getIndex(JObject *object, int i)
{
	//LOG(LOG_DEBUG, "CJ_getIndex");
	if (i < object->size)
	{
		return &(object->objects[i]);
	}
	return NULL;
}

/**
 * Private Functions (declared in ClassyJSON_private.h)
 */
char *findOneOf(char *haystack, char *needles)
{
	//LOG(LOG_DEBUG, "findOneOf");
	char *firstNeedle = needles;
	while (*haystack)
	{
		needles = firstNeedle;
		while (*needles)
		{
			if (*haystack == *needles)
			{
				return haystack;
			}
			else
			{
				*needles++;
			}
		}
		*haystack++;
	}
}

void CJ_initObject(JObject *object)
{
	//LOG(LOG_DEBUG, "initObject");
	object->isObject = false;
	object->isArray = false;
	object->key = NULL;
	object->valueAsString = NULL;
	object->valueAsInt16 = 0;
	object->valueAsHex = 0;
	object->valueAsBool = NULL;
	object->size = 0;
	object->objects = NULL;
}

int getItem(char **data, char **item)
{
	//LOG(LOG_DEBUG, "getItem");
	int i;

	for (i = 0; i < strlen(*data); i++)
	{
		if ((*data)[i] == '{')
		{
			(*data) = &((*data)[i]);
			return 1;
		}
		else if ((*data)[i] == '[')
		{
			(*data) = &((*data)[i]);
			return 2;
		}
		else if ((*data)[i] == '"')
		{
			char *start = &(*data)[i];
			start++;
			char *end = strstr(start, "\"");
			*item = (char*)malloc(sizeof(char) * ((end - start) + 1));
			memset(*item, '\0', (end - start) + 1);
			strncpy(*item, start, end - start);
			(*data) = ++end;
			readEmptyChars(data, false);
			return 0;
		}
		else if ((*data)[i] == ',')
		{
			//ignore for now
		}
		else if ((*data)[i] != ' ' && (*data)[i] != '\t' && (*data)[i] != '\n' && (*data)[i] != '\r' && (*data)[i] != ':')
		{
			char *start = &(*data)[i];

			char *end = findOneOf(start, ", \n\r]}\0");

			*item = (char*)malloc(sizeof(char) * ((end - start) + 1));
			memset(*item, '\0', (end - start) + 1);
			strncpy(*item, start, end - start);
			if (end[0] == ']' || end[0] == '}')
			{
				end--;
			}
			(*data) = ++end;
			readEmptyChars(data, false);
			return 0;
		}
		else
		{
			//printf("found %c\n", (*data)[i]);
		}
	}
}

void getKeyValuePair(char **data, JObject *object)
{
	//LOG(LOG_INFO, "getKeyValuePair");
	if (getItem(data, &object->key) != 0)
	{
		LOG(LOG_ERR, "KV1: error (%c)\n", (*data)[0]);
		(*data)++;
		return;
	}

	if (getItem(data, &object->valueAsString) != 0)
	{
		if ((*data)[0] == '[')
		{
			object->isArray = true;
			(*data)++;
			getArray(data, object);

		}
		else if ((*data)[0] == '{')
		{
			object->isObject = true;
			(*data)++;
			getObject(data, object);
		}
		else
		{
			LOG(LOG_ERR, "KV2: error (%c)\n", (*data)[0]);
			(*data)++;
			return;
		}
	}
	else
	{
		parseData(object->valueAsString, &object->valueAsInt16, &object->valueAsHex, &object->valueAsBool);
	}
}

void getArray(char **data, JObject *object)
{
	//LOG(LOG_DEBUG, "getArray");
	if ((*data)[0] == '[')
		(*data)++;
	while ((*data)[0] != ']')
	{
		int size = (sizeof(JObject) * (object->size + 1)) + 1;
		object->objects = (JObject *)realloc(object->objects, size);
		CJ_initObject(&object->objects[object->size]);
		if (getItem(data, &(object->objects[object->size].valueAsString)) != 0)
		{
			if ((*data)[0] == '[')
			{
				object->objects[object->size].isArray = true;
				(*data)++;
				getArray(data, &object->objects[object->size]);

			}
			else if ((*data)[0] == '{')
			{
				object->objects[object->size].isObject = true;
				(*data)++;
				getObject(data, &object->objects[object->size]);
			}
			else
			{
				LOG(LOG_ERR, "AR: error (%c)\n", (*data)[0]);
				(*data)++;
				return;
			}
		}
		else
		{
			parseData(object->objects[object->size].valueAsString, &object->objects[object->size].valueAsInt16, &object->objects[object->size].valueAsHex, &object->objects[object->size].valueAsBool);
		}
		object->size++;
	}
	(*data)++;
	readEmptyChars(data, true);

}

void getObject(char **data, JObject *object)
{
	//LOG(LOG_DEBUG, "getObject");
	if ((*data)[0] == '{')
		(*data)++;
	while ((*data)[0] != '}')
	{
		int size = (sizeof(JObject) * (object->size + 1)) + 1;
		object->objects = (JObject *)realloc(object->objects, sizeof(JObject) * (object->size + 1));
		CJ_initObject(&object->objects[object->size]);
		getKeyValuePair(data, &object->objects[object->size]);
		object->size++;
	}
	(*data)++;
	readEmptyChars(data, true);
}

void parseData(char *string, long *integer, long *hex, bool *boolean)
{
	//LOG(LOG_DEBUG, "parseData");
	*integer = (long)strtoul(string, NULL, 10);
	*hex = (long)strtoul(string, NULL, 16);
	char *str = (char *)malloc(strlen(string) + 1);
	memset(str, '\0', strlen(string) + 1);
	strcpy(str, string);
	int i;
	for (i = 0; str[i]; i++) {
		str[i] = tolower(str[i]);
	}
	if (strcmp(str, "true") == 0)
	{
		*boolean = true;
	}
	else if (strcmp(str, "false") == 0)
	{
		*boolean = false;
	}
}

void readEmptyChars(char **data, bool includeComma)
{
	//LOG(LOG_DEBUG, "readEmptyChars");
	while ((*data)[0] == ' ' || (*data)[0] == '\t' || (*data)[0] == '\n' || (*data)[0] == '\r' || (includeComma && (*data)[0] == ','))
	{
		(*data)++;
	}
}

int freePair(JObject *object)
{
	if(object->isObject || object->isArray)
	{
		return -1;
	}

	object->valueAsBool = false;
	object->valueAsHex = 0;
	object->valueAsInt16 = 0;
	free(object->valueAsString);
	object->valueAsString = NULL;
}

int CJ_free(JObject *object)
{
	int i;
	if (object->size != 0)
	{
		for (i = 0; i < object->size; i++)
		{
			JObject *child = CJ_getIndex(object, i);
			if(CJ_free(child) != 0)
			{
				return -1;
			}
			child = NULL;
		}
		free(object->objects);
		object->objects = NULL;
		object->size = 0;
	}
	free(object->key);
	object->key = NULL;
	free(object->valueAsString);
	object->valueAsString = NULL;
	object->valueAsBool = false;
	object->valueAsHex = 0;
	object->valueAsInt16 = 0;
	return 0;
}


bool CJ_addKeyInt(JObject *object, char *key, int keyLength, long value)
{
	//LOG(LOG_DEBUG, "CJ_addKeyInt");
	int d = (value == 0 ? 1 : ((int)(log10(fabs(value)) + 1) + (value < 0 ? 1 : 0)));
	char *stringValue = (char *)malloc((d) * sizeof(char));
	sprintf(stringValue, "%d", (unsigned int)value);
	return CJ_addKeyString(object, key, keyLength, stringValue, d);
	return false;
}

bool CJ_addKeyHex(JObject *object, char *key, int keyLength, unsigned long value)
{
	//LOG(LOG_DEBUG, "CJ_addKeyHex");
	int d = (value == 0 ? 1 : ((int)(log10(fabs(value)) + 1) + (value < 0 ? 1 : 0)));
	char *stringValue = (char *)malloc((d) * sizeof(char));
	memset(stringValue, '\0', sizeof(stringValue));
	sprintf(stringValue, "%X", (unsigned int)value);
	return CJ_addKeyString(object, key, keyLength, stringValue, d);
	return false;
}

bool CJ_addKeyBool(JObject *object, char *key, int keyLength, bool value)
{
	//LOG(LOG_DEBUG, "CJ_addKeyBool");
	if (value)
	{
		return CJ_addKeyString(object, key, keyLength, "true", 4);
	}
	else
	{
		return CJ_addKeyString(object, key, keyLength, "true", 5);
	}
}

bool CJ_addKeyString(JObject *object, char *key, int keyLength, char *value, int length)
{
	//LOG(LOG_DEBUG, "CJ_addKeyString");
	if (object->isArray || CJ_exists(object, key))
	{
		return false;
	}
	object->isObject = true;
	int size = (sizeof(JObject) * (object->size + 1));
	object->objects = (JObject *)realloc(object->objects, sizeof(JObject) * (object->size + 1) + 1);
	CJ_initObject(&object->objects[object->size]);
	object->objects[object->size].key = (char *)malloc(keyLength + 1);
	memset(object->objects[object->size].key, '\0', keyLength + 1);
	strncpy(object->objects[object->size].key, key, keyLength);
	object->objects[object->size].valueAsString = (char*)malloc(length + 1);
	memset(object->objects[object->size].valueAsString, '\0', length + 1);
	strncpy(object->objects[object->size].valueAsString, value, length);
	parseData(object->objects[object->size].valueAsString, &object->objects[object->size].valueAsInt16, &object->objects[object->size].valueAsHex, &object->objects[object->size].valueAsBool);
	object->size++;
	return true;
}

bool CJ_addInt(JObject *object, long value)
{
	//LOG(LOG_DEBUG, "CJ_addInt");
	int d = (value == 0 ? 1 : ((int)(log10(fabs(value)) + 1) + (value < 0 ? 1 : 0)));
	char *stringValue = (char *)malloc(d);
	sprintf(stringValue, "%d", (unsigned int)value);
	return CJ_addString(object, stringValue, d);
	return false;
}

bool CJ_addHex(JObject *object, unsigned long value)
{
	//LOG(LOG_DEBUG, "CJ_addHex");
	int d = (value == 0 ? 1 : ((int)(log10(fabs(value)) + 1) + (value < 0 ? 1 : 0)));
	char *stringValue = (char *)malloc(d);
	memset(stringValue, '\0', sizeof(stringValue));
	sprintf(stringValue, "%X", (unsigned int)value);
	return CJ_addString(object, stringValue, d);
	return false;
}

bool CJ_addBool(JObject *object, bool value)
{
	//LOG(LOG_DEBUG, "CJ_addBool");
	if (value)
	{
		return CJ_addString(object, "true", 4);
	}
	else
	{
		return CJ_addString(object, "true", 5);
	}
}

bool CJ_addString(JObject *object, char *value, int length)
{
	//LOG(LOG_DEBUG, "CJ_addString");
	if (object->isObject)
	{
		return false;
	}
	object->isArray = true;
	int size = (sizeof(JObject) * (object->size + 1));
	object->objects = (JObject *)realloc(object->objects, sizeof(JObject) * (object->size + 1) + 1);
	CJ_initObject(&object->objects[object->size]);
	object->objects[object->size].valueAsString = (char*)malloc(sizeof(char) * (length + 1));
	memset(object->objects[object->size].valueAsString, '\0', sizeof(object->objects[object->size].valueAsString));
	strncpy(object->objects[object->size].valueAsString, value, length);
	parseData(object->objects[object->size].valueAsString, &object->objects[object->size].valueAsInt16, &object->objects[object->size].valueAsHex, &object->objects[object->size].valueAsBool);
	object->size++;
	return true;
}
