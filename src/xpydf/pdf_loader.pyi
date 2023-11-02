from typing import Any, Dict, List, Optional, Tuple, TypedDict

import numpy.typing as npt

class XpdfPythonCapsule: ...

class ImageInfo(TypedDict):
    width: float
    height: float

class PageInfo(TypedDict):
    page_number: int
    width: float
    height: float
    images: List[ImageInfo]

class Font(TypedDict):
    name: str
    type: str
    size: str

class PdfLoader:
    filename: str
    capsule: Optional[XpdfPythonCapsule] = None

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
        map_numeric_char_names: bool = False,
        map_unknown_char_names: bool = True,
        read_unicode_cmap: bool = True,
        mode: str = "table",
        owner_password: Optional[str] = None,
        user_password: Optional[str] = None,
    ) -> None: ...
    def extract_bytes(self) -> List[bytes]: ...
    def extract_font_map(self) -> Tuple[List[bytes], Dict[int, Font]]: ...
    def extract_strings(self) -> List[str]: ...
    def extract_page_info(self) -> List[PageInfo]: ...
    def extract_fonts(self) -> List[str]: ...
    def extract_images(self, page_number: int) -> List[npt.NDArray[Any]]: ...
    def page_to_image(self, page_number: int, dpi: int = 150) -> npt.NDArray[Any]: ...
    def __del__(self) -> None: ...
