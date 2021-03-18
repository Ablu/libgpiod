// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2017-2021 Bartosz Golaszewski <bartekgola@gmail.com>

#include <catch2/catch.hpp>
#include <gpiod.hpp>

#include "gpio-mockup.hpp"

using ::gpiod::test::mockup;

namespace {

const ::std::string consumer = "line-test";

} /* namespace */

TEST_CASE("Line information can be correctly retrieved", "[line]")
{
	mockup::probe_guard mockup_chips({ 8 }, mockup::FLAG_NAMED_LINES);
	::gpiod::chip chip(mockup::instance().chip_path(0));
	auto line = chip.get_line(4);

	SECTION("unexported line")
	{
		REQUIRE(line.offset() == 4);
		REQUIRE(line.name() == "gpio-mockup-A-4");
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_INPUT);
		REQUIRE_FALSE(line.is_active_low());
		REQUIRE(line.consumer().empty());
		REQUIRE_FALSE(line.is_used());
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);
		REQUIRE(line.bias() == ::gpiod::line::BIAS_UNKNOWN);
	}

	SECTION("exported line")
	{
		::gpiod::line_request config;

		config.consumer = consumer.c_str();
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		line.request(config);

		REQUIRE(line.offset() == 4);
		REQUIRE(line.name() == "gpio-mockup-A-4");
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE_FALSE(line.is_active_low());
		REQUIRE(line.is_used());
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);
		REQUIRE(line.bias() == ::gpiod::line::BIAS_UNKNOWN);
	}

	SECTION("exported line with flags")
	{
		::gpiod::line_request config;

		config.consumer = consumer.c_str();
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = ::gpiod::line_request::FLAG_ACTIVE_LOW |
			       ::gpiod::line_request::FLAG_OPEN_DRAIN;
		line.request(config);

		REQUIRE(line.offset() == 4);
		REQUIRE(line.name() == "gpio-mockup-A-4");
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.is_active_low());
		REQUIRE(line.is_used());
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_OPEN_DRAIN);
		REQUIRE(line.bias() == ::gpiod::line::BIAS_UNKNOWN);
	}

	SECTION("exported open source line")
	{
		::gpiod::line_request config;

		config.consumer = consumer.c_str();
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = ::gpiod::line_request::FLAG_OPEN_SOURCE;
		line.request(config);

		REQUIRE(line.offset() == 4);
		REQUIRE(line.name() == "gpio-mockup-A-4");
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE_FALSE(line.is_active_low());
		REQUIRE(line.is_used());
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_OPEN_SOURCE);
		REQUIRE(line.bias() == ::gpiod::line::BIAS_UNKNOWN);
	}

	SECTION("exported bias disable line")
	{
		::gpiod::line_request config;

		config.consumer = consumer.c_str();
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = ::gpiod::line_request::FLAG_BIAS_DISABLED;
		line.request(config);

		REQUIRE(line.offset() == 4);
		REQUIRE(line.name() == "gpio-mockup-A-4");
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE_FALSE(line.is_active_low());
		REQUIRE(line.is_used());
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);
		REQUIRE(line.bias() == ::gpiod::line::BIAS_DISABLED);
	}

	SECTION("exported pull-down line")
	{
		::gpiod::line_request config;

		config.consumer = consumer.c_str();
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = ::gpiod::line_request::FLAG_BIAS_PULL_DOWN;
		line.request(config);

		REQUIRE(line.offset() == 4);
		REQUIRE(line.name() == "gpio-mockup-A-4");
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE_FALSE(line.is_active_low());;
		REQUIRE(line.is_used());
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);
		REQUIRE(line.bias() == ::gpiod::line::BIAS_PULL_DOWN);
	}

	SECTION("exported pull-up line")
	{
		::gpiod::line_request config;

		config.consumer = consumer.c_str();
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = ::gpiod::line_request::FLAG_BIAS_PULL_UP;
		line.request(config);

		REQUIRE(line.offset() == 4);
		REQUIRE(line.name() == "gpio-mockup-A-4");
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE_FALSE(line.is_active_low());
		REQUIRE(line.is_used());
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);
		REQUIRE(line.bias() == ::gpiod::line::BIAS_PULL_UP);
	}
}

