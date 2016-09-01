#include "net/message.h"
#include "unittest.h"

#include <stdio.h>
#include <stdbool.h>

void test_simple_message_all();

void UNITTEST_SET_ALL_SUITE()
{
    UNITTEST_SUITE_INIT();
    UNITTEST_SUITE_SETUP(NULL);
    UNITTEST_SUITE_ADD_TESTCASE("test_simple_message_all", test_simple_message_all);
}

void test_simple_message_all()
{
    SimpleMessage* self = simple_message_create();
    void* p1 = simple_message_get_push_ptr(self, 10);
    memcpy(p1, "0123456789", 10);

    TEST_EQUAL_INT(0, simple_message_size(self));
    simple_message_set_push_size(self, 10);
    TEST_EQUAL_INT(10, simple_message_size(self));

    void* p2 = simple_message_get_push_ptr(self, 10);
    memcpy(p2, "9876543210", 10);

    TEST_EQUAL_INT(10, simple_message_size(self));
    simple_message_set_push_size(self, 10);
    TEST_EQUAL_INT(20, simple_message_size(self));

    TEST_EQUAL_STRING("01234567899876543210", simple_message_get_pull_ptr(self));

    simple_message_clear(self);

     void* p3 = simple_message_get_push_ptr(self, 5);
    memcpy(p3, "01234", 5);

    TEST_EQUAL_INT(0, simple_message_size(self));
    simple_message_set_push_size(self, 5);
    TEST_EQUAL_INT(5, simple_message_size(self));

    void* p4 = simple_message_get_push_ptr(self, 5);
    memcpy(p4, "43210", 10);

    TEST_EQUAL_INT(5, simple_message_size(self));
    simple_message_set_push_size(self, 5);
    TEST_EQUAL_INT(10, simple_message_size(self));

    TEST_EQUAL_STRING("0123443210", simple_message_get_pull_ptr(self));

    simple_message_destroy(self);
}

UNITTEST_REGISTER();
