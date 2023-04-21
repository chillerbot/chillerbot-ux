use super::CFGFLAG_CLIENT;
use super::CFGFLAG_SERVER;
use ddnet_base::s;
use ddnet_base::UserPtr;
use ddnet_engine::gs_ConsoleDefaultColor;
use ddnet_engine::IConsole;
use ddnet_engine::IConsole_FCommandCallback;
use ddnet_engine::IConsole_IResult;
use ddnet_engine::IConsole_OUTPUT_LEVEL_STANDARD;
use std::pin::Pin;
use std::ffi::CStr;
use std::os::raw::c_char;

extern crate unicode_width;

use unicode_width::UnicodeWidthStr;

#[cxx::bridge]
mod ffi {
    extern "C++" {
        include!("base/rust.h");
        include!("engine/console.h");

        type IConsole = ddnet_engine::IConsole;
    }
    extern "Rust" {
        unsafe fn str_width_unicode(text: *const c_char) -> i32;
    }
}

/// str_width_unicode
#[allow(non_snake_case)]
pub fn str_width_unicode(text: *const c_char) -> i32 {
    // println!("helo rust gamingers");
    let slice = unsafe { CStr::from_ptr(text) };
    // let slice = slice.to_str().unwrap_or_default();
    let slice = slice.to_str().unwrap();
    // println!("rust got str: {}", gaminger);
    let width = UnicodeWidthStr::width(slice) as i32; // .width_cjk().try_into().unwrap();
    return width;
}
