#ifndef IMAGE_MANAGER_HPP
#define IMAGE_MANAGER_HPP

struct mineTilesImageGIMP
{
	unsigned int width;
	unsigned int height;
	unsigned int bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
	unsigned char pixel_data[64 * 64 * 4 + 1];
};

namespace ImageManager
{
	extern const mineTilesImageGIMP mineTiles_img;
}

#endif // IMAGE_MANAGER_HPP
