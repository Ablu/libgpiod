// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * This file is part of libgpiod.
 *
 * Copyright (C) 2017-2018 Bartosz Golaszewski <bartekgola@gmail.com>
 */

/* C++ reimplementation of the gpiodetect tool. */

#include <gpiod.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 1) {
		::std::cerr << "usage: " << argv[0] << ::std::endl;
		return EXIT_FAILURE;
	}

	for (const auto& entry: ::std::filesystem::directory_iterator("/dev/")) {
		if (::gpiod::is_gpiochip_device(entry.path())) {
			::gpiod::chip chip(entry.path());

			::std::cout << chip.name() << " ["
				    << chip.label() << "] ("
				    << chip.num_lines() << " lines)" << ::std::endl;
		}
	}

	return EXIT_SUCCESS;
}
