import mmap
import numpy as np
import pytest
import os
import unittest

PATH = os.path.join(os.sep, 'dev', 'nan64')

MANY_PAGES = 1024 * 4 * 20


@unittest.skipUnless(os.path.exists(PATH), reason="need the device to mmap")
class TestNaN(unittest.TestCase):

    def _load(self, *shape):
        with open(PATH, 'rb') as f:
            mm = mmap.mmap(f.fileno(),
                           8 * np.prod(shape),
                           access=mmap.ACCESS_COPY)
            return np.ndarray(shape=shape, dtype=float, buffer=mm)

    def test_read(self):
        arr = self._load(MANY_PAGES)
        assert np.isnan(arr).all()

    def test_write(self):
        a = self._load(MANY_PAGES)
        b = self._load(MANY_PAGES)
        expected = np.arange(MANY_PAGES, dtype=float)

        a[0::2] = expected[0::2]
        b[1::2] = expected[1::2]

        np.testing.assert_allclose(np.nansum((a, b), axis=0), expected)

        c = self._load(MANY_PAGES)
        assert np.isnan(c).all(), "didn't break the driver's page!"

    def test_bad(self, shape=1):
        # no polluting the template NaN page
        with open(PATH, 'rb') as f:
            with pytest.raises(PermissionError):
                mm = mmap.mmap(f.fileno(), 8 * shape, access=mmap.ACCESS_WRITE)
