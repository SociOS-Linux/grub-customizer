#include "pscriptname_translator.h"

std::string pscriptname_decode(std::string const& input){
	std::string result = input;
	int last_nonnum_pos = input.find_last_not_of("0123456789");
	if (last_nonnum_pos != -1 && result[last_nonnum_pos] == '~' && last_nonnum_pos != input.length()-1)
		result = result.substr(0, last_nonnum_pos);
	return result;
}

std::string pscriptname_encode(std::string const& input, int x){
	std::ostringstream out;
	out << input;
	int last_nonnum_pos = input.find_last_not_of("0123456789");
	if (x != 0 || (last_nonnum_pos != -1 && input[last_nonnum_pos] == '~') && last_nonnum_pos != input.length()-1)
		out << "~" << x;
	return out.str();
}
