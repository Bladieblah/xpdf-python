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

  // fprintf(stderr, "Char [%c] font [%s] size [%.0f] color [%.1f, %.1f, %.1f]\n", buf[0], name->getCString(), fontSizeA, colorRA, colorGA, colorBA);
  
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

  // fprintf(stderr, "fontId %d >> ", fontId);

  fontId = fontId ^ (n << 16);

  // fprintf(stderr, "%d (%d , %d)\n", fontId, (char)fontId, (fontId >> 16));

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

  // fprintf(stderr, "Encoded %d\n", s->getLength());
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

  // fprintf(stderr, "Line length %d // %s\n", line->pw, line->text);
}
