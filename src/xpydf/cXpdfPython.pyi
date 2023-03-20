from typing import List

from xpydf.pdf_loader import PageInfo

class XpdfPythonCapsule: ...

def construct(
    filename: str,
    cliptext: bool,
    discard_diag: bool,
    discard_rotated_text: bool,
    verbose: bool,
    quiet: bool,
) -> XpdfPythonCapsule: ...
def extractText(capsule: XpdfPythonCapsule) -> List[bytes]: ...
def extractImages(capsule: XpdfPythonCapsule) -> List[PageInfo]: ...
def deleteObject(capsule: XpdfPythonCapsule) -> None: ...
