/*
 * Simple API test cases for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 */

#include "gpiod-unit.h"

static void simple_set_get_value(void)
{
	int ret;

	ret = gpiod_simple_get_value("gpiod-unit", gu_chip_name(0), 3, false);
	GU_ASSERT_EQ(ret, 0);

	ret = gpiod_simple_set_value("gpiod-unit", gu_chip_name(0),
				     3, 1, false, NULL, NULL);
	GU_ASSERT_RET_OK(ret);

	ret = gpiod_simple_get_value("gpiod-unit", gu_chip_name(0), 3, false);
	GU_ASSERT_EQ(ret, 1);
}
GU_DEFINE_TEST(simple_set_get_value,
	       "simple set/get value",
	       GU_LINES_UNNAMED, { 8 });

static void simple_set_get_value_multiple(void)
{
	unsigned int offsets[] = { 0, 1, 2, 3, 4, 5, 6, 12, 13, 15 };
	int values[10], ret;

	ret = gpiod_simple_get_value_multiple("gpiod-unit", gu_chip_name(0),
					      offsets, values, 10, false);
	GU_ASSERT_RET_OK(ret);

	GU_ASSERT_EQ(values[0], 0);
	GU_ASSERT_EQ(values[1], 0);
	GU_ASSERT_EQ(values[2], 0);
	GU_ASSERT_EQ(values[3], 0);
	GU_ASSERT_EQ(values[4], 0);
	GU_ASSERT_EQ(values[5], 0);
	GU_ASSERT_EQ(values[6], 0);
	GU_ASSERT_EQ(values[7], 0);
	GU_ASSERT_EQ(values[8], 0);
	GU_ASSERT_EQ(values[9], 0);

	values[0] = 1;
	values[1] = 1;
	values[2] = 1;
	values[3] = 0;
	values[4] = 0;
	values[5] = 1;
	values[6] = 0;
	values[7] = 1;
	values[8] = 0;
	values[9] = 0;

	ret = gpiod_simple_set_value_multiple("gpiod-unit", gu_chip_name(0),
					      offsets, values, 10, false,
					      NULL, NULL);
	GU_ASSERT_RET_OK(ret);

	ret = gpiod_simple_get_value_multiple("gpiod-unit", gu_chip_name(0),
					      offsets, values, 10, false);
	GU_ASSERT_RET_OK(ret);

	GU_ASSERT_EQ(values[0], 1);
	GU_ASSERT_EQ(values[1], 1);
	GU_ASSERT_EQ(values[2], 1);
	GU_ASSERT_EQ(values[3], 0);
	GU_ASSERT_EQ(values[4], 0);
	GU_ASSERT_EQ(values[5], 1);
	GU_ASSERT_EQ(values[6], 0);
	GU_ASSERT_EQ(values[7], 1);
	GU_ASSERT_EQ(values[8], 0);
	GU_ASSERT_EQ(values[9], 0);
}
GU_DEFINE_TEST(simple_set_get_value_multiple,
	       "simple set/get value multiple",
	       GU_LINES_UNNAMED, { 16 });

static void simple_get_value_multiple_max_lines(void)
{
	unsigned int offsets[GPIOD_REQUEST_MAX_LINES + 1];
	int values[GPIOD_REQUEST_MAX_LINES + 1], ret;

	ret = gpiod_simple_get_value_multiple("gpiod-unit", gu_chip_name(0),
					      offsets, values,
					      GPIOD_REQUEST_MAX_LINES + 1,
					      false);
	GU_ASSERT_NOTEQ(ret, 0);
	GU_ASSERT_EQ(gpiod_errno(), GPIOD_ELINEMAX);
}
GU_DEFINE_TEST(simple_get_value_multiple_max_lines,
	       "gpiod_simple_get_value_multiple() max lines",
	       GU_LINES_UNNAMED, { 128 });

static void simple_set_value_multiple_max_lines(void)
{
	unsigned int offsets[GPIOD_REQUEST_MAX_LINES + 1];
	int values[GPIOD_REQUEST_MAX_LINES + 1], ret;

	ret = gpiod_simple_set_value_multiple("gpiod-unit", gu_chip_name(0),
					      offsets, values,
					      GPIOD_REQUEST_MAX_LINES + 1,
					      false, NULL, NULL);
	GU_ASSERT_NOTEQ(ret, 0);
	GU_ASSERT_EQ(gpiod_errno(), GPIOD_ELINEMAX);
}
GU_DEFINE_TEST(simple_set_value_multiple_max_lines,
	       "gpiod_simple_set_value_multiple() max lines",
	       GU_LINES_UNNAMED, { 128 });