TEST_CASE("Line values can be set and read", "[line]")
{
	mockup::probe_guard mockup_chips({ 8 });
	::gpiod::chip chip(mockup::instance().chip_path(0));
	::gpiod::line_request config;

	config.consumer = consumer.c_str();

	SECTION("get value (single line)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_INPUT;
		line.request(config);
		REQUIRE(line.get_value() == 0);
		mockup::instance().chip_set_pull(0, 3, 1);
		REQUIRE(line.get_value() == 1);
	}

	SECTION("set value (single line)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		line.request(config);
		line.set_value(1);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 1);
		line.set_value(0);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 0);
	}

	SECTION("set value with default value parameter")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		line.request(config, 1);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 1);
	}

	SECTION("get multiple values at once")
	{
		auto lines = chip.get_lines({ 0, 1, 2, 3, 4 });
		config.request_type = ::gpiod::line_request::DIRECTION_INPUT;
		lines.request(config);
		REQUIRE(lines.get_values() == ::std::vector<int>({ 0, 0, 0, 0, 0 }));
		mockup::instance().chip_set_pull(0, 1, 1);
		mockup::instance().chip_set_pull(0, 3, 1);
		mockup::instance().chip_set_pull(0, 4, 1);
		REQUIRE(lines.get_values() == ::std::vector<int>({ 0, 1, 0, 1, 1 }));
	}

	SECTION("set multiple values at once")
	{
		auto lines = chip.get_lines({ 0, 1, 2, 6, 7 });
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		lines.request(config);
		lines.set_values({ 1, 1, 0, 1, 0 });
		REQUIRE(mockup::instance().chip_get_value(0, 0) == 1);
		REQUIRE(mockup::instance().chip_get_value(0, 1) == 1);
		REQUIRE(mockup::instance().chip_get_value(0, 2) == 0);
		REQUIRE(mockup::instance().chip_get_value(0, 6) == 1);
		REQUIRE(mockup::instance().chip_get_value(0, 7) == 0);
	}

	SECTION("set multiple values with default values parameter")
	{
		auto lines = chip.get_lines({ 1, 2, 4, 6, 7 });
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		lines.request(config, { 1, 1, 0, 1, 0 });
		REQUIRE(mockup::instance().chip_get_value(0, 1) == 1);
		REQUIRE(mockup::instance().chip_get_value(0, 2) == 1);
		REQUIRE(mockup::instance().chip_get_value(0, 4) == 0);
		REQUIRE(mockup::instance().chip_get_value(0, 6) == 1);
		REQUIRE(mockup::instance().chip_get_value(0, 7) == 0);
	}

	SECTION("get value (single line, active-low")
	{
		auto line = chip.get_line(4);
		config.request_type = ::gpiod::line_request::DIRECTION_INPUT;
		config.flags = ::gpiod::line_request::FLAG_ACTIVE_LOW;
		line.request(config);
		REQUIRE(line.get_value() == 1);
		mockup::instance().chip_set_pull(0, 4, 1);
		REQUIRE(line.get_value() == 0);
	}

	SECTION("set value (single line, active-low)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = ::gpiod::line_request::FLAG_ACTIVE_LOW;
		line.request(config);
		line.set_value(1);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 0);
		line.set_value(0);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 1);
	}
}

TEST_CASE("Line can be reconfigured", "[line]")
{
	mockup::probe_guard mockup_chips({ 8 });
	::gpiod::chip chip(mockup::instance().chip_path(0));
	::gpiod::line_request config;

	config.consumer = consumer.c_str();

	SECTION("set config (single line, active-state)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_INPUT;
		config.flags = 0;
		line.request(config);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_INPUT);
		REQUIRE_FALSE(line.is_active_low());

		line.set_config(::gpiod::line_request::DIRECTION_OUTPUT,
			::gpiod::line_request::FLAG_ACTIVE_LOW,1);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.is_active_low());
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 0);
		line.set_value(0);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 1);

		line.set_config(::gpiod::line_request::DIRECTION_OUTPUT, 0);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE_FALSE(line.is_active_low());
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 0);
		line.set_value(1);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 1);
	}

	SECTION("set flags (single line, active-state)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = 0;
		line.request(config,1);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 1);

		line.set_flags(::gpiod::line_request::FLAG_ACTIVE_LOW);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.is_active_low());
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 0);

		line.set_flags(0);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE_FALSE(line.is_active_low());
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 1);
	}

	SECTION("set flags (single line, drive)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = 0;
		line.request(config);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);

		line.set_flags(::gpiod::line_request::FLAG_OPEN_DRAIN);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_OPEN_DRAIN);

		line.set_flags(::gpiod::line_request::FLAG_OPEN_SOURCE);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_OPEN_SOURCE);

		line.set_flags(0);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);
	}

	SECTION("set flags (single line, bias)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = 0;
		line.request(config);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);

		line.set_flags(::gpiod::line_request::FLAG_OPEN_DRAIN);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_OPEN_DRAIN);

		line.set_flags(::gpiod::line_request::FLAG_OPEN_SOURCE);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_OPEN_SOURCE);

		line.set_flags(0);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(line.drive() == ::gpiod::line::DRIVE_PUSH_PULL);
	}

	SECTION("set direction input (single line)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_OUTPUT;
		config.flags = 0;
		line.request(config);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		line.set_direction_input();
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_INPUT);
	}

	SECTION("set direction output (single line)")
	{
		auto line = chip.get_line(3);
		config.request_type = ::gpiod::line_request::DIRECTION_INPUT;
		config.flags = 0;
		line.request(config);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_INPUT);
		line.set_direction_output(1);
		REQUIRE(line.direction() == ::gpiod::line::DIRECTION_OUTPUT);
		REQUIRE(mockup::instance().chip_get_value(0, 3) == 1);
	}
}

