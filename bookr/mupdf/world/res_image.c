#include "fitz-base.h"
#include "fitz-world.h"

fz_image *
fz_keepimage(fz_image *image)
{
	image->refs ++;
	return image;
}

void
fz_dropimage(fz_image *image)
{
#ifndef PSP
	printf("fz_dropimage, image %p, image->refs: %d\n", image, image->refs);
#endif
	if (--image->refs <= 0)
	{
		if (image->drop)
			image->drop(image);
		if (image->cs)
			fz_dropcolorspace(image->cs);
		fz_free(image);
	}
}

