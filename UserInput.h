/**
 * Represents a user input state.
 */
class UserInput {
	private:
		bool _left, _right, _up, _down;

	public:
		/**
		 * Creates a UserInput object from the given input key inputs.
		 * @param left True if the left arrow key is pressed down.
		 * @param right True if the right arrow key is pressed down.
		 * @param up True if the up arrow key is pressed down.
		 * @param down True if the down arrow key is pressed down.
		 */
		UserInput(bool left, bool right, bool up, bool down) :
		    _left(left), _right(right), _up(up), _down(down) { }

		/**
		 * @return the angle change rate indicated by the key input.
		 */
		float getAngleChangeRate();

		/**
		 * @return the forward acceleration indicated by the key input.
		 */
		float getForwardAcceleration();
}
