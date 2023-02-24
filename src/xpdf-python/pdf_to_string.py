from typing import List
import cXpdfPython

class PdfToString:
    def __init__(self):
        self.pts_capsule = cXpdfPython.construct()

    def as_bytes(self, filename: str):
        pages: List[bytes]
        self.pts_capsule, pages = cXpdfPython.loadFile(self.pts_capsule, filename)
        return pages

    def as_strings(self, filename: str):
        pages: List[bytes] = self.as_bytes(filename)
        return [page.decode('unicode_escape', 'replace') for page in pages]

    def __delete__(self):
        cTokenizer.deleteObject(self.tokenizer_capsule)

if __name__ == "__main__":
    pts = PdfToString()
    path = "../NL-PDF-Extractietool/data/input/9999748983.pdf"
    result = pts.as_strings(path)
    print(len(result))

    for i, page in enumerate(result):
        print("----------------------------------------------------- %d -----------------------------------------------------" % (i))
        print(page)
