#include "MainTestSuite.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "ClassyJSON.h"

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
	// go back to the start of the file
	rewind(fp);

	char *fileContent = (char *)malloc(sizeof(char) * (string_size + 1));

	int i = 0;
	char ch;
	while ((ch = fgetc(fp)) != EOF)
	{
		fileContent[i] = ch;
		i++;
	}
	fileContent[i] = '\0';

	fclose(fp);

	return fileContent;
}

START_TEST(simpleObjectTest)
{
	char *fileContent = openJSONFile("./test/SimpleObjectTest.json");

	JObject object;
	CJ_parse(fileContent, &object);

	ck_assert(CJ_exists(&object, "naam") == true);
	ck_assert(CJ_exists(&object, "work") == true);
	ck_assert(CJ_exists(&object, "age") == true);

	ck_assert(CJ_getKey(&object, "naam")->isObject == false);
	ck_assert(CJ_getKey(&object, "naam")->size == 0);
	ck_assert(CJ_getKey(&object, "work")->isObject == false);
	ck_assert(CJ_getKey(&object, "work")->size == 0);
	ck_assert(CJ_getKey(&object, "age")->isObject == false);
	ck_assert(CJ_getKey(&object, "age")->size == 0);

	ck_assert_str_eq(CJ_getKey(&object, "naam")->valueAsString, "Jane Doe");
	ck_assert_str_eq(CJ_getKey(&object, "work")->valueAsString, "Example.org");
	ck_assert(CJ_getKey(&object, "age")->valueAsInt16 == 39);

	freeJObject(&object);
	free(fileContent);
	fileContent = NULL;
}
END_TEST

START_TEST(simpleArrayTest)
{
	char *fileContent = openJSONFile("./test/SimpleArrayTest.json");

	JObject object;
	CJ_parse(fileContent, &object);

	ck_assert(CJ_exists(&object, "stringArray") == true);
	ck_assert(CJ_exists(&object, "intArray") == true);
	ck_assert(CJ_getKey(&object, "stringArray")->isArray == true);
	ck_assert(CJ_getKey(&object, "intArray")->isArray == true);
	ck_assert(CJ_getKey(&object, "stringArray")->size == 3);
	ck_assert(CJ_getKey(&object, "intArray")->size == 6);
	ck_assert_str_eq(CJ_getIndex(CJ_getKey(&object, "stringArray"), 0)->valueAsString, "first");
	ck_assert_str_eq(CJ_getIndex(CJ_getKey(&object, "stringArray"), 1)->valueAsString, "second");
	ck_assert_str_eq(CJ_getIndex(CJ_getKey(&object, "stringArray"), 2)->valueAsString, "third");

	ck_assert(CJ_getIndex(CJ_getKey(&object, "intArray"), 0)->valueAsInt16 == 1);
	ck_assert(CJ_getIndex(CJ_getKey(&object, "intArray"), 1)->valueAsInt16 == 2);
	ck_assert(CJ_getIndex(CJ_getKey(&object, "intArray"), 2)->valueAsInt16 == 3);
	ck_assert(CJ_getIndex(CJ_getKey(&object, "intArray"), 3)->valueAsInt16 == 4);
	ck_assert(CJ_getIndex(CJ_getKey(&object, "intArray"), 4)->valueAsInt16 == 5);
	ck_assert(CJ_getIndex(CJ_getKey(&object, "intArray"), 5)->valueAsInt16 == 6);

	freeJObject(&object);
	free(fileContent);
	fileContent = NULL;
}
END_TEST