TEST_CASE("Exported line can be released", "[line]")
{
	mockup::probe_guard mockup_chips({ 8 });
	::gpiod::chip chip(mockup::instance().chip_path(0));
	auto line = chip.get_line(4);
	::gpiod::line_request config;

	config.consumer = consumer.c_str();
	config.request_type = ::gpiod::line_request::DIRECTION_INPUT;

	line.request(config);

	REQUIRE(line.get_value() == 0);

	line.release();

	REQUIRE_THROWS_AS(line.get_value(), ::std::system_error);
}

TEST_CASE("Uninitialized GPIO line behaves correctly", "[line]")
{
	::gpiod::line line;

	SECTION("uninitialized line is 'false'")
	{
		REQUIRE_FALSE(line);
	}

	SECTION("using uninitialized line throws logic_error")
	{
		REQUIRE_THROWS_AS(line.name(), ::std::logic_error);
	}
}

TEST_CASE("Uninitialized GPIO line_bulk behaves correctly", "[line][bulk]")
{
	::gpiod::line_bulk bulk;

	SECTION("uninitialized line_bulk is 'false'")
	{
		REQUIRE_FALSE(bulk);
	}

	SECTION("using uninitialized line_bulk throws logic_error")
	{
		REQUIRE_THROWS_AS(bulk.get(0), ::std::logic_error);
	}
}

TEST_CASE("Cannot request the same line twice", "[line]")
{
	mockup::probe_guard mockup_chips({ 8 });
	::gpiod::chip chip(mockup::instance().chip_path(0));
	::gpiod::line_request config;

	config.consumer = consumer.c_str();
	config.request_type = ::gpiod::line_request::DIRECTION_INPUT;

	SECTION("two separate calls to request()")
	{
		auto line = chip.get_line(3);

		REQUIRE_NOTHROW(line.request(config));
		REQUIRE_THROWS_AS(line.request(config), ::std::system_error);
	}

	SECTION("request the same line twice in line_bulk")
	{
		/*
		 * While a line_bulk object can hold two or more line objects
		 * representing the same line - requesting it will fail.
		 */
		auto lines = chip.get_lines({ 2, 3, 4, 4 });

		REQUIRE_THROWS_AS(lines.request(config), ::std::system_error);
	}
}

TEST_CASE("Cannot get/set values of unrequested lines", "[line]")
{
	mockup::probe_guard mockup_chips({ 8 });
	::gpiod::chip chip(mockup::instance().chip_path(0));
	auto line = chip.get_line(3);

	SECTION("get value")
	{
		REQUIRE_THROWS_AS(line.get_value(), ::std::system_error);
	}

	SECTION("set value")
	{
		REQUIRE_THROWS_AS(line.set_value(1), ::std::system_error);
	}
}

TEST_CASE("Line objects can be compared")
{
	mockup::probe_guard mockup_chips({ 8 });
	::gpiod::chip chip(mockup::instance().chip_path(0));
	auto line1 = chip.get_line(3);
	auto line2 = chip.get_line(3);
	auto line3 = chip.get_line(4);

	REQUIRE(line1 == line2);
	REQUIRE(line2 != line3);
}
