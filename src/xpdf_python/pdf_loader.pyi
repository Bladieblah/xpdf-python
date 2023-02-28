from typing import List, TypedDict

class XpdfPythonCapsule: ...

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
    capsule: XpdfPythonCapsule
    def __init__(self, filename: str) -> None: ...
    def extract_bytes(self) -> List[bytes]: ...
    def extract_strings(self) -> List[str]: ...
    def extract_images(self) -> List[PageInfo]: ...
    def __delete__(self) -> None: ...
