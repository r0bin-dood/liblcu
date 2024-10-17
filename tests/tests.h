#ifndef TESTS_H
#define TESTS_H

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/CUnit.h>

void add_list_tests(CU_pSuite suite);
void add_tpool_tests(CU_pSuite suite);

#endif // TESTS_H