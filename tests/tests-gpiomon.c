/*
 * Test cases for the gpiomon tool.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 */

#include "gpiod-test.h"

#include <signal.h>
#include <unistd.h>

static void gpiomon_single_rising_edge_event(void)
{
	test_tool_run("gpiomon", "--rising-edge", "--num-events=1",
		      test_chip_name(1), "4", (char *)NULL);
	test_set_event(1, 4, TEST_EVENT_RISING, 200);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_RET_OK(test_tool_exit_status());
	TEST_ASSERT_NOT_NULL(test_tool_stdout());
	TEST_ASSERT_NULL(test_tool_stderr());
	TEST_ASSERT_REGEX_MATCH(test_tool_stdout(),
				"event\\:\\s+RISING\\s+EDGE\\s+offset\\:\\s+4\\s+timestamp:\\s+\\[[0-9]+\\.[0-9]+\\]");
}
TEST_DEFINE(gpiomon_single_rising_edge_event,
	    "tools: gpiomon - single rising edge event",
	    0, { 8, 8 });

static void gpiomon_single_rising_edge_event_active_low(void)
{
	test_tool_run("gpiomon", "--rising-edge", "--num-events=1",
		      "--active-low", test_chip_name(1), "4", (char *)NULL);
	test_set_event(1, 4, TEST_EVENT_RISING, 200);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_RET_OK(test_tool_exit_status());
	TEST_ASSERT_NOT_NULL(test_tool_stdout());
	TEST_ASSERT_NULL(test_tool_stderr());
	TEST_ASSERT_REGEX_MATCH(test_tool_stdout(),
				"event\\:\\s+RISING\\s+EDGE\\s+offset\\:\\s+4\\s+timestamp:\\s+\\[[0-9]+\\.[0-9]+\\]");
}
TEST_DEFINE(gpiomon_single_rising_edge_event_active_low,
	    "tools: gpiomon - single rising edge event (active-low)",
	    0, { 8, 8 });

static void gpiomon_single_rising_edge_event_silent(void)
{
	test_tool_run("gpiomon", "--rising-edge", "--num-events=1",
		      "--silent", test_chip_name(1), "4", (char *)NULL);
	test_set_event(1, 4, TEST_EVENT_RISING, 200);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_RET_OK(test_tool_exit_status());
	TEST_ASSERT_NULL(test_tool_stdout());
	TEST_ASSERT_NULL(test_tool_stderr());
}
TEST_DEFINE(gpiomon_single_rising_edge_event_silent,
	    "tools: gpiomon - single rising edge event (silent mode)",
	    0, { 8, 8 });

static void gpiomon_four_alternating_events(void)
{
	test_tool_run("gpiomon", "--num-events=4",
		      test_chip_name(1), "4", (char *)NULL);
	test_set_event(1, 4, TEST_EVENT_ALTERNATING, 100);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_RET_OK(test_tool_exit_status());
	TEST_ASSERT_NOT_NULL(test_tool_stdout());
	TEST_ASSERT_NULL(test_tool_stderr());
	TEST_ASSERT_REGEX_MATCH(test_tool_stdout(),
				"event\\:\\s+FALLING\\s+EDGE\\s+offset\\:\\s+4\\s+timestamp:\\s+\\[[0-9]+\\.[0-9]+\\]");
	TEST_ASSERT_REGEX_MATCH(test_tool_stdout(),
				"event\\:\\s+RISING\\s+EDGE\\s+offset\\:\\s+4\\s+timestamp:\\s+\\[[0-9]+\\.[0-9]+\\]");
}
TEST_DEFINE(gpiomon_four_alternating_events,
	    "tools: gpiomon - four alternating events",
	    0, { 8, 8 });

