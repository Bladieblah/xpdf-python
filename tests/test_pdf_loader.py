from pathlib import Path
import unittest

from xpdf_python.pdf_loader import PdfLoader

DATA = Path(__file__).parent / 'data'

class TestPdfLoader(unittest.TestCase):
    def test_non_existent_file(self):
        with self.assertRaises(IOError):
            PdfLoader(str(DATA / 'missing.pdf'))
