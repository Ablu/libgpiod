# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileCopyrightText: 2022 Bartosz Golaszewski <brgl@bgdev.pl>

from os import environ
from setuptools import setup, Extension, find_packages

gpiod_ext = Extension(
    "gpiod._ext",
    sources=[
        "gpiod/ext/chip.c",
        "gpiod/ext/common.c",
        "gpiod/ext/line-config.c",
        "gpiod/ext/line-settings.c",
        "gpiod/ext/module.c",
        "gpiod/ext/request.c",
    ],
    define_macros=[("_GNU_SOURCE", "1")],
    libraries=["gpiod"],
    extra_compile_args=["-Wall", "-Wextra"],
)

gpiosim_ext = Extension(
    "tests.gpiosim._ext",
    sources=["tests/gpiosim/ext.c"],
    define_macros=[("_GNU_SOURCE", "1")],
    libraries=["gpiosim"],
    extra_compile_args=["-Wall", "-Wextra"],
)

procname_ext = Extension(
    "tests.procname._ext",
    sources=["tests/procname/ext.c"],
    define_macros=[("_GNU_SOURCE", "1")],
    extra_compile_args=["-Wall", "-Wextra"],
)

extensions = [gpiod_ext]
if "GPIOD_WITH_TESTS" in environ and environ["GPIOD_WITH_TESTS"] == "1":
    extensions.append(gpiosim_ext)
    extensions.append(procname_ext)

with open("gpiod/version.py", "r") as fd:
    exec(fd.read())

setup(
    name="libgpiod",
    packages=find_packages(exclude=["tests", "tests.*"]),
    ext_modules=extensions,
    version=__version__,
    author="Bartosz Golaszewski",
    author_email="brgl@bgdev.pl",
    description="Python bindings for libgpiod",
    long_description="This is a package spun out of the main libgpiod repository containing " \
                     "python bindings to the underlying C library.",
    platforms=["linux"],
    license="LGPLv2.1",
)
