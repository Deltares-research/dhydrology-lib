import os
import platform


import _ctypes
from ctypes import (
    cdll, c_double, c_int, c_char_p, c_bool, c_char, c_float, c_void_p, ARRAY, Structure, POINTER, byref, CFUNCTYPE
)

def load_library(path):
    """Return the native library, loaded with """
    library_dir = os.path.dirname(path)
    library_path = os.path.abspath(path)

    if platform.system() == 'Windows':
        olddir = os.getcwd()
        os.chdir(library_dir)

        import win32api
        win32api.SetDllDirectory('.')

    result = cdll.LoadLibrary(library_path)

    if platform.system() == 'Windows':
        os.chdir(olddir)

    return result

def unload_library(lib):
    _ctypes.FreeLibrary(lib._handle)    

class DHydrology:
    LIBRARY_PATH = None

    def __init__(self):
        self.lib = load_library(self.LIBRARY_PATH)

    def solve(self, p):
        self.lib.HELLO_DHYDRO(p)

    def finalize(self):
        unload_library(self.lib)
        
