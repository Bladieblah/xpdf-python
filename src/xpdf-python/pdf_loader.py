import json
from typing import Any, Dict, List
import cXpdfPython

class PdfLoader:
    filename: str

    def __init__(self, filename: str):
        self.filename = filename
        self.pts_capsule = cXpdfPython.construct(filename)

    def extract_bytes(self):
        pages: List[bytes]
        self.pts_capsule, pages = cXpdfPython.extractText(self.pts_capsule)
        return pages

    def extract_strings(self):
        pages: List[bytes] = self.extract_bytes()
        return [page.decode('unicode_escape', 'replace') for page in pages]

    def extract_images(self):
        images: List[Dict[str, Any]]
        self.pts_capsule, images = cXpdfPython.extractImages(self.pts_capsule)
        return images

    def __delete__(self):
        cTokenizer.deleteObject(self.tokenizer_capsule)

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
