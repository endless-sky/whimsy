/* glyphs.cpp
Copyright 2020 Michael Zahniser

Program to export glyphs for a bitmap font.
*/

#include <vector>
#include <iostream>
#include <fstream>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

using namespace std;

static const int GLYPHS = 98;

static const int FONT_SIZE = 16;
static const int CHAR_W = 18;
static const int CHAR_H = 18;
static const int BASE = 13;
static const int LEFT = 2;
static const int DPI = 72;

static const int WIDTH = CHAR_W * GLYPHS;
static const int HEIGHT = CHAR_H;
static const int GLYPH_PITCH = CHAR_W;

static const char *filename = "/usr/local/share/fonts/truetype/Optima.ttf";



int main(int argc, char *argv[])
{
	FT_Library library;
	FT_Init_FreeType(&library);
	
	FT_Face face;
	FT_New_Face(library, filename, 0, &face);
	
	FT_Set_Char_Size(face, FONT_SIZE * 64, 0, DPI, 0);
	FT_GlyphSlot slot = face->glyph;
	
	vector<unsigned char> image(WIDTH * HEIGHT, 0);
	vector<unsigned char>::iterator start = image.begin();
	for(int n = 32; n < 32 + GLYPHS; ++n)
	{
		// Map normal quotes to curly quotes.
		int trueN = n;
		if(n == '\'')
			trueN = 0x2019;
		if(n == '"')
			trueN = 0x201D;
		if(n == 128)
			trueN = 0x2018;
		if(n == 129)
			trueN = 0x201C;
		FT_Load_Char(face, trueN, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
		
		// Copy the glyph into the output bitmap.
		for(size_t row = 0; row < slot->bitmap.rows; ++row)
		{
			int y = BASE - slot->bitmap_top + row;
			if(y < 0 || y >= CHAR_H)
				continue;
			
			vector<unsigned char>::iterator it = start + WIDTH * (CHAR_H - 1 - y) + LEFT;
			for(size_t x = 0; x < slot->bitmap.width; ++x)
				*it++ = slot->bitmap.buffer[row * slot->bitmap.pitch + x];
		}
		start += GLYPH_PITCH;
	}
	
	ofstream out("font.bmp", ios::out | ios::binary);
	out.put('B');
	out.put('M');
	
	unsigned size = WIDTH * HEIGHT * 4 + 54;
	out.put((size >> 0) & 0xFF);
	out.put((size >> 8) & 0xFF);
	out.put((size >> 16) & 0xFF);
	out.put((size >> 24) & 0xFF);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put(54);
	out.put(0);
	out.put(0);
	out.put(0);
	
	out.put(40);
	out.put(0);
	out.put(0);
	out.put(0);
	out.put((WIDTH >> 0) & 0xFF);
	out.put((WIDTH >> 8) & 0xFF);
	out.put((WIDTH >> 16) & 0xFF);
	out.put((WIDTH >> 24) & 0xFF);
	out.put((HEIGHT >> 0) & 0xFF);
	out.put((HEIGHT >> 8) & 0xFF);
	out.put((HEIGHT >> 16) & 0xFF);
	out.put((HEIGHT >> 24) & 0xFF);
	out.put(1);
	out.put(0);
	out.put(4 * 8);
	out.put(0);
	for(int i = 0; i < 24; ++i)
		out.put(0);
	
	for(vector<unsigned char>::iterator it = image.begin(); it != image.end(); ++it)
	{
		out.put(*it);
		out.put(*it);
		out.put(*it);
		out.put(255);
	}
	
	return 0;
}
