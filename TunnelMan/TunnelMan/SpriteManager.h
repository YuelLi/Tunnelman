#ifndef SPRITEMANAGER_H_
#define SPRITEMANAGER_H_

#include "freeglut.h"

#ifndef GL_BGR
#define GL_BGR GL_BGR_EXT
#endif

#ifndef GL_BGRA
#define GL_BGRA GL_BGRA_EXT
#endif

#include "GameConstants.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

class SpriteManager
{
public:

	SpriteManager()
	 : m_mipMapped(true)
	{
	}

	void setMipMapping(bool status)
	{
		m_mipMapped = status;
	}

	bool loadSprite(std::string filename_tga, int imageID, int frameNum)
	{
		  // Load Texture Data From TGA File

		unsigned int spriteID = getSpriteID(imageID, frameNum);
		if (INVALID_SPRITE_ID == spriteID)
			return false;

		m_frameCountPerSprite[imageID]++;	// keep track of how many frames per sprite we loaded

		std::string line;
		std::string contents = "";
		std::ifstream tgaFile(filename_tga, std::ios::in|std::ios::binary);

		if (!tgaFile)
			return false;

		char type[3];
		char info[6];
		unsigned char byteCount;
		unsigned int textureWidth;
		unsigned int textureHeight;
		long imageSize;
		char* imageData = nullptr;

		  // Read file header info
		tgaFile.read(type, 3);
		tgaFile.seekg(12);
		tgaFile.read(info, 6);
		textureWidth = static_cast<unsigned char>(info[0]) + static_cast<unsigned char>(info[1]) * 256;
		textureHeight = static_cast<unsigned char>(info[2]) + static_cast<unsigned char>(info[3]) * 256;
		byteCount = static_cast<unsigned char>(info[4]) / 8;
		imageSize = textureWidth * textureHeight * byteCount;
		imageData = new char[imageSize];
		tgaFile.seekg(18);
		  // Read image data
		tgaFile.read(imageData, imageSize);
		if (!tgaFile)
		{
			delete [] imageData;
			return false;
		}

		  //image type either 2 (color) or 3 (greyscale)
		if (type[1] != 0 || (type[2] != 2 && type[2] != 3))
			return false;

		if (byteCount != 3 && byteCount != 4)
			return false;

		  // Transfer Texture To OpenGL

		glEnable(GL_DEPTH_TEST);

		  // allocate a texture handle
		GLuint glTextureID;
		glGenTextures(1, &glTextureID);

		  // bind our new texture
		glBindTexture(GL_TEXTURE_2D, glTextureID);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (m_mipMapped)
		{
			  // when texture area is small, bilinear filter the closest mipmap
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			  // when texture area is large, bilinear filter the first mipmap
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		  // Have the texture wrap both vertically and horizontally.
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLfloat>(GL_REPEAT));
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLfloat>(GL_REPEAT));

		if (m_mipMapped)
		{
			  // build our texture mipmaps
			  // byteCount of 3 means that BGR data is being supplied. byteCount of 4 means that BGRA data is being supplied.
			if (3 == byteCount)
				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, textureWidth, textureHeight, GL_BGR, GL_UNSIGNED_BYTE, imageData);
			else if (4 == byteCount)
				gluBuild2DMipmaps(GL_TEXTURE_2D, 4, textureWidth, textureHeight, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
		}
		else
		{
			  // byteCount of 3 means that BGR data is being supplied. byteCount of 4 means that BGRA data is being supplied.
			if (3 == byteCount)
				glTexImage2D(GL_TEXTURE_2D, 0, 3, textureWidth, textureHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData);
			else if (4 == byteCount)
				glTexImage2D(GL_TEXTURE_2D, 0, 4, textureWidth, textureHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
		}

		delete [] imageData;

		m_imageMap[spriteID] = glTextureID;

		return true;
	}

	unsigned int getNumFrames(int imageID) const
	{
		auto it = m_frameCountPerSprite.find(imageID);
		if (it == m_frameCountPerSprite.end())
			return 0;

		return it->second;
	}

