/*
 * Test cases for the gpiofind tool.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 */

#include "gpiod-test.h"

static void gpiofind_found(void)
{
	test_gpiotool_run("gpiofind", "gpio-mockup-B-7", (char *)NULL);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_RET_OK(test_tool_exit_status());
	TEST_ASSERT_NOT_NULL(test_tool_stdout());
	TEST_ASSERT_STR_EQ(test_tool_stdout(), "gpiochip1 7\n");
	TEST_ASSERT_NULL(test_tool_stderr());
}
TEST_DEFINE(gpiofind_found,
	    "tools: gpiofind - found",
	    TEST_FLAG_NAMED_LINES, { 4, 8 });

static void gpiofind_not_found(void)
{
	test_gpiotool_run("gpiofind", "nonexistent", (char *)NULL);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_EQ(test_tool_exit_status(), 1);
	TEST_ASSERT_NULL(test_tool_stdout());
	TEST_ASSERT_NULL(test_tool_stderr());
}
TEST_DEFINE(gpiofind_not_found,
	    "tools: gpiofind - not found",
	    TEST_FLAG_NAMED_LINES, { 4, 8 });
