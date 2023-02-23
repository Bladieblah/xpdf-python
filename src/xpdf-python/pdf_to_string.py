import cXpdfPython

class PdfToString:
    def __init__(self):
        self.pts_capsule = cXpdfPython.construct()

    def load_file(self, filename: str):
        return cXpdfPython.loadFile(filename)

    def __delete__(self):
        cTokenizer.deleteObject(self.tokenizer_capsule)
