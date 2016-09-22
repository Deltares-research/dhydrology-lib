import platform

import unittest

from dhydrology import DHydrology

class Test_dhydrology_tests(unittest.TestCase):
    DHydrology.LIBRARY_PATH = r'../../bin/x64/Debug/dhydrology.dll'

    def test_A(self):
        hyd = DHydrology()
        hyd.solve('1')
        hyd.finalize()

    def test_B(self):
        hyd = DHydrology()
        hyd.solve('2')
        hyd.finalize()

if __name__ == '__main__':
    unittest.main()
