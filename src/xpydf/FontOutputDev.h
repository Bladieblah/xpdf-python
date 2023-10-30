#ifndef FONT_OUTPUT_DEV_H
#define FONT_OUTPUT_DEV_H

#include <map>
#include <string>

#include "TextOutputDev.h"

class TextPageFont: public TextPage {
public:
  TextPageFont(TextOutputControl *controlA) : TextPage(controlA) { };

protected:
  TextChar *textCharType(Unicode cA, int charPosA, int charLenA,
    double xMinA, double yMinA, double xMaxA, double yMaxA,
    int rotA, GBool rotatedA, GBool clippedA, GBool invisibleA,
    TextFontInfo *fontA, double fontSizeA,
    double colorRA, double colorGA, double colorBA);
private:
  std::map<std::string, unsigned int> fontNameIds;
  std::map<std::string, unsigned int> fontTypeIds;
  std::map<FontSpec, unsigned int> fontIds;
};

typedef struct FontSpec {
  unsigned int fontNameId, fontTypeId;
  int fontSize;
} FontSpec;

class FontOutputDev: public TextOutputDev {
public:
  FontOutputDev(TextOutputFunc func, void *stream,
    TextOutputControl *controlA) : TextOutputDev(func, stream, controlA) {
    delete text;
    text = new TextPageFont(&control);
  }
};

#endif
