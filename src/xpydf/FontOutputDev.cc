#include <math.h>

#include "GList.h"
#include "GlobalParams.h"
#include "UnicodeRemapping.h"
#include "UnicodeTypeTable.h"
#include "GfxState.h"

#include "FontOutputDev.h"


TextChar *TextPageFont::textCharType(Unicode cA, int charPosA, int charLenA,
  double xMinA, double yMinA, double xMaxA, double yMaxA,
  int rotA, GBool rotatedA, GBool clippedA, GBool invisibleA,
  TextFontInfo *fontA, double fontSizeA,
  double colorRA, double colorGA, double colorBA)
{
  GString *name = fontA->getFontName();
  Unicode fontId = 0;

  if (name) {
    char fontCode[1000], fontName[1000], fontType[1000];

    if (sscanf(name->getCString(), "%[^+]+%[^-]-%s", fontCode, fontName, fontType) != EOF) {
      if (fontNameIds.find(fontName) == fontNameIds.end()) {
        fontNameIds[fontName] = fontNameIds.size();
      }

      if (fontTypeIds.find(fontType) == fontTypeIds.end()) {
        fontTypeIds[fontType] = fontTypeIds.size();
      }

      FontSpec spec = {fontNameIds[fontName], fontTypeIds[fontType], fontSizeA};

      if (fontIds.find(spec) == fontIds.end()) {
        fontIds[spec] = fontIds.size() + 1;
      }

      fontId = fontIds[spec];
    }
  }

  return new TextChar(fontId, charPosA, charLenA, xMinA, yMinA, xMaxA, yMaxA,
    rotA, rotatedA, clippedA, invisibleA, fontA, fontSizeA,
    colorRA, colorGA, colorBA);
}
