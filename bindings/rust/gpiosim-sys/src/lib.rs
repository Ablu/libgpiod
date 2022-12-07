// SPDX-License-Identifier: Apache-2.0 OR BSD-3-Clause
// SPDX-FileCopyrightText: 2022 Linaro Ltd.
// SPDX-FileCopyrightTest: 2022 Viresh Kumar <viresh.kumar@linaro.org>

use libgpiod::{Error, Result, OperationType};

#[allow(non_camel_case_types, non_upper_case_globals)]
#[cfg_attr(test, allow(deref_nullptr, non_snake_case))]
#[allow(dead_code)]
mod bindings_raw {
    include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
}
use bindings_raw::*;

mod sim;
pub use sim::*;

use crate::{
    gpiosim_value_GPIOSIM_VALUE_INACTIVE as GPIOSIM_VALUE_INACTIVE,
    gpiosim_value_GPIOSIM_VALUE_ACTIVE as GPIOSIM_VALUE_ACTIVE,
    gpiosim_value_GPIOSIM_VALUE_ERROR as GPIOSIM_VALUE_ERROR,
    gpiosim_direction_GPIOSIM_HOG_DIR_INPUT as GPIOSIM_HOG_DIR_INPUT,
    gpiosim_direction_GPIOSIM_HOG_DIR_OUTPUT_HIGH as GPIOSIM_HOG_DIR_OUTPUT_HIGH,
    gpiosim_direction_GPIOSIM_HOG_DIR_OUTPUT_LOW as GPIOSIM_HOG_DIR_OUTPUT_LOW,
    gpiosim_pull_GPIOSIM_PULL_UP as GPIOSIM_PULL_UP,
    gpiosim_pull_GPIOSIM_PULL_DOWN as GPIOSIM_PULL_DOWN,
};

/// Value settings.
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub enum Value {
    /// Active
    Active,
    /// Inactive
    InActive,
}

impl Value {
    pub(crate) fn new(val: gpiosim_value) -> Result<Self> {
        Ok(match val {
            GPIOSIM_VALUE_INACTIVE => Value::InActive,
            GPIOSIM_VALUE_ACTIVE => Value::Active,
            GPIOSIM_VALUE_ERROR => {
                return Err(Error::OperationFailed(
                    OperationType::SimBankGetVal, errno::errno()
                ))
            }
            _ => return Err(Error::InvalidEnumValue("Value", val as i32)),
        })
    }
}

/// Direction settings.
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub enum Direction {
    /// Direction is input - for reading the value of an externally driven GPIO line.
    Input,
    /// Direction is output - for driving the GPIO line, value is high.
    OutputHigh,
    /// Direction is output - for driving the GPIO line, value is low.
    OutputLow,
}

impl Direction {
    fn val(self) -> gpiosim_direction {
        match self {
            Direction::Input => GPIOSIM_HOG_DIR_INPUT,
            Direction::OutputHigh => GPIOSIM_HOG_DIR_OUTPUT_HIGH,
            Direction::OutputLow => GPIOSIM_HOG_DIR_OUTPUT_LOW,
        }
    }
}

/// Internal pull settings.
#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub enum Pull {
    /// The internal pull-up is enabled.
    Up,
    /// The internal pull-down is enabled.
    Down,
}

impl Pull {
    fn val(self) -> gpiosim_pull {
        match self {
            Pull::Up => GPIOSIM_PULL_UP,
            Pull::Down => GPIOSIM_PULL_DOWN,
        }
    }
}
