import json
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

    def __init__(self, filename: str):
        self.filename = filename
        self.capsule = cXpdfPython.construct(filename)

    def extract_bytes(self):
        pages: List[bytes]
        pages = cXpdfPython.extractText(self.capsule)
        return pages

    def extract_strings(self):
        pages: List[bytes] = self.extract_bytes()
        return [page.decode('unicode_escape', 'replace') for page in pages]

    def extract_images(self):
        images: List[PageInfo]
        images = cXpdfPython.extractImages(self.capsule)
        return images

    def __delete__(self):
        cXpdfPython.deleteObject(self.capsule)

if __name__ == "__main__":
    path = "../NL-PDF-Extractietool/data/input/9999748983.pdf"
    loader = PdfLoader(path)
    result = loader.extract_strings()
    print(len(result))

    for i, page in enumerate(result):
        print("----------------------------------------------------- %d -----------------------------------------------------" % (i))
        print(page)
    
    pages = loader.extract_images()
    print(json.dumps(pages, indent=4))