	enum Angle {
		degrees_0 = 0, degrees_90 = 90, degrees_180 = 180, degrees_270 = 270,
		face_left = 1, face_right = 2, face_up = 3, face_down = 4
	};

	bool plotSprite(int imageID, int frame, double gx, double gy, double gz, Angle angleDegrees, double size)
	{
		unsigned int spriteID = getSpriteID(imageID,frame);
		if (INVALID_SPRITE_ID == spriteID)
			return false;

		auto it = m_imageMap.find(spriteID);
		if (it == m_imageMap.end())
			return false;

		glPushMatrix();

		double finalWidth, finalHeight;

		finalWidth = SPRITE_WIDTH_GL * size;
		finalHeight = SPRITE_HEIGHT_GL * size;

		// object's x/y location is center-based, but sprite plotting is upper-left-corner based
		const double xoffset = finalWidth / 2;
		const double yoffset = finalHeight / 2;

		glTranslatef(static_cast<GLfloat>(gx-xoffset),static_cast<GLfloat>(gy-yoffset),static_cast<GLfloat>(gz));
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glEnable (GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, it->second);

		glColor3f(1.0, 1.0, 1.0);

		double cx1,cx2,cx3,cx4;
		double cy1,cy2,cy3,cy4;

		switch (angleDegrees)
		{
		default:
		case degrees_0:
		case face_right:
			cx1 = 0; cy1 = 0;
			cx2 = 1; cy2 = 0;
			cx3 = 1; cy3 = 1;
			cx4 = 0; cy4 = 1;
			break;
		case face_left:
			cx1 = 1; cy1 = 0;
			cx2 = 0; cy2 = 0;
			cx3 = 0; cy3 = 1;
			cx4 = 1; cy4 = 1;
			break;
		case degrees_90:
		case face_up:
			cx1 = 0; cy1 = 1;
			cx2 = 0; cy2 = 0;
			cx3 = 1; cy3 = 0;
			cx4 = 1; cy4 = 1;
			break;
		case degrees_180:
			cx1 = 1; cy1 = 1;
			cx2 = 0; cy2 = 1;
			cx3 = 0; cy3 = 0;
			cx4 = 1; cy4 = 0;
			break;
		case degrees_270:
		case face_down:
			cx1 = 1; cy1 = 0;
			cx2 = 1; cy2 = 1;
			cx3 = 0; cy3 = 1;
			cx4 = 0; cy4 = 0;
			break;
		}

		glBegin (GL_QUADS);
		  glTexCoord2d(cx1, cy1);
		  glVertex3f(0, 0, 0);
		  glTexCoord2d(cx2, cy2);
		  glVertex3f(static_cast<GLfloat>(finalWidth), 0, 0);
		  glTexCoord2d(cx3, cy3);
		  glVertex3f(static_cast<GLfloat>(finalWidth), static_cast<GLfloat>(finalHeight), 0);
		  glTexCoord2d(cx4, cy4);
		  glVertex3f(0, static_cast<GLfloat>(finalHeight), 0);
		  glEnd();

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);

		glPopAttrib();
		glPopMatrix();

		return true;
	}

	~SpriteManager()
	{
		for (auto it = m_imageMap.begin(); it != m_imageMap.end(); it++)
			glDeleteTextures(1, &it->second);
	}

private:

	bool							m_mipMapped;
	std::map<unsigned int, GLuint>	m_imageMap;
	std::map<unsigned int, unsigned int>		m_frameCountPerSprite;

	static const int INVALID_SPRITE_ID = -1;
	static const int MAX_IMAGES = 1000;
	static const int MAX_FRAMES_PER_SPRITE = 100;

	int getSpriteID(unsigned int imageID, unsigned int frame) const
	{
		if (imageID >= MAX_IMAGES || frame >= MAX_FRAMES_PER_SPRITE)
			return INVALID_SPRITE_ID;

		return imageID * MAX_FRAMES_PER_SPRITE + frame;
	}
};

#endif // SPRITEMANAGER_H_
