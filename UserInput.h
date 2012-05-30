class UserInput {
	private:
		bool _left, _right, _up, _down;

	public:
		UserInput(bool left, bool right, bool up, bool down) :
		    _left(left), _right(right), _up(up), _down(down) { }

		float getAngleChangeRate();
		float getForwardAcceleration();
}
