from ctypes import *


class Args(Structure):
    _fields_ = [
        ('data_file', c_char_p),
        ('output_file', c_char_p),
        ('pt', c_double),
        ('n_noise', c_int),
        ('n_track', c_int),
        ('particle', c_char_p),
        ('mode', c_int),
        ('deg', c_char_p)
    ]


Hough = cdll.LoadLibrary("libTracker.so")

run = Hough.run_hough
