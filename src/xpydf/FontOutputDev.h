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
  TextPageFont(TextOutputControl *controlA);

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

protected:
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
