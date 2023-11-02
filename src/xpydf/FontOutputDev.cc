#include <math.h>

#include "Error.h"
#include "GList.h"
#include "GlobalParams.h"
#include "UnicodeMap.h"
#include "UnicodeRemapping.h"
#include "UnicodeTypeTable.h"
#include "GfxState.h"

#include "FontOutputDev.h"


bool operator<(const FontSpec& l, const FontSpec& r) {
  return (
    l.fontNameId < r.fontNameId
    || (l.fontNameId == r.fontNameId && l.fontTypeId < r.fontTypeId)
    || (l.fontNameId == r.fontNameId && l.fontTypeId == r.fontTypeId && l.fontSize < r.fontSize)
  );
}

bool operator==(const FontSpec& l, const FontSpec& r) {
  return (l.fontNameId == r.fontNameId && l.fontTypeId == r.fontTypeId && l.fontSize == r.fontSize);
}

bool operator!=(const FontSpec& l, const FontSpec& r) {
  return (l.fontNameId != r.fontNameId || l.fontTypeId != r.fontTypeId || l.fontSize != r.fontSize);
}

TextPageFont::TextPageFont(TextOutputControl *controlA) : TextPage(controlA) {
  if (!(uMap = globalParams->getTextEncoding())) {
    fprintf(stderr, "WARNING: Encoding not found");
    return;
  }

  spaceLen = uMap->mapUnicode(0x20, space, sizeof(space));
  eolLen = 0; // make gcc happy

  switch (globalParams->getTextEOL()) {
  case eolUnix:
    eolLen = uMap->mapUnicode(0x0a, eol, sizeof(eol));
    break;
  case eolDOS:
    eolLen = uMap->mapUnicode(0x0d, eol, sizeof(eol));
    eolLen += uMap->mapUnicode(0x0a, eol + eolLen, (int)sizeof(eol) - eolLen);
    break;
  case eolMac:
    eolLen = uMap->mapUnicode(0x0d, eol, sizeof(eol));
    break;
  }

  if (eolLen != 1 || spaceLen != 1) {
    fprintf(stderr, "WARNING: Unexpected delimiter lengths: [spacelen] = %d, [eollen] = %d\n", spaceLen, eolLen);
  }

  fontNameIds["__space__"] = -1U;
  fontTypeIds["__space__"] = -1U;
  FontSpec dummy = {-1U, -1U, 0};
  fontSpecIds[dummy] = space[0];

  fontNameIds["__eol__"] = -2U;
  fontTypeIds["__eol__"] = -2U;
  dummy = (FontSpec){-2U, -2U, 0};
  fontSpecIds[dummy] = eol[0];

  fontNameIds["__invalid__"] = -3U;
  fontTypeIds["__invalid__"] = -3U;
  dummy = (FontSpec){-3U, -3U, 0};
  fontSpecIds[dummy] = FONT_INVALID;

  fontNameIds["__unknown__"] = -4U;
  fontTypeIds["__unknown__"] = -4U;
  dummy = (FontSpec){-4U, -4U, 0};
  fontSpecIds[dummy] = FONT_UNKNOWN;


  for(int i = 0; i <= 253; i++) {
    if (i != space[0] && i != eol[0] && i != FONT_UNKNOWN) {
      availableIds.push(i);
    }
  };
};

TextChar *TextPageFont::textCharType(Unicode cA, int charPosA, int charLenA,
  double xMinA, double yMinA, double xMaxA, double yMaxA,
  int rotA, GBool rotatedA, GBool clippedA, GBool invisibleA,
  TextFontInfo *fontA, double fontSizeA,
  double colorRA, double colorGA, double colorBA)
{
  GString *name = fontA->getFontName();
  Unicode fontId = FONT_UNKNOWN;

  char buf[8];
  int n = uMap->mapUnicode(cA, buf, sizeof(buf));
  
  if (n == 0) {
    fontId = FONT_INVALID;
  }
  else if (name) {
    char fontCode[1000], fontName[1000], fontType[1000];

    if (sscanf(name->getCString(), "%[^+]+%[^-]-%s", fontCode, fontName, fontType) != EOF) {
      if (fontNameIds.find(fontName) == fontNameIds.end()) {
        fontNameIds[fontName] = fontNameIds.size();
      }

      if (fontTypeIds.find(fontType) == fontTypeIds.end()) {
        fontTypeIds[fontType] = fontTypeIds.size();
      }

      FontSpec spec = {fontNameIds[fontName], fontTypeIds[fontType], (unsigned int)fontSizeA};

      if (fontSpecIds.find(spec) == fontSpecIds.end()) {
        if (availableIds.size() == 0) {
          fprintf(stderr, "ERROR: Font id overflow\n");
          error(errInternal, -1, "ERROR: Font id overflow");
        }
        fontSpecIds[spec] = availableIds.front();
        availableIds.pop();
      }

      fontId = fontSpecIds[spec];
    }
  }

  fontId = fontId ^ (n << 16);

  return new TextChar(fontId, charPosA, charLenA, xMinA, yMinA, xMaxA, yMaxA,
    rotA, rotatedA, clippedA, invisibleA, fontA, fontSizeA,
    colorRA, colorGA, colorBA);
}

void TextPageFont::encodeFragment(Unicode *text, int len, UnicodeMap *uMap, GBool primaryLR, GString *s) {
  char buf[8];
  int n;

  for (int i = 0; i < len; ++i) {
    if (text[i] != FONT_INVALID) {
      n = (text[i] >> 16);
      if (n == 0) n = 1;
      buf[0] = (char)text[i];
      for (int j = 0; j < n; j++) {
        s->append(buf, 1);
      }
    }
  }
}

void TextPageFont::computeLinePhysWidth(TextLine *line, UnicodeMap *uMap) {
  int n;
  if (uMap->isUnicode()) {
    line->pw = line->len;
  } else {
    line->pw = 0;
    for (int i = 0; i < line->len; ++i) {
      if (line->text[i] != FONT_INVALID) {
        n = (line->text[i] >> 16);
        if (n == 0) n = 1;
        line->pw += n;
      }
    }
  }
}
