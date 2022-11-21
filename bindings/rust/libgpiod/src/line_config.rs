// SPDX-License-Identifier: Apache-2.0 OR BSD-3-Clause
// SPDX-FileCopyrightText: 2022 Linaro Ltd.
// SPDX-FileCopyrightTest: 2022 Viresh Kumar <viresh.kumar@linaro.org>

use std::os::raw::{c_ulong, c_void};
use std::slice;

use super::{
    gpiod,
    line::{Offset, Settings},
    Error, OperationType, Result,
};

/// Line configuration objects.
///
/// The line-config object contains the configuration for lines that can be
/// used in two cases:
///  - when making a line request
///  - when reconfiguring a set of already requested lines.
///
/// A new line-config object is empty. Using it in a request will lead to an
/// error. In order for a line-config to become useful, it needs to be assigned
/// at least one offset-to-settings mapping by calling
/// ::gpiod_line_config_add_line_settings.
///
/// When calling ::gpiod_chip_request_lines, the library will request all
/// offsets that were assigned settings in the order that they were assigned.

#[derive(Debug, Eq, PartialEq)]
pub struct Config {
    pub(crate) config: *mut gpiod::gpiod_line_config,
}

impl Config {
    /// Create a new line config object.
    pub fn new() -> Result<Self> {
        // SAFETY: The `gpiod_line_config` returned by libgpiod is guaranteed to live as long
        // as the `struct Config`.
        let config = unsafe { gpiod::gpiod_line_config_new() };

        if config.is_null() {
            return Err(Error::OperationFailed(
                OperationType::LineConfigNew,
                errno::errno(),
            ));
        }

        Ok(Self { config })
    }

    /// Resets the entire configuration stored in the object. This is useful if
    /// the user wants to reuse the object without reallocating it.
    pub fn reset(&mut self) {
        // SAFETY: `gpiod_line_config` is guaranteed to be valid here.
        unsafe { gpiod::gpiod_line_config_reset(self.config) }
    }

    /// Add line settings for a set of offsets.
    pub fn add_line_settings(&self, offsets: &[Offset], settings: Settings) -> Result<()> {
        // SAFETY: `gpiod_line_config` is guaranteed to be valid here.
        let ret = unsafe {
            gpiod::gpiod_line_config_add_line_settings(
                self.config,
                offsets.as_ptr(),
                offsets.len() as c_ulong,
                settings.settings,
            )
        };

        if ret == -1 {
            Err(Error::OperationFailed(
                OperationType::LineConfigAddSettings,
                errno::errno(),
            ))
        } else {
            Ok(())
        }
    }

    /// Get line settings for offset.
    pub fn line_settings(&self, offset: Offset) -> Result<Settings> {
        // SAFETY: `gpiod_line_config` is guaranteed to be valid here.
        let settings = unsafe { gpiod::gpiod_line_config_get_line_settings(self.config, offset) };

        if settings.is_null() {
            return Err(Error::OperationFailed(
                OperationType::LineConfigGetSettings,
                errno::errno(),
            ));
        }

        Ok(Settings::new_with_settings(settings))
    }

    /// Get configured offsets.
    pub fn offsets(&self) -> Result<Vec<Offset>> {
        let mut num: u64 = 0;
        let mut ptr: *mut Offset = std::ptr::null_mut();

        // SAFETY: The `ptr` array returned by libgpiod is guaranteed to live as long
        // as it is not explicitly freed with `free()`.
        let ret = unsafe {
            gpiod::gpiod_line_config_get_offsets(
                self.config,
                &mut num as *mut _ as *mut _,
                &mut ptr,
            )
        };

        if ret == -1 {
            return Err(Error::OperationFailed(
                OperationType::LineConfigGetOffsets,
                errno::errno(),
            ));
        }

        // SAFETY: The `ptr` array returned by libgpiod is guaranteed to live as long
        // as it is not explicitly freed with `free()`.
        let offsets = unsafe { slice::from_raw_parts(ptr as *const Offset, num as usize).to_vec() };

        // SAFETY: The `ptr` array is guaranteed to be valid here.
        unsafe { libc::free(ptr as *mut c_void) };

        Ok(offsets)
    }
}

impl Drop for Config {
    /// Free the line config object and release all associated resources.
    fn drop(&mut self) {
        // SAFETY: `gpiod_line_config` is guaranteed to be valid here.
        unsafe { gpiod::gpiod_line_config_free(self.config) }
    }
}
