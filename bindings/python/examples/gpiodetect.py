#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later

#
# This file is part of libgpiod.
#
# Copyright (C) 2017-2018 Bartosz Golaszewski <bartekgola@gmail.com>
#

'''Reimplementation of the gpiodetect tool in Python.'''

import gpiod
import os

if __name__ == '__main__':
    for entry in os.scandir('/dev/'):
        if gpiod.is_gpiochip_device(entry.path):
            with gpiod.Chip(entry.path) as chip:
                print('{} [{}] ({} lines)'.format(chip.name(),
                                                  chip.label(),
                                                  chip.num_lines()))
