#ifndef MATERIAL_H
#define MATERIAL_H

#include "hack.h"

class Material
{
	
};

class Color : public Material
{
    private:
        unsigned char r, g, b, a;

	public:
        /**
         * Creates a color, whose default color is black. If any of the
         * arguments are greater than 255 or less than 0, takes the positive
         * result of mod 256.
         *
         * @param x the red channel. Defaults to 0.
         * @param y the green channel. Defaults to 0.
         * @param z the blue channel. Defaults to 0.
         */
		Color(unsigned char x = 0, unsigned char y = 0, unsigned char z = 0) 
            : r(x), g(y), b(z), a(0) {} 
};

#endif
