#ifndef FONT_OUTPUT_DEV_H
#define FONT_OUTPUT_DEV_H

#include <map>
#include <queue>
#include <string>

#include "UnicodeMap.h"
#include "TextOutputDev.h"

#define FONT_INVALID 256
#define FONT_UNKNOWN 255

typedef struct FontSpec {
  unsigned int fontNameId, fontTypeId, fontSize;
} FontSpec;

typedef struct NamedFontSpec {
  std::string fontName, fontType;
  unsigned int fontSize;
} NamedFontSpec;

bool operator<(const FontSpec& l, const FontSpec& r);
bool operator==(const FontSpec& l, const FontSpec& r);
bool operator!=(const FontSpec& l, const FontSpec& r);

class TextPageFont: public TextPage {
public:
  TextPageFont(TextOutputControl *controlA) : TextPage(controlA) {
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

  std::map<unsigned int, NamedFontSpec> getFontSpecs() {
    std::map<unsigned int, NamedFontSpec> result;

    std::map<unsigned int, std::string> fontNames;
    std::map<unsigned int, std::string> fontTypes;

    for (auto pair : fontNameIds) {
      fontNames[pair.second] = pair.first;
    }

    for (auto pair : fontTypeIds) {
      fontTypes[pair.second] = pair.first;
    }

    for (auto pair : fontSpecIds) {
      result[pair.second] = (NamedFontSpec) {
        fontNames[pair.first.fontNameId],
        fontTypes[pair.first.fontTypeId],
        pair.first.fontSize
      };
    }

    return result;
  }

// protected:
  TextChar *textCharType(Unicode cA, int charPosA, int charLenA,
    double xMinA, double yMinA, double xMaxA, double yMaxA,
    int rotA, GBool rotatedA, GBool clippedA, GBool invisibleA,
    TextFontInfo *fontA, double fontSizeA,
    double colorRA, double colorGA, double colorBA) override;
  
  void encodeFragment(Unicode *text, int len, UnicodeMap *uMap,
    GBool primaryLR, GString *s) override;
  
  void computeLinePhysWidth(TextLine *line, UnicodeMap *uMap) override;
private:
  std::map<std::string, unsigned int> fontNameIds;
  std::map<std::string, unsigned int> fontTypeIds;
  std::map<FontSpec, unsigned int> fontSpecIds;

  std::queue<unsigned int> availableIds;

  char space[8], eol[16];
  int spaceLen, eolLen;
  UnicodeMap *uMap;
};

class FontOutputDev: public TextOutputDev {
public:
  FontOutputDev(TextOutputFunc func, void *stream,
    TextOutputControl *controlA) : TextOutputDev(func, stream, controlA) {
    delete text;
    text = new TextPageFont(&control);
  }

  std::map<unsigned int, NamedFontSpec> getFontSpecs() {
    return ((TextPageFont *)text)->getFontSpecs();
  }
};

#endif
