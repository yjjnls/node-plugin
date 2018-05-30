//
//
//
"use strict"
const os=require('os');
const path = require('path')
const fs = require('fs')

var ARCH = os.arch()
var PLATFORM = os.platform()





var Arch={
    X86    : 'x86',
    X86_64 : 'x86_64'
}

var Platform={
    DARWIN  : 'Darwin',
    LINUX   : 'Linux',
    WINDOWS : 'Windows'
}

var DynamicLibraryExt={
    DARWIN  : 'dylib',
    LINUX   : 'so',
    WINDOWS : 'dll'    
}

switch( os.platform() ){
    case 'win32':{
        Platform.current = Platform.WINDOWS;
        DynamicLibraryExt.current = DynamicLibraryExt.WINDOWS;
    }
    break;
    case 'linux':{
        Platform.current = Platform.LINUX;
        DynamicLibraryExt.current = DynamicLibraryExt.LINUX;
    }
    break;
    case 'darwin':{
        Platform.current = Platform.DARWIN;
        DynamicLibraryExt.current = DynamicLibraryExt.DARWIN;
    }
    break;
}

switch( os.arch() ){
    case 'ia32':
    Arch.current = Arch.X86;
    break;
    case 'x64':
    Arch.current = Arch.X86_64;
    break;
}

module.exports = {
    Arch: Arch,
    Platform: Platform,
    DynamicLibraryExt: DynamicLibraryExt
}