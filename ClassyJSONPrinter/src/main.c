#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ClassyJSON.h"

#define OK 0
#define STOP 1

bool printBoolean = false;
bool printHex = false;
bool printInt = false;
int indentationCount = 0;
void printJObject(JObject *object);

void printHelp()
{
	printf("ClassyJSONPrinter\n");
	printf("\n");
	printf("usage: ClassyJSONPrinter [options] file.json\n");
	printf("options:\n");
	printf("\t-h:\tshow this help text\n");
	printf("\t-i:\tprint integer values\n");
	printf("\t-x:\tprint hex values\n");
	printf("\t-b:\tprint boolean values\n");
	printf("\n");
}

void freeJObject(JObject *object)
{
	int i;
	if (object->size != 0)
	{
		for (i = 0; i < object->size; i++)
		{
			freeJObject(CJ_getIndex(object, i));
		}
		free(object->objects);
		object->objects = NULL;
	}
	return;
}

char *openJSONFile(char *filename)
{
	FILE *fp = fopen(filename, "r"); // read mode

	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	int string_size;
	// Seek the last byte of the file
	fseek(fp, 0, SEEK_END);
	// Offset from the first to the last byte, or in other words, filesize
	string_size = ftell(fp);
	printf("size: %d\n", string_size);
	// go back to the start of the file
	rewind(fp);

	char *fileContent = (char *)malloc(sizeof(char) * (string_size + 1));

	int i = 0;
	char ch;
	while ((ch = fgetc(fp)) != EOF && i < string_size)
	{
		fileContent[i] = ch;
		i++;
	printf("i: %d\n", i);
	}
	printf("content: %s\n", fileContent);
	fileContent[i] = '\0';

	fclose(fp);

	return fileContent;
}

void printTabs()
{
	int i;
	for (i = 0; i < indentationCount; i++)
	{
		printf("\t");
	}
}

void printKeyValue(JObject *object)
{
	if (object->key != NULL)
	{
		printf("%s: ", object->key);
	}
	if (printBoolean || printHex || printInt)
	{
		printf("( ");
	}
	printf("\"%s\"", object->valueAsString);
	if (printInt)
	{
		printf(", %ld", object->valueAsInt16);
	}
	if (printHex)
	{
		printf(", 0x%lX", object->valueAsHex);
	}
	if (printBoolean)
	{
		printf(", %s", object->valueAsBool ? "true" : "false");
	}

	if (printBoolean || printHex || printInt)
	{
		printf(" )");
	}
	printf("\n");
}

void printArray(JObject *object)
{
	if (object->key != NULL)
	{
		printf("%s: ", object->key);
	}
	printf("[\n");
	indentationCount++;
	int i;
	for (i = 0; i < object->size; i++)
	{
		printJObject(&object->objects[i]);
	}
	indentationCount--; 
	printTabs();
	printf("]\n");
}

void printObject(JObject *object)
{
	if (object->key != NULL)
	{
		printf("%s: ", object->key);
	}
	printf("{\n");
	indentationCount++;
	int i;
	for (i = 0; i < object->size; i++)
	{
		printJObject(&object->objects[i]);
	}
	indentationCount--;
	printTabs();
	printf("}\n");
}

void printJObject(JObject *object)
{
	if (object->isObject)
	{
		printTabs();
		printObject(object);
	}
	else if (object->isArray)
	{
		printTabs();
		printArray(object);
	}
	else
	{
		printTabs();
		printKeyValue(object);
	}
}

int handleInput(char *input)
{
	if (strcmp(input, "-h") == 0)
	{
		printHelp();
		return STOP;
	}
	else if (strcmp(input, "-i") == 0)
	{
		printInt = true;
	}
	else if (strcmp(input, "-x") == 0)
	{
		printHex = true;
	}
	else if (strcmp(input, "-b") == 0)
	{
		printBoolean = true;
	}
	else
	{
		printHelp();
		return STOP;
	}
	return OK;
}

int main(int argc, char** argv)
{
	printf("start\n");
	if (argc < 2)
	{
		printHelp();
		return 0;
	}

	printf("start\n");
	if (argc > 2)
	{
		int i = 0;
		for (i = 1; i < argc - 1; i++)
		{
			if (handleInput(argv[i]) == STOP)
			{
				return 0;
			}
		}
	}
	if (strcmp(argv[argc - 1], "-h") == 0)
	{
		printHelp();
		return 0;
	}

	printf("open\n");
	char* JSONdata = openJSONFile(argv[argc - 1]);
	printf("data: [%s]\n", JSONdata);
	
	JObject object;
	printf("parse\n");
	CJ_parse(JSONdata, &object);
	printf("parsed\n");

	char *testData;
	CJ_getString(&object, &testData);
	printf("\n\n[%s]\n\n", testData);
	free(testData);
	testData = NULL;
	
	printf("print\n");
	printJObject(&object);

	printf("exit\n");
	freeJObject(&object);
	
	free(JSONdata);
	JSONdata = NULL;
	
	printf("done\n");
	return 0;
}