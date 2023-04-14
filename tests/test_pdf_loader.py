import unittest
from pathlib import Path

from xpydf.pdf_loader import PdfLoader

DATA = Path(__file__).parent / "test_data"


class TestPdfLoader(unittest.TestCase):
    def test_non_existent_file(self):
        with self.assertRaises(IOError):
            PdfLoader(str(DATA / "missing.pdf"))

    def test_text_extraction(self):
        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"))

        text = loader.extract_strings()

        self.assertEqual(1, len(text))

        lines = text[0].split("\n")
        self.assertEqual(6, len(lines))
        self.assertEqual("XPyDF Testing", lines[0])

    def test_page_info(self):
        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"))
        page_info = loader.extract_images()

        self.assertEqual(1, len(page_info))

        page = page_info[0]
        self.assertEqual(1, page["page_number"])
        self.assertEqual(612, page["width"])
        self.assertEqual(792, page["height"])

    def test_image_extraction(self):
        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"))
        page_info = loader.extract_images()
        images = page_info[0]["images"]

        self.assertEqual(3, len(images))

        self.assertEqual(100, images[0]["width"])
        self.assertEqual(100, images[0]["height"])

        self.assertEqual(50, images[1]["width"])
        self.assertEqual(50, images[1]["height"])

        self.assertEqual(200, images[2]["width"])
        self.assertEqual(200, images[2]["height"])
    
    def test_modes(self):
        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"), mode="table")
        text = loader.extract_strings()
        self.assertEqual(1, len(text))

        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"), mode="simple")
        text = loader.extract_strings()
        self.assertEqual(1, len(text))

        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"), mode="lineprinter")
        text = loader.extract_strings()
        self.assertEqual(1, len(text))

        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"), mode="physical")
        text = loader.extract_strings()
        self.assertEqual(1, len(text))
    
    def test_password(self):
        with self.assertRaises(OSError):
            PdfLoader(str(DATA / "xpdf_tests_password.pdf"))
        
        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"), owner_password="ownerpassword")
        text = loader.extract_strings()
        self.assertEqual(1, len(text))
        
        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"), user_password="userpassword")
        text = loader.extract_strings()
        self.assertEqual(1, len(text))
        
        loader = PdfLoader(str(DATA / "xpdf_tests.pdf"), owner_password="ownerpassword", user_password="userpassword")
        text = loader.extract_strings()
        self.assertEqual(1, len(text))
            
