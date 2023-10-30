
from typing import Any, List, Optional
import numpy.typing as npt

from xpydf.pdf_loader import PageInfo

class XpdfPythonCapsule: ...

def construct(
    filename: str,
    cliptext: bool,
    discard_diag: bool,
    discard_rotated_text: bool,
    verbose: bool,
    quiet: bool,
    mode: int,
    mapNumericCharNames: bool = False,
    mapUnknownCharNames: bool = True,
    ownerPw: Optional[str] = None,
    userPw: Optional[str] = None,
) -> XpdfPythonCapsule: ...
def extractText(capsule: XpdfPythonCapsule) -> List[bytes]: ...
def extractPageInfo(capsule: XpdfPythonCapsule) -> List[PageInfo]: ...
def extractFonts(capsule: XpdfPythonCapsule) -> List[str]: ...
def extractImages(capsule: XpdfPythonCapsule, page_number: int) -> List[npt.NDArray[Any]]: ...
def pageToImage(capsule: XpdfPythonCapsule, page_number: int, dpi: int) -> npt.NDArray[Any]: ...
def deleteObject(capsule: XpdfPythonCapsule) -> None: ...
