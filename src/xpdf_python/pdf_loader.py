from typing import List, TypedDict

import cXpdfPython


class ImageInfo(TypedDict):
    width: float
    height: float


class PageInfo(TypedDict):
    page_number: int
    width: float
    height: float
    images: List[ImageInfo]


class PdfLoader:
    filename: str

    def __init__(
        self, 
        filename: str,
        cliptext: bool = False,
        discard_diag: bool = True,
        discard_rotated_text: bool = True,
        no_page_breaks: bool = False,
        insert_bom: bool = False,
        verbose: bool = False,
        quiet: bool = True,
    ):
        self.filename = filename
        self.capsule = cXpdfPython.construct(
            filename,
            cliptext,
            discard_diag,
            discard_rotated_text,
            no_page_breaks,
            insert_bom,
            verbose,
            quiet
        )

    def extract_bytes(self):
        pages: List[bytes]
        pages = cXpdfPython.extractText(self.capsule)
        return pages

    def extract_strings(self):
        pages: List[bytes] = self.extract_bytes()
        return [page.decode("unicode_escape", "replace") for page in pages]

    def extract_images(self):
        images: List[PageInfo]
        images = cXpdfPython.extractImages(self.capsule)
        return images

    def __del__(self):
        cXpdfPython.deleteObject(self.capsule)