START_TEST(multipleObjectTest)
{
	char *fileContent = openJSONFile("./test/MultipleObjectTest.json");
	
	JObject object;
	CJ_parse(fileContent, &object);

	ck_assert(CJ_exists(&object, "user1") == true);
	ck_assert(CJ_exists(&object, "user2") == true);

	ck_assert(CJ_getKey(&object, "user1")->isObject == true);
	ck_assert(CJ_getKey(&object, "user2")->isObject == true);

	ck_assert(CJ_getKey(&object, "user1")->size == 3);
	ck_assert(CJ_getKey(&object, "user2")->size == 3);

	JObject *user1 = CJ_getKey(&object, "user1");

	ck_assert(CJ_exists(user1, "naam") == true);
	ck_assert(CJ_exists(user1, "afgestudeerd") == true);
	ck_assert(CJ_exists(user1, "age") == true);

	ck_assert(CJ_getKey(user1, "naam")->isObject == false);
	ck_assert(CJ_getKey(user1, "afgestudeerd")->isObject == false);
	ck_assert(CJ_getKey(user1, "age")->isObject == false);

	ck_assert(CJ_getKey(user1, "naam")->size == 0);
	ck_assert(CJ_getKey(user1, "afgestudeerd")->size == 0);
	ck_assert(CJ_getKey(user1, "age")->size == 0);

	ck_assert_str_eq(CJ_getKey(user1, "naam")->valueAsString, "John Doe");
	ck_assert(CJ_getKey(user1, "afgestudeerd")->valueAsBool == true);
	ck_assert(CJ_getKey(user1, "age")->valueAsInt16 == 39);

	JObject *user2 = CJ_getKey(&object, "user2");

	ck_assert(CJ_exists(user2, "naam") == true);
	ck_assert(CJ_exists(user2, "afgestudeerd") == true);
	ck_assert(CJ_exists(user2, "age") == true);

	ck_assert(CJ_getKey(user2, "naam")->isObject == false);
	ck_assert(CJ_getKey(user2, "afgestudeerd")->isObject == false);
	ck_assert(CJ_getKey(user2, "age")->isObject == false);

	ck_assert(CJ_getKey(user2, "naam")->size == 0);
	ck_assert(CJ_getKey(user2, "afgestudeerd")->size == 0);
	ck_assert(CJ_getKey(user2, "age")->size == 0);

	ck_assert_str_eq(CJ_getKey(user2, "naam")->valueAsString, "Jane Doe");
	ck_assert(CJ_getKey(user2, "afgestudeerd")->valueAsBool == false);
	ck_assert(CJ_getKey(user2, "age")->valueAsInt16 == 55);

	freeJObject(&object);
	free(fileContent);
	fileContent = NULL;
}
END_TEST

START_TEST(recursiveArrayTest)
{
	char *fileContent = openJSONFile("./test/RecursiveArrayTest.json");

	JObject object;
	CJ_parse(fileContent, &object);

	ck_assert(CJ_exists(&object, "masterArray") == true);
	ck_assert(CJ_getKey(&object, "masterArray")->isArray == true);
	ck_assert(CJ_getKey(&object, "masterArray")->size == 2);

	JObject *arrayItem1;

	arrayItem1 = CJ_getIndex(CJ_getKey(&object, "masterArray"), 0);

	ck_assert(arrayItem1 != NULL);
	ck_assert(arrayItem1->isArray == true);
	ck_assert(arrayItem1->size == 2);

	ck_assert_str_eq(CJ_getIndex(arrayItem1, 0)->valueAsString, "test1");
	ck_assert_str_eq(CJ_getIndex(arrayItem1, 1)->valueAsString, "test2");

	JObject *arrayItem2;

	arrayItem2 = CJ_getIndex(CJ_getKey(&object, "masterArray"), 1);

	ck_assert(arrayItem2 != NULL);
	ck_assert(arrayItem2->isArray == true);
	ck_assert(arrayItem2->size == 2);

	ck_assert(CJ_getIndex(arrayItem2, 0)->valueAsInt16 == 1);
	ck_assert(CJ_getIndex(arrayItem2, 1)->valueAsInt16 == 2);

	freeJObject(&object);
	free(fileContent);
	fileContent = NULL;
}
END_TEST

START_TEST(recursiveObjectTest)
{
	char *fileContent = openJSONFile("./test/RecursiveObjectTest.json");

	JObject object;
	CJ_parse(fileContent, &object);

	ck_assert(CJ_exists(&object, "masterObject") == true);

	JObject *masterObject;
	masterObject = CJ_getKey(&object, "masterObject");

	ck_assert(masterObject->isObject == true);
	ck_assert(masterObject->size == 2);
	ck_assert(CJ_exists(masterObject, "ChildObject1") == true);
	ck_assert(CJ_exists(masterObject, "ChildObject2") == true);

	JObject *ChildObject1;
	ChildObject1 = CJ_getKey(masterObject, "ChildObject1");

	ck_assert(ChildObject1 != NULL);

	ck_assert(ChildObject1->isObject == true);
	ck_assert(ChildObject1->size == 2);
	ck_assert(CJ_exists(ChildObject1, "test1") == true);
	ck_assert(CJ_exists(ChildObject1, "test2") == true);

	ck_assert(CJ_getKey(ChildObject1, "test1")->valueAsInt16 == 1);
	ck_assert_str_eq(CJ_getKey(ChildObject1, "test2")->valueAsString, "test2");

	JObject *ChildObject2;
	ChildObject2 = CJ_getKey(masterObject, "ChildObject2");

	ck_assert(ChildObject2 != NULL);

	ck_assert(ChildObject2->isObject == true);
	ck_assert(ChildObject2->size == 3);
	ck_assert(CJ_exists(ChildObject2, "test1") == true);
	ck_assert(CJ_exists(ChildObject2, "test2") == true);
	ck_assert(CJ_exists(ChildObject2, "thirdObject") == true);

	ck_assert(CJ_getKey(ChildObject2, "test1")->valueAsInt16 == 1);
	ck_assert_str_eq(CJ_getKey(ChildObject2, "test2")->valueAsString, "test2");

	JObject *thirdObject;
	thirdObject = CJ_getKey(ChildObject2, "thirdObject");

	ck_assert(thirdObject != NULL);

	ck_assert(thirdObject->isObject == true);
	ck_assert(thirdObject->size == 1);

	ck_assert(CJ_exists(thirdObject, "data") == true);
	ck_assert_str_eq(CJ_getKey(thirdObject, "data")->valueAsString, "data");

	freeJObject(&object);
	free(fileContent);
	fileContent = NULL;
}
END_TEST

