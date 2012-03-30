#include <exception>

class ParseException : public std::exception {
	private:
		std::string msg;
    public:
        ParseException(std::string msg = "") : msg(msg) { }

		const char* what() const throw() {
			return msg.c_str();
		}
};

