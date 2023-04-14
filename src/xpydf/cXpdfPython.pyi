from typing import List, Optional

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
    ownerPw: Optional[str] = None,
    userPw: Optional[str] = None,
) -> XpdfPythonCapsule: ...
def extractText(capsule: XpdfPythonCapsule) -> List[bytes]: ...
def extractImages(capsule: XpdfPythonCapsule) -> List[PageInfo]: ...
def deleteObject(capsule: XpdfPythonCapsule) -> None: ...