START_TEST(recursiveTest)
{
	char *fileContent = openJSONFile("./test/RecursiveTest.json");

	JObject object;
	CJ_parse(fileContent, &object);

	ck_assert(CJ_exists(&object, "Object") == true);
	ck_assert(CJ_exists(&object, "Array") == true);
	ck_assert(CJ_exists(&object, "Object2") == true);

	JObject *Object;
	Object = CJ_getKey(&object, "Object");

	ck_assert(Object->isObject == true);
	ck_assert(Object->size == 2);
	ck_assert(CJ_exists(Object, "Array1inObject") == true);
	ck_assert(CJ_exists(Object, "Array2inObject") == true);

	JObject *Array1inObject;
	Array1inObject = CJ_getKey(Object, "Array1inObject");

	ck_assert(Array1inObject->isArray == true);
	ck_assert(Array1inObject->size == 3);

	ck_assert(CJ_getIndex(Array1inObject, 0)->valueAsInt16 == 1);
	ck_assert(CJ_getIndex(Array1inObject, 1)->valueAsInt16 == 2);
	ck_assert(CJ_getIndex(Array1inObject, 2)->valueAsInt16 == 3);

	JObject *Array2inObject;
	Array2inObject = CJ_getKey(Object, "Array2inObject");

	ck_assert(Array2inObject->isArray == true);
	ck_assert(Array2inObject->size == 5);

	ck_assert_str_eq(CJ_getIndex(Array2inObject, 0)->valueAsString, "first");
	ck_assert_str_eq(CJ_getIndex(Array2inObject, 1)->valueAsString, "second");
	ck_assert_str_eq(CJ_getIndex(Array2inObject, 2)->valueAsString, "third");
	ck_assert_str_eq(CJ_getIndex(Array2inObject, 3)->valueAsString, "fourth");
	ck_assert_str_eq(CJ_getIndex(Array2inObject, 4)->valueAsString, "fifth");

	JObject *Array;
	Array = CJ_getKey(&object, "Array");

	ck_assert(Array->isArray == true);
	ck_assert(Array->size == 2);

	JObject *Object1inArray;
	Object1inArray = CJ_getIndex(Array, 0);

	ck_assert(Object1inArray->isObject == true);
	ck_assert(Object1inArray->size == 2);
	ck_assert(CJ_getKey(Object1inArray, "Object1inArray")->valueAsBool == true);
	ck_assert_str_eq(CJ_getKey(Object1inArray, "data")->valueAsString, "data");

	JObject *Object2inArray;
	Object2inArray = CJ_getIndex(Array, 1);

	ck_assert(Object2inArray->isObject == true);
	ck_assert(Object2inArray->size == 2);
	ck_assert(CJ_getKey(Object2inArray, "Object2inArray")->valueAsBool == true);
	ck_assert(CJ_getKey(Object2inArray, "data")->valueAsHex == 16);

	JObject *Object2;
	Object2 = CJ_getKey(&object, "Object2");
	ck_assert(Object2->isObject == true);
	ck_assert(Object2->size == 1);

	JObject *ArrayInObject;
	ArrayInObject = CJ_getKey(Object2, "ArrayInObject");

	ck_assert(ArrayInObject->isArray == true);
	ck_assert(ArrayInObject->size == 1);

	JObject *ObjectinArrayinObject;
	ObjectinArrayinObject = CJ_getIndex(ArrayInObject, 0);

	ck_assert(ObjectinArrayinObject->isObject == true);
	ck_assert(ObjectinArrayinObject->size == 2);

	ck_assert(CJ_getKey(ObjectinArrayinObject, "ObjectinArrayinObject")->valueAsBool == true);
	ck_assert(CJ_getKey(ObjectinArrayinObject, "data")->valueAsInt16 == 10);

}
END_TEST

Suite* GetMainTestSuite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Main");

	/* Core test case */
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, simpleObjectTest);
	tcase_add_test(tc_core, simpleArrayTest);
	tcase_add_test(tc_core, multipleObjectTest);
	tcase_add_test(tc_core, recursiveArrayTest);
	tcase_add_test(tc_core, recursiveObjectTest);
	tcase_add_test(tc_core, recursiveTest);
	suite_add_tcase(s, tc_core);

	return s;
}