static void gpiomon_falling_edge_events_sigint(void)
{
	test_tool_run("gpiomon", "--falling-edge",
		      test_chip_name(0), "4", (char *)NULL);
	test_set_event(0, 4, TEST_EVENT_FALLING, 100);
	usleep(200000);
	test_tool_signal(SIGINT);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_RET_OK(test_tool_exit_status());
	TEST_ASSERT_NOT_NULL(test_tool_stdout());
	TEST_ASSERT_NULL(test_tool_stderr());
	TEST_ASSERT_REGEX_MATCH(test_tool_stdout(),
				"event\\:\\s+FALLING\\s+EDGE\\s+offset\\:\\s+4\\s+timestamp:\\s+\\[[0-9]+\\.[0-9]+\\]");
}
TEST_DEFINE(gpiomon_falling_edge_events_sigint,
	    "tools: gpiomon - receive falling edge events and kill with SIGINT",
	    0, { 8, 8 });

static void gpiomon_both_events_sigterm(void)
{
	test_tool_run("gpiomon", "--falling-edge", "--rising-edge",
		      test_chip_name(0), "4", (char *)NULL);
	test_set_event(0, 4, TEST_EVENT_ALTERNATING, 100);
	usleep(300000);
	test_tool_signal(SIGTERM);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_RET_OK(test_tool_exit_status());
	TEST_ASSERT_NOT_NULL(test_tool_stdout());
	TEST_ASSERT_NULL(test_tool_stderr());
	TEST_ASSERT_REGEX_MATCH(test_tool_stdout(),
				"event\\:\\s+FALLING\\s+EDGE\\s+offset\\:\\s+4\\s+timestamp:\\s+\\[[0-9]+\\.[0-9]+\\]");
	TEST_ASSERT_REGEX_MATCH(test_tool_stdout(),
				"event\\:\\s+RISING\\s+EDGE\\s+offset\\:\\s+4\\s+timestamp:\\s+\\[[0-9]+\\.[0-9]+\\]");
}
TEST_DEFINE(gpiomon_both_events_sigterm,
	    "tools: gpiomon - receive both types of events and kill with SIGTERM",
	    0, { 8, 8 });

static void gpiomon_no_arguments(void)
{
	test_tool_run("gpiomon", (char *)NULL);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_EQ(test_tool_exit_status(), 1);
	TEST_ASSERT_NULL(test_tool_stdout());
	TEST_ASSERT_NOT_NULL(test_tool_stderr());
	TEST_ASSERT_STR_CONTAINS(test_tool_stderr(),
				 "gpiochip must be specified");
}
TEST_DEFINE(gpiomon_no_arguments,
	    "tools: gpiomon - no arguments",
	    0, { });

static void gpiomon_line_not_specified(void)
{
	test_tool_run("gpiomon", test_chip_name(1), (char *)NULL);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_EQ(test_tool_exit_status(), 1);
	TEST_ASSERT_NULL(test_tool_stdout());
	TEST_ASSERT_NOT_NULL(test_tool_stderr());
	TEST_ASSERT_STR_CONTAINS(test_tool_stderr(),
				 "GPIO line offset must be specified");
}
TEST_DEFINE(gpiomon_line_not_specified,
	    "tools: gpiomon - line not specified",
	    0, { 4, 4 });

static void gpiomon_line_out_of_range(void)
{
	test_tool_run("gpiomon", test_chip_name(0), "4", (char *)NULL);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_EQ(test_tool_exit_status(), 1);
	TEST_ASSERT_NULL(test_tool_stdout());
	TEST_ASSERT_NOT_NULL(test_tool_stderr());
	TEST_ASSERT_STR_CONTAINS(test_tool_stderr(),
				 "error waiting for events");
}
TEST_DEFINE(gpiomon_line_out_of_range,
	    "tools: gpiomon - line out of range",
	    0, { 4 });

static void gpiomon_more_than_one_line_given(void)
{
	test_tool_run("gpiomon", test_chip_name(0), "2", "3", (char *)NULL);
	test_tool_wait();

	TEST_ASSERT(test_tool_exited());
	TEST_ASSERT_EQ(test_tool_exit_status(), 1);
	TEST_ASSERT_NULL(test_tool_stdout());
	TEST_ASSERT_NOT_NULL(test_tool_stderr());
	TEST_ASSERT_STR_CONTAINS(test_tool_stderr(),
				 "watching more than one GPIO line unsupported");
}
TEST_DEFINE(gpiomon_more_than_one_line_given,
	    "tools: gpiomon - more than one line given",
	    0, { 4 });
