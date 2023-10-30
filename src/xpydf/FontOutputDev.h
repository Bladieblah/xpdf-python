#ifndef FONT_OUTPUT_DEV_H
#define FONT_OUTPUT_DEV_H

#include <map>
#include <string>

#include "UnicodeMap.h"
#include "TextOutputDev.h"

typedef struct FontSpec {
  unsigned int fontNameId, fontTypeId, fontSize;
} FontSpec;

bool operator<(const FontSpec& l, const FontSpec& r);
bool operator==(const FontSpec& l, const FontSpec& r);
bool operator!=(const FontSpec& l, const FontSpec& r);

class TextPageFont: public TextPage {
public:
  TextPageFont(TextOutputControl *controlA) : TextPage(controlA) {fprintf(stderr, "Making TextPageFont\n");};

  std::map<FontSpec, unsigned int> getFontIds() {
    return fontIds;
  }

// protected:
  TextChar *textCharType(Unicode cA, int charPosA, int charLenA,
    double xMinA, double yMinA, double xMaxA, double yMaxA,
    int rotA, GBool rotatedA, GBool clippedA, GBool invisibleA,
    TextFontInfo *fontA, double fontSizeA,
    double colorRA, double colorGA, double colorBA) override;

  void encodeFragment(Unicode *text, int len, UnicodeMap *uMap, GBool primaryLR, GString *s) override;

private:
  std::map<std::string, unsigned int> fontNameIds;
  std::map<std::string, unsigned int> fontTypeIds;
  std::map<FontSpec, unsigned int> fontIds;
};

class FontOutputDev: public TextOutputDev {
public:
  FontOutputDev(TextOutputFunc func, void *stream,
    TextOutputControl *controlA) : TextOutputDev(func, stream, controlA) {
    delete text;
    text = new TextPageFont(&control);
  }

  std::map<FontSpec, unsigned int> getFontIds() {
    return text->getFontIds();
  }

protected:
  TextPageFont *text;
};

#endif
