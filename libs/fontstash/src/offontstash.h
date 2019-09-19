#ifndef GLFONTSTASH_H
#define GLFONTSTASH_H

FONScontext* glfonsCreate(int width, int height, int flags);
void glfonsDelete(FONScontext* ctx);

unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#endif

#ifdef GLFONTSTASH_IMPLEMENTATION

#include "ofMain.h"

struct GLFONScontext {
	ofImage* img;
	ofVbo* vbo;
	int width, height;
};
typedef struct GLFONScontext GLFONScontext;

static int glfons__renderCreate(void* userPtr, int width, int height)
{
	GLFONScontext* context = (GLFONScontext*)userPtr;
	// Create may be called multiple times, delete existing texture.
	if (context->img != 0) {
		delete context->img;
		context->img = 0;
	}

	if (context->vbo != 0) {
		delete context->vbo;
		context->vbo = 0;
	}

	context->width = width;
	context->height = height;

	context->img = new ofImage();
	context->img->allocate(width, height, OF_IMAGE_COLOR_ALPHA);
	
	context->vbo = new ofVbo();
	
	return 1;
}

static int glfons__renderResize(void* userPtr, int width, int height)
{
	// Reuse create to resize too.
	return glfons__renderCreate(userPtr, width, height);
}

static void glfons__renderUpdate(void* userPtr, int* rect, const unsigned char* data)
{
	GLFONScontext* context = (GLFONScontext*)userPtr;
	int w = rect[2] - rect[0];
	int h = rect[3] - rect[1];

	if (context->img == 0) return;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			context->img->setColor(x + rect[0], y + rect[1], ofColor(255, data[(rect[1] + y) * context->width + (rect[0] + x)]));
		}
	}

	context->img->update();
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
	GLFONScontext* context = (GLFONScontext*)userPtr;
	if (context->img == 0) return;

	/*
	// debug
	context->img->draw(0,0);
	return;
	*/

	ofFillFlag flag = ofGetFill();
	ofFill();
	context->img->bind();

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

	context->img->unbind();
	flag ? ofFill() : ofNoFill();
}

static void glfons__renderDelete(void* userPtr)
{
	GLFONScontext* context = (GLFONScontext*)userPtr;

	if (context->img != 0) {
		delete context->img;
		context->img = 0;
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
	GLFONScontext* context;

	context = (GLFONScontext*)malloc(sizeof(GLFONScontext));
	if (context == NULL) goto error;
	memset(context, 0, sizeof(GLFONScontext));

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
