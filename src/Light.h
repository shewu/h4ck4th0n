#ifndef LIGHT_H
#define LIGHT_H

class Light {
	public:
		/**
		 * Gets the position of the light.
		 *
		 * @return the position of the light.
		 */
		Vector3D getPosition() const {
			return position;
		}

		/**
		 * Gets the color of the light.
		 *
		 * @return the color of the light.
		 */
		Color getColor() const {
			return color;
		}

		/**
		 * Default constructor; does nothing.
		 */
		Light() { }

		/**
		 * Constructs a light from a position and a color.
		 *
		 * @param position the position of the light.
		 * @param color the color of the light.
		 */
		Light(Vector3D position, Color color) : position(position), color(color) { }

	private:
		Vector3D position;
		Color color;
};

#endif

