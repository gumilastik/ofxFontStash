#ifndef GLFONTSTASH_H
#define GLFONTSTASH_H

FONScontext* glfonsCreate(int width, int height, int flags);
void glfonsDelete(FONScontext* ctx);

unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#endif

#ifdef GLFONTSTASH_IMPLEMENTATION

#include "ofMain.h"

struct OFFONScontext {
	ofFbo* fbo;
	ofVbo* vbo;
	int width, height;
};
typedef struct OFFONScontext OFFONScontext;

static int glfons__renderCreate(void* userPtr, int width, int height)
{
	OFFONScontext* context = (OFFONScontext*)userPtr;
	// Create may be called multiple times, delete existing texture.
	if (context->fbo != 0) {
		delete context->fbo;
		context->fbo = 0;
	}

	if (context->vbo == 0) {
		context->vbo = new ofVbo();
	}

	context->width = width;
	context->height = height;

	context->fbo = new ofFbo();
	context->fbo->allocate(width, height, GL_RGBA);
	return 1;
}

static int glfons__renderResize(void* userPtr, int width, int height)
{
	// Reuse create to resize too.
	return glfons__renderCreate(userPtr, width, height);
}

static void glfons__renderUpdate(void* userPtr, int* rect, const unsigned char* data)
{
	OFFONScontext* context = (OFFONScontext*)userPtr;
	int w = rect[2] - rect[0];
	int h = rect[3] - rect[1];

	if (context->fbo == 0) return;

	ofImage img;
	img.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			img.setColor(x, y, ofColor(255, data[(x + rect[0]) + (y + rect[1]) * context->width]));
		}
	}
	img.update();

	context->fbo->begin();
	img.draw(rect[0], rect[1]);
	context->fbo->end();
}


ofFloatColor rgbaToOf(unsigned int& col)
{
	ofFloatColor c;
	c.a = (col >> 24 & 0xff) / 255.0;
	c.b = (col >> 16 & 0xff) / 255.0;
	c.g = (col >> 8 & 0xff) / 255.0;
	c.r = (col >> 0 & 0xff) / 255.0;
	return c;
}

static void glfons__renderDraw(void* userPtr, const float* verts, const float* tcoords, const unsigned int* colors, int nverts)
{
	OFFONScontext* context = (OFFONScontext*)userPtr;
	if (context->fbo == 0) return;

	/*
	// debug
	context->fbo->getTexture()->draw(0, 0);
	return;
	*/

	context->fbo->getTexture().bind();

	context->vbo->setVertexData((glm::vec2*)verts, nverts, GL_DYNAMIC_DRAW);
	context->vbo->setTexCoordData(tcoords, nverts, GL_DYNAMIC_DRAW);
	/*
	vector<ofFloatColor> col;
	col.resize(nverts);
	for (int i = 0; i < nverts; i++)
	{
		col[i] = rgbaToOf(colors[i]);
	}
	context->vbo->setColorData(col.data(), nverts, GL_DYNAMIC_DRAW);
	*/

	context->vbo->draw(GL_TRIANGLES, 0, nverts);

	context->fbo->getTexture().unbind();
}

static void glfons__renderDelete(void* userPtr)
{
	OFFONScontext* context = (OFFONScontext*)userPtr;
	if (context->fbo != 0) {
		delete context->fbo;
		context->fbo = 0;
	}
	if (context->vbo != 0) {
		delete context->vbo;
		context->vbo = 0;
	}
	free(context);
}

FONScontext* glfonsCreate(int width, int height, int flags)
{
	FONSparams params;
	OFFONScontext* context;

	context = (OFFONScontext*)malloc(sizeof(OFFONScontext));
	if (context == NULL) goto error;
	memset(context, 0, sizeof(OFFONScontext));

	memset(&params, 0, sizeof(params));
	params.width = width;
	params.height = height;
	params.flags = (unsigned char)flags;
	params.renderCreate = glfons__renderCreate;
	params.renderResize = glfons__renderResize;
	params.renderUpdate = glfons__renderUpdate;
	params.renderDraw = glfons__renderDraw;
	params.renderDelete = glfons__renderDelete;
	params.userPtr = context;

	return fonsCreateInternal(&params);

error:
	if (context != NULL) free(context);
	return NULL;
}

void glfonsDelete(FONScontext* ctx)
{
	fonsDeleteInternal(ctx);
}

unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (r) | (g << 8) | (b << 16) | (a << 24);
}

#endif
