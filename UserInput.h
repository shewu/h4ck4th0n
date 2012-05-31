/**
 * Represents a user input state.
 */
class UserInput {
	private:
		bool _left, _right, _up, _down;
		float _theta;

	public:
		/**
		 * Creates a UserInput object from the given input key inputs.
		 * @param left True if the `left' button is pressed down.
		 * @param right True if the `right' button is pressed down.
		 * @param up True if the `up' button is pressed down.
		 * @param down True if the `down' button is pressed down.
		 */
		UserInput(bool left, bool right, bool up, bool down, float theta) :
		    _left(left), _right(right), _up(up), _down(down), _theta(theta) { }

		/**
		 * Returns the forward acceleration vector indicated by the key input; that is,
		 * the acceleration of the player, where (0, 1) points in the direction the player
		 * is currently facing.
		 * @return the forward acceleration indicated by the key input.
		 */
		Vector2D getForwardAcceleration() const;
}
