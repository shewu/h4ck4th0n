#ifndef MATERIAL_H
#define MATERIAL_H

#include "hack.h"

class Material
{
	
};

class Texture : public Material
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

        /**
         * Gets the red channel of this Color instance.
         *
         * @return the value of the red channel.
         */
        const unsigned char getR() const {
            return r;
        }

        /**
         * Gets the green channel of this Color instance.
         *
         * @return the value of the green channel.
         */
        const unsigned char getG() const {
            return g;
        }

        /**
         * Gets the blue channel of this Color instance.
         *
         * @return the value of the blue channel.
         */
        const unsigned char getB() const {
            return b;
        }
};

#endif
