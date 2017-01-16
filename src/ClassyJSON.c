#include "ClassyJSON.h"
#include "ClassyJSON_private.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Public Functions (declared in ClassyJSON.h)
 */
void CJ_parse(char *JSONdata, JObject *object)
{
	if (JSONdata[0] != '{')
	{
		printf("Error\n");
		object = NULL;
		return;
	}
	char *data = JSONdata;
	initObject(object);
	object->isObject = true;

	getObject(&data, object);
}

bool CJ_exists(JObject *object, const char *key)
{
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

void initObject(JObject *object)
{
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
			strncpy(*item, start, end - start);
			(*item)[end - start] = '\0';
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
			strncpy(*item, start, end - start);
			(*item)[end - start] = '\0';
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
	if (getItem(data, &object->key) != 0)
	{
		printf("error\n");
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
			printf("error\n");
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
	if ((*data)[0] == '[')
		(*data)++;
	while ((*data)[0] != ']')
	{
		int size = (sizeof(JObject) * (object->size + 1));
		object->objects = (JObject *)realloc(object->objects, size);
		initObject(&object->objects[object->size]);
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
				printf("error\n");
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
	if ((*data)[0] == '{')
		(*data)++;
	while ((*data)[0] != '}')
	{
		int size = (sizeof(JObject) * (object->size + 1));
		object->objects = (JObject *)realloc(object->objects, sizeof(JObject) * (object->size + 1));
		initObject(&object->objects[object->size]);
		getKeyValuePair(data, &object->objects[object->size]);
		object->size++;
	}
	(*data)++;
	readEmptyChars(data, true);
}

void parseData(char *string, long *integer, long *hex, bool *boolean)
{
	*integer = (long)strtol(string, NULL, 10);
	*hex = (long)strtol(string, NULL, 16);
	char *str = (char *)malloc(sizeof(char) * strlen(string));
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
	while ((*data)[0] == ' ' || (*data)[0] == '\t' || (*data)[0] == '\n' || (*data)[0] == '\r' || (includeComma && (*data)[0] == ','))
	{
		(*data)++;
	}
}
