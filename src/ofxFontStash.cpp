#include "ofxFontStash.h"

#define FONTSTASH_IMPLEMENTATION
#define GLFONTSTASH_IMPLEMENTATION

extern "C" {
#include "fontstash.h"
#include "offontstash.h"
}

ofxFontStash::ofxFontStash(){
	fs = NULL;
}

ofxFontStash::~ofxFontStash(){
	if (fs != NULL) {
		fonsDeleteInternal(fs);
		fs = NULL;
	}
}

void ofxFontStash::load(const filesystem::path & filename, float fontsize) {
	bool bUseArb = ofGetUsingArbTex();
	ofDisableArbTex();

	fs = glfonsCreate(2048, 2048, FONS_ZERO_TOPLEFT);
	if (fs == NULL) {
		printf("Could not create stash.\n");
		return;
	}

	font = fonsAddFont(fs, "font", ofToDataPath(filename).c_str());
	if (font == FONS_INVALID) {
		printf("Could not add font normal.\n");
		return;
	}

	fonsClearState(fs);
	fonsSetFont(fs, font);
	fonsSetSize(fs, 1.6 * fontsize);
	fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
	
	//fonsSetColor(fs, white);
	//fonsSetSpacing(fs, 5.0f);
	//fonsSetBlur(fs, 10.0f);

	bUseArb ? ofEnableArbTex() : ofDisableArbTex();
}

float ofxFontStash::getLineHeight() {
	float lh = 0;
	if (font != FONS_INVALID) {
		fonsVertMetrics(fs, NULL, NULL, &lh);
	}
	return lh;
}

 float ofxFontStash::stringWidth(const std::string & s) {
	if (font != FONS_INVALID) {
		return fonsTextBounds(fs, 0, 0, s.c_str(), NULL, NULL);
	}
	return 0;
}

 float ofxFontStash::stringHeight(const std::string & s) {
	 float bounds[4] = { 0, 0, 0, 0 };
	 if (font != FONS_INVALID) {
		 fonsTextBounds(fs, 0, 0, s.c_str(), NULL, bounds);
	 }
	 return bounds[3] - bounds[1]; // maxy - miny
 }

 ofRectangle ofxFontStash::getStringBoundingBox(const string & s, float x, float y) {
	 float bounds[4] = { 0, 0, 0, 0 };
	 if (font != FONS_INVALID) {
		 fonsTextBounds(fs, x, y, s.c_str(), NULL, bounds);
	 }
	 return ofRectangle(bounds[0], bounds[1], bounds[2], bounds[3]);
 }

 
 vector<ofRectangle> ofxFontStash::getStringSymbolsBoundingBoxes(const string & s, float x, float y, bool needJoin) {
	 vector<ofRectangle> rects;
	 if (font != FONS_INVALID) {

		 FONSstate* state = fons__getState(fs);
		 unsigned int codepoint;
		 unsigned int utf8state = 0;
		 FONSquad q;
		 FONSglyph* glyph = NULL;
		 int prevGlyphIndex = -1;
		 short isize = (short)(state->size*10.0f);
		 short iblur = (short)state->blur;
		 float scale;
		 FONSfont* font;
		 float startx, advance;
		 float minx, miny, maxx, maxy;

		 if (fs == NULL) return rects;
		 if (state->font < 0 || state->font >= fs->nfonts) return rects;
		 font = fs->fonts[state->font];
		 if (font->data == NULL) return rects;

		 scale = fons__tt_getPixelHeightScale(&font->font, (float)isize / 10.0f);

		 // Align vertically.
		 y += fons__getVertAlign(fs, font, state->align, isize);

		 minx = maxx = x;
		 miny = maxy = y;
		 startx = x;

		 char* str = (char*)s.data();
		 char* end = str + strlen(str);

		 for (; str != end; ++str) {
			 if (fons__decutf8(&utf8state, &codepoint, *(const unsigned char*)str))
				 continue;
			 glyph = fons__getGlyph(fs, font, codepoint, isize, iblur);
			 if (glyph != NULL) {
				 fons__getQuad(fs, font, prevGlyphIndex, glyph, scale, state->spacing, &x, &y, &q);
				 if (q.x0 < minx) minx = q.x0;
				 if (q.x1 > maxx) maxx = q.x1;
				 if (fs->params.flags & FONS_ZERO_TOPLEFT) {
					 if (q.y0 < miny) miny = q.y0;
					 if (q.y1 > maxy) maxy = q.y1;
				 }
				 else {
					 if (q.y1 < miny) miny = q.y1;
					 if (q.y0 > maxy) maxy = q.y0;
				 }
				 rects.push_back(ofRectangle(q.x0, q.y0, q.x1 - q.x0, q.y1 - q.y0));
			 }
			 prevGlyphIndex = glyph != NULL ? glyph->index : -1;
		 }

		 advance = x - startx;

		 // Align horizontally
		 if (state->align & FONS_ALIGN_LEFT) {
			 // empty
		 }
		 else if (state->align & FONS_ALIGN_RIGHT) {
			 minx -= advance;
			 maxx -= advance;
			 for (size_t i = 0; i < rects.size(); i++) {
				 rects[i].x -= advance;
			 }
		 }
		 else if (state->align & FONS_ALIGN_CENTER) {
			 minx -= advance * 0.5f;
			 maxx -= advance * 0.5f;
			 for (size_t i = 0; i < rects.size(); i++) {
				 rects[i].x -= advance * 0.5f;
			 }
		 }

		 if (needJoin) {
			 for (size_t i = 0; i < rects.size(); i++) {
				 if (i < rects.size() - 1) {
					 rects[i].width = rects[i + 1].x - rects[i].x;
				 }
				 rects[i].y = miny;
				 rects[i].height = maxy - miny;
			 }
		 }
	 }

	 return rects;
 }
 

void ofxFontStash::drawString(const string & s, float x, float y) {
	if (font != FONS_INVALID) {
		fonsDrawText(fs, x, y, s.c_str(), NULL);
	}
}

