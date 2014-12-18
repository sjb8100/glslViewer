#include "texture.h"

#include <stdio.h>
#include <string.h>
#include <iostream>

#include <FreeImage.h>		//	FreeImage type is GL_BGR
#include <GLES2/gl2ext.h>	//	Need for GL_BGRA_EXT

// static variable for freeImage initialization:
void InitFreeImage(bool deinit=false){
	// need a new bool to avoid c++ "deinitialization order fiasco":
	// http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.15
	static bool	* bFreeImageInited = new bool(false);
	if(!*bFreeImageInited && !deinit){
		FreeImage_Initialise();
		*bFreeImageInited = true;
	}
	if(*bFreeImageInited && deinit){
		FreeImage_DeInitialise();
	}
}

Texture::Texture():m_width(0),m_height(0),m_bpp(0),m_id(0){
}

Texture::~Texture(){
	glDeleteTextures(1, &m_id);
}

bool Texture::load(const std::string& _path){

	InitFreeImage();

	glEnable(GL_TEXTURE_2D);

	//	From http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=9
	//
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; 
   	FIBITMAP* dib(0); 
	fif = FreeImage_GetFileType(_path.c_str(), 0); // Check the file signature and deduce its format

	if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
    	fif = FreeImage_GetFIFFromFilename(_path.c_str()); 

   	if(fif == FIF_UNKNOWN){
   		// If still unkown, return failure
   		std::cerr << "Texture: Unknown type" << std::endl; 
   		return false;
   	}

   	if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
      dib = FreeImage_Load(fif, _path.c_str()); 
   	if(!dib){
   		std::cerr << "Texture: unhable to open" << std::endl;
   		return false;
   	}

	FIBITMAP *pImage = FreeImage_ConvertTo32Bits(dib);
	m_width = FreeImage_GetWidth(pImage);
	m_height = FreeImage_GetHeight(pImage);

	// If somehow one of these failed (they shouldn't), return failure
	if(pImage == NULL || m_width == 0 || m_height == 0){
		std::cerr << "Texture: something went wrong" << std::endl;
		return false; 
	} 

	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id); 
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, m_width, m_height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	FreeImage_Unload(dib);

	unbind();

	return true;
}

void Texture::bind(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind(){
	glBindTexture(GL_TEXTURE_2D, 0);
}