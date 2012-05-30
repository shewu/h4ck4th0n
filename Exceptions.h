#include <exception>

class ParseException : public std::exception {
	private:
		std::string msg;
    public:
        ParseException(std::string msg = "") : msg(msg) { }
		~ParseException() throw () { }

		const char* what() const throw() {
			return msg.c_str();
		}
};

