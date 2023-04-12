from typing import List, TypedDict

import cXpdfPython


class ImageInfo(TypedDict):
    """Container for image metadata

    Attributes
    ----------
    width : float
        Width of the image in points
    height : float
        Height of the image in points
    """

    width: float
    height: float


class PageInfo(TypedDict):
    """Container for page metadata

    Attributes
    ----------
    page_number : int
        The page number
    width : float
        Width of the page in points
    height : float
        Height of the page in points
    images : List[ImageInfo]
        A list of images
    """

    page_number: int
    width: float
    height: float
    images: List[ImageInfo]


xpdf_modes = {
    "table": 0,
    "simple": 1,
    "lineprinter": 2,
    "physical": 3,
}


class PdfLoader:
    filename: str
    capsule = None

    def __init__(
        self,
        filename: str,
        cliptext: bool = False,
        discard_diag: bool = True,
        discard_rotated_text: bool = True,
        verbose: bool = False,
        quiet: bool = True,
        mode: str = "table",
    ):
        """Load a file for extraction.

        Parameters
        ----------
        filename : str
            Path to the pdf to load
        cliptext : bool, optional
            _description_, by default False
        discard_diag : bool, optional
            Remove diagonal text, like watermarks, by default True
        discard_rotated_text : bool, optional
            Remove vertical text, by default True
        verbose : bool, optional
            Print per-page status information, by default False
        quiet : bool, optional
            Don't print any messages or errors, by default True
        mode : str, optional
            Select the xpdf parsing mode, choices are:
             - table (default): physical layout optimised for tables
             - simple: simple one-column physical layout
             - lineprinter: strict fixed-pitch/height layout
             - physical: maintain original physical layout

        Raises
        ------
        IOError
            When the file is not found, or cannot be loaded
        """
        if mode not in xpdf_modes:
            xpdf_mode = 0
        else:
            xpdf_mode = xpdf_modes[mode]

        self.filename = filename
        self.capsule = cXpdfPython.construct(filename, cliptext, discard_diag, discard_rotated_text, verbose, quiet, xpdf_mode)

    def extract_bytes(self) -> List[bytes]:
        """Extract encoded text from the pdf

        Returns
        -------
        List[bytes]
            The text of each page
        """
        pages: List[bytes] = []
        if self.capsule is not None:
            pages = cXpdfPython.extractText(self.capsule)

        return pages

    def extract_strings(self) -> List[str]:
        """Extract and decode text from the pdf

        Returns
        -------
        List[str]
            The text of each page, decoded as unicode
        """
        pages = self.extract_bytes()
        return [page.decode("unicode_escape", "replace") for page in pages]

    def extract_images(self) -> List[PageInfo]:
        """Return image related metadata from the pdf

        Returns
        -------
        List[PageInfo]
            A PageInfo object for each page
        """
        images: List[PageInfo] = []
        if self.capsule is not None:
            images = cXpdfPython.extractImages(self.capsule)

        return images

    def __del__(self):
        if self.capsule:
            cXpdfPython.deleteObject(self.capsule)
            del self.capsule
