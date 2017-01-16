#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "ClassyJSON.h"
#include "check.h"
#include "MainTestSuite.h"

int main()
{
	int number_failed;
	SRunner *sr;

	sr = srunner_create(GetMainTestSuite());

	srunner_set_fork_status(sr, CK_NOFORK);
	srunner_set_xml(sr, "test.xml");
	srunner_set_log(sr, "test.log");
	srunner_set_tap(sr, "test.tap");

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
