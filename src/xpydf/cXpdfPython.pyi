
from typing import Any, Dict, List, Optional, Tuple, TypedDict
import numpy.typing as npt

from xpydf.pdf_loader import PageInfo

class XpdfPythonCapsule: ...

class Font(TypedDict):
    name: str
    type: str
    size: str

def construct(
    filename: str,
    cliptext: bool,
    discard_diag: bool,
    discard_rotated_text: bool,
    verbose: bool,
    quiet: bool,
    mapNumericCharNames: bool = False,
    mapUnknownCharNames: bool = True,
    readUnicodeCMap: bool = True,
    mode: int = 0,
    ownerPw: Optional[str] = None,
    userPw: Optional[str] = None,
) -> XpdfPythonCapsule: ...
def extractText(capsule: XpdfPythonCapsule) -> List[bytes]: ...
def extractFontMap(capsule: XpdfPythonCapsule) -> Tuple[List[bytes], Dict[int, Font]]: ...
def extractPageInfo(capsule: XpdfPythonCapsule) -> List[PageInfo]: ...
def extractFonts(capsule: XpdfPythonCapsule) -> List[str]: ...
def extractImages(capsule: XpdfPythonCapsule, page_number: int) -> List[npt.NDArray[Any]]: ...
def pageToImage(capsule: XpdfPythonCapsule, page_number: int, dpi: int) -> npt.NDArray[Any]: ...
def deleteObject(capsule: XpdfPythonCapsule) -> None: ...
