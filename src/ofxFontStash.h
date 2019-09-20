#ifndef ofxFontStash_h
#define ofxFontStash_h

#include "ofMain.h"

extern "C" {
#include "fontstash.h"
#include "offontstash.h"
}

class ofxFontStash {
    int font = FONS_INVALID;
    FONScontext* fs = NULL;
    
public:
    
    ofxFontStash();
    ~ofxFontStash();
    
	void load(const filesystem::path &filename, float fontsize);

	float getLineHeight();
	float stringWidth(const std::string& s);
	float stringHeight(const std::string& s);
	ofRectangle getStringBoundingBox(const string &s, float x, float y);

	// custom function
	vector<ofRectangle> getStringSymbolsBoundingBoxes(const string &s, float x, float y, bool needJoin = false);

	void drawString(const string &s, float x, float y);

	/// static C error handler
	static void stashError(void* uptr, int error, int val);
};

#endif

