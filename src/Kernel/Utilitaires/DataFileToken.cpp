/****************************************************************************
* Copyright (c) 2025, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#include <DataFileToken.h>
#include <DataFileExpression.h>

/**
 * @brief Constructs a DataFileToken object and initializes its attributes.
 *
 * This constructor initializes the token with its file location, type, and string value.
 * Depending on the token type, it computes and stores the corresponding value in a `std::any`.
 *
 * @param file_name_ The name of the file where the token is located.
 * @param start_line_ The starting line number of the token in the file.
 * @param start_column_ The starting column number of the token in the file.
 * @param end_line_ The ending line number of the token in the file.
 * @param end_column_ The ending column number of the token in the file.
 * @param type_ The type of the token, as defined in the DataFileToken::Type enumeration.
 * @param string_ The string representation of the token read from the input file.
 *
 * @note For certain token types, the constructor extracts or computes a value from the string
 *       representation and stores it in the `value` attribute. For example:
 *       - Comments have their delimiters removed.
 *       - String literals have their quotes removed.
 *       - Integer and floating-point tokens are parsed into their respective numeric types.
 *
 * @throws std::invalid_argument If the string cannot be converted to an integer or floating-point value.
 * @throws std::out_of_range If the numeric value is out of range for its type.
 *
 * @warning If an error occurs during numeric conversion or an unsupported token type is encountered,
 *          the program will terminate using `Process::exit(DONT_PRINT_TOKEN)`.
 */
DataFileToken::DataFileToken(
		const std::string& file_name_,
		size_t start_line_,
		size_t start_column_,
		size_t end_line_,
		size_t end_column_,
		DataFileToken::Type type_,
		std::string string_):

		file_name(file_name_),
		start_line(start_line_),
		start_column(start_column_),
		end_line(end_line_),
		end_column(end_column_),
		type(type_),
		string(string_) {

	// depending on the type, compute the value and store it in a std::any
	switch (type) {
		case DataFileToken::Type::CommentBlockHash:
		case DataFileToken::Type::CommentBlockCStyle:
			value = string.substr(2, string.size() - 4); break;
		case DataFileToken::Type::CommentLineCStyle:
			value = string.substr(2); break;
		case DataFileToken::Type::OpenCurlyBracket:
		case DataFileToken::Type::CloseCurlyBracket:
		case DataFileToken::Type::OpenParenthesis:
		case DataFileToken::Type::CloseParenthesis:
		case DataFileToken::Type::Space:
		case DataFileToken::Type::Expression:
			value = std::string(""); break;
		case DataFileToken::Type::Operator:
		case DataFileToken::Type::OperatorUnary:
		case DataFileToken::Type::Name:
			value = string; break;
		case DataFileToken::Type::LiteralString:
			value = string.substr(1, string.size() - 2); break;
		case DataFileToken::Type::Int: {
        	try {
            	value = std::stoi(string);
				break;
			} catch (std::invalid_argument const& ex) {
				print_as_error("Failed to interpret this token as integer.");
				Process::exit(DONT_PRINT_TOKEN);
				break;
			} catch (std::out_of_range const& ex) {
				print_as_error("Integer value is out of available range.");
				Process::exit(DONT_PRINT_TOKEN);
				break;
			}}
		case DataFileToken::Type::Float: {
        	try {
            	value = std::stod(string);
				break;
			} catch (std::invalid_argument const& ex) {
				print_as_error("Failed to interpret this token as floating point number.");
				Process::exit(DONT_PRINT_TOKEN);
				break;
			} catch (std::out_of_range const& ex) {
				print_as_error("Floating point value is out of available range.");
				Process::exit(DONT_PRINT_TOKEN);
				break;
			}}
		case DataFileToken::Type::Macro:
			value = string.substr(1); break;
		default:
			print_as_error("Not valid token type for this token.");
			Process::exit(DONT_PRINT_TOKEN);
	}
}

// return a set of all necessary Types for tokenization
std::set<DataFileToken::Type> DataFileToken::get_types_full_set() {
	std::set<DataFileToken::Type> out = {
		CommentBlockHash,
		CommentBlockCStyle,
		CommentLineCStyle,
		OpenCurlyBracket,
		CloseCurlyBracket,
		OpenParenthesis,
		CloseParenthesis,
		Operator,
		Space,
		Name,
		LiteralString,
		Int,
		Float,
		Macro
	};
	return out;
}


/**
 * @brief Determines if a given character is considered a "letter" based on specific criteria.
 * 
 * This function checks if the input character falls within one of the following categories:
 * - Lowercase English letters ('a' to 'z')
 * - Uppercase English letters ('A' to 'Z')
 * - Underscore ('_')
 * - Period ('.')
 * - Percent sign ('%')
 * - Equals sign ('=')
 * - Digits ('0' to '9')
 * 
 * @param c The character to be checked.
 * @return true If the character matches any of the specified criteria.
 * @return false Otherwise.
 */
bool DataFileToken::isletter(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_' || c == '.' || c == '%' || c == '=' || ('0' <= c && c <= '9');
}

/**
 * @brief Converts a double value to its string representation in scientific notation.
 *
 * This function takes a double value and converts it to a string using scientific
 * notation with the maximum number of significant digits that can be represented
 * without loss of precision for a double type.
 *
 * @param value The double value to be converted to a string.
 * @return A string representation of the input value in scientific notation.
 */
std::string DataFileToken::to_string(double value) {
	std::ostringstream oss;
    oss << std::scientific << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
    return oss.str();
}

/**
 * @brief Overloads the stream insertion operator to output a DataFileToken::Type.
 *
 * This function allows a DataFileToken::Type to be written to a Sortie stream.
 * It uses an intermediate std::ostringstream to convert the token type to a string
 * representation before inserting it into the output stream.
 *
 * @param os The output stream (Sortie) to which the token type will be written.
 * @param token_type The DataFileToken::Type to be written to the output stream.
 * @return A reference to the output stream (Sortie) after the token type has been written.
 */
Sortie& operator<<(Sortie& os, const DataFileToken::Type& token_type) {
	std::ostringstream oss;
	oss << token_type;
	return os << oss.str();
}

/**
 * @brief Overloads the stream insertion operator to output a string representation
 *        of a DataFileToken::Type enumeration value.
 * 
 * @param os The output stream to which the string representation will be written.
 * @param token_type The DataFileToken::Type enumeration value to be converted to a string.
 * @return A reference to the output stream after the string representation has been written.
 * 
 * @details This function maps each enumeration value of DataFileToken::Type to its
 *          corresponding string representation. If the enumeration value does not
 *          match any of the predefined cases, "UNKNOWN_TOKEN_TYPE" is written to the stream.
 * 
 * @note The function assumes that all possible enumeration values are handled explicitly
 *       in the switch statement. If a new enumeration value is added to DataFileToken::Type,
 *       the switch statement should be updated accordingly.
 */
std::ostream& operator<<(std::ostream& os, const DataFileToken::Type& token_type) {
	switch (token_type) {
		case DataFileToken::Type::CommentBlockHash:
			return os << "CommentBlockHash";
		case DataFileToken::Type::CommentBlockCStyle:
			return os << "CommentBlockCStyle";
		case DataFileToken::Type::CommentLineCStyle:
			return os << "CommentLineCStyle";
		case DataFileToken::Type::OpenCurlyBracket:
			return os << "OpenCurlyBracket";
		case DataFileToken::Type::CloseCurlyBracket:
			return os << "CloseCurlyBracket";
		case DataFileToken::Type::OpenParenthesis:
			return os << "OpenParenthesis";
		case DataFileToken::Type::CloseParenthesis:
			return os << "CloseParenthesis";
		case DataFileToken::Type::Operator:
			return os << "Operator";
		case DataFileToken::Type::OperatorUnary:
			return os << "OperatorUnary";
		case DataFileToken::Type::Space:
			return os << "Space";
		case DataFileToken::Type::Name:
			return os << "Name";
		case DataFileToken::Type::LiteralString:
			return os << "LiteralString";
		case DataFileToken::Type::Int:
			return os << "Int";
		case DataFileToken::Type::Float:
			return os << "Float";
		case DataFileToken::Type::Macro:
			return os << "Macro";
		case DataFileToken::Type::Expression:
			return os << "Expression";
		default:
			return os << "UNKNOWN_TOKEN_TYPE";
	}
}

/**
 * @brief Checks if a given string is a valid comment block based on hash ('#') characters.
 *
 * This function determines the validity of a comment block string by checking:
 * - If the string has a single character and it is '#'.
 * - If the string has two characters and the second character is a whitespace.
 * - If the string has more than two characters and the third-to-last character is not a whitespace.
 * - If the string has more than three characters and the second-to-last character is not '#'.
 *
 * @param string The input string to validate as a comment block.
 * @return true if the string satisfies the conditions for a valid comment block, false otherwise.
 */
bool DataFileToken::check_comment_block_hash(const std::string& string) {
	// is valid if the first character is '#' and until the last character is '#'
	size_t length = string.length();
	return (length == 1 && string[0] == '#') ||
	       (length == 2 && isspace(string[1])) ||
	       (length >  2 && !isspace(string[length - 3])) ||
	       (length >  3 && string[length - 2] != '#');
}

/**
 * @brief Checks if a given string represents a valid C-style comment block.
 *
 * A valid C-style comment block, and has matching
 * pairs of delimiter patterns within the comment body.
 *
 * @param string The input string to check.
 * @return true if the string is a valid C-style comment block, false otherwise.
 *
 * @note The function considers the following cases:
 * - If the string length is 1, it is valid only if it contains '/'.
 * - If the string length is 2, it is valid only if it contains the starting delimiter.
 * - For longer strings, it checks the balance of starting delimiter and ending delimiter patterns within
 *   the comment body.
 */
bool DataFileToken::check_comment_block_c_style(const std::string& string) {
	// is valid if the first character is '/', the second is '*', and until the penultimate is '*' and the last is '/'
	size_t length = string.length();
	if (length <= 2) {
		return (length == 1 && string[0] == '/') ||
		       (length == 2 && string[1] == '*');
	} else {
		// counte the number of "/*" pattern and close pattern and close pattern
		size_t count_open = 0;
		size_t count_close = 0;

		std::string substring = string.substr(2, length - 3);

    	size_t pos = 0;
    	while ((pos = substring.find("/*", pos)) != std::string::npos) {
    	    count_open++;
        	pos += 2;
    	}

    	pos = 0;
    	while ((pos = substring.find("*/", pos)) != std::string::npos) {
    	    count_close++;
        	pos += 2;
    	}

		return count_open + 1 != count_close;
	}
}

/**
 * @brief Checks if a given string represents a C-style comment line.
 *
 * This function determines whether the input string is a valid C-style comment line.
 * A valid C-style comment line satisfies one of the following conditions:
 * - The string has a length of 1 and the first character is '/'.
 * - The string has a length of 2 and the second character is '/'.
 * - The string has a length greater than 2 and the last character is not '\n'.
 *
 * @param string The input string to check.
 * @return true if the string is a valid C-style comment line, false otherwise.
 */
bool DataFileToken::check_comment_line_c_style(const std::string& string) {
	// is valid if the first two characters are '/' and until the last character is '\n'
	size_t length = string.length();
	return (length == 1 && string[0] == '/') ||
	       (length == 2 && string[1] == '/') ||
		   (length >  2 && string[length - 1] != '\n');
}

/**
 * @brief Checks if the given string contains exactly one character and that character is an open curly bracket '{'.
 * 
 * @param string The input string to be checked.
 * @return true If the string has a length of 1 and the character is '{'.
 * @return false Otherwise.
 */
bool DataFileToken::check_open_curly_bracket(const std::string& string) {
	return (string.length() == 1 && string[0] == '{');
}

/**
 * @brief Checks if the given string is a single closing curly bracket ('}').
 *
 * This function verifies whether the input string has a length of exactly one
 * and contains the closing curly bracket character ('}').
 *
 * @param string The input string to check.
 * @return true if the string is exactly one character long and is '}', false otherwise.
 */
bool DataFileToken::check_close_curly_bracket(const std::string& string) {
	return (string.length() == 1 && string[0] == '}');
}

/**
 * @brief Checks if the given string is a single open parenthesis '('.
 * 
 * @param string The input string to check.
 * @return true If the string has a length of 1 and contains the character '('.
 * @return false Otherwise.
 */
bool DataFileToken::check_open_parenthesis(const std::string& string) {
	return (string.length() == 1 && string[0] == '(');
}

/**
 * @brief Checks if the given string is a single closing parenthesis.
 *
 * This function verifies whether the input string consists of exactly one
 * character and that character is a closing parenthesis ')'.
 *
 * @param string The input string to check.
 * @return true if the string is exactly one character long and is ')',
 *         false otherwise.
 */
bool DataFileToken::check_close_parenthesis(const std::string& string) {
	return (string.length() == 1 && string[0] == ')');
}

/**
 * @brief Checks if the given string represents a valid operator.
 * 
 * This function evaluates whether the input string matches any of the predefined
 * operators or symbols. The supported operators include arithmetic, logical, 
 * comparison, and other miscellaneous symbols.
 * 
 * @param string The input string to check.
 * @return true If the input string matches one of the predefined operators.
 * @return false If the input string does not match any of the predefined operators.
 */
bool DataFileToken::check_operator(const std::string& string) {
	return (string == "+")   ||
		   (string == "-")   ||
		   (string == "*")   ||
		   (string == "/")   ||
		   (string == "^")   ||
		   (string == "min") ||
		   (string == "max") ||
		   (string == "mod") ||
		   (string == "$")   ||
		   (string == ":")   ||
		   (string == ";")   ||
		   (string == ",")   ||
		   (string == "&")   ||
		   (string == "&&")  ||
		   (string == ">")   ||
		   (string == "<")   ||
		   (string == ">=")  ||
		   (string == "]")   ||
		   (string == "<=")  ||
		   (string == "[");
}

/**
 * @brief Checks if the last character of a given string is a whitespace character.
 * 
 * @param string The input string to be checked.
 * @return true If the last character of the string is a whitespace character.
 * @return false Otherwise.
 */
bool DataFileToken::check_space(const std::string& string) {
	return isspace(string[string.length() - 1]);
}

/**
 * @brief Checks if the given string is a valid name based on specific rules.
 *
 * A valid name must satisfy the following conditions:
 * - It can only contain alphanumeric characters.
 * - It cannot start with a digit.
 * - If the string has only one character, it must be a letter.
 * - If the string has more than one character, the last character can be either a letter or a digit.
 *
 * @param string The input string to validate.
 * @return true if the string is valid according to the rules, false otherwise.
 */
bool DataFileToken::check_name(const std::string& string) {
	// valid if it only contains alphanumeric but not digits
	// it can contain numbers in it but not on the first character
	size_t size = string.length();
	if (string.length() == 1)
		return isletter(string[size - 1]);
	else
		return isletter(string[size - 1]) || isdigit(string[size - 1]);
}

/**
 * @brief Checks if a given string is a valid literal string.
 *
 * A string is considered valid if it meets the following conditions:
 * - It contains only ASCII characters.
 * - It has a length of 1 and the single character is a double quote (`"`).
 * - It has a length of 2.
 * - It has a length greater than 2 and the second-to-last character is not a double quote (`"`).
 *
 * @param string The string to be checked.
 * @return true if the string is a valid literal string, false otherwise.
 */
bool DataFileToken::check_literal_string(const std::string& string) {
	// valid if it only contains ascii characters
	return (string.length() == 1 && string[0] == '"') ||
	       (string.length() == 2) ||
	       (string.length() >  2 && string[string.length() - 2] != '"');
}

/**
 * @brief Checks if the given string represents an integer value.
 *
 * This function determines whether the provided string is a valid integer.
 * It allows for hexadecimal values in the format "0x".
 *
 * @param string The input string to check.
 * @return true If the string represents an integer or a valid hexadecimal value.
 * @return false Otherwise.
 */
bool DataFileToken::check_int(const std::string& string) {
	size_t length = string.length();

	// allow for hexadecimal values
	if (length == 2 &&
	    string[length - 2] == '0' &&
	    string[length - 1] == 'x')
		return true;
	
	return (isdigit(string[length - 1]));
}

/**
 * @brief Checks if a given string represents a valid floating-point number.
 *
 * This function validates whether the input string conforms to the format of a 
 * floating-point number. It considers the following rules:
 * - The string can contain digits, at most one decimal point, and an optional '+' or '-' sign at the beginning.
 * - If the string ends with a '.', it is valid only if there is no other '.' in the string.
 * - If the string ends with 'e' or 'E', it is valid only if it is preceded by a digit or a '.'.
 * - If the string ends with '+' or '-', it is valid only if it is preceded by 'e' or 'E'.
 * - The string must not contain multiple occurrences of 'e' or 'E'.
 * - The string must not contain invalid characters other than digits, '.', 'e', 'E', '+', or '-'.
 *
 * @param string The input string to validate.
 * @return true If the string represents a valid floating-point number.
 * @return false Otherwise.
 */
bool DataFileToken::check_float(const std::string& string) {
	size_t length = string.length();

	// valid if contains only digits, one point at most, and potential '+-' sign in front
	if (string[length - 1] == '.') {
		// check if a point is already present in the string
		if (length > 1 || true) {
			for (size_t index = 0; index < length - 1; index++) {
				// if yes, its not a valid float string representation
				if (string[index] == '.')
					return false;
			}
		}

		return true;
	}

	if (string[length - 1] == 'e' || string[length - 1] == 'E') {
		if (length == 1)
			return false;
		else if (!isdigit(string[length - 2]) && string[length - 2] != '.')
			return false;

		// check if an exponent is already present in the string
		if (string.length() > 1 || true) {
			for (size_t index = 0; index < length - 1; index++) {
				if (string[index] == 'e' || string[index] == 'E')
					return false;
			}
		}

		return true;
	}

	// the exponent can have a + or - sign in front of it
	if (string[length - 1] == '+' || string[length - 1] == '-') {
		// the previous character should be the exponent 'e'
		if (length <= 1)
			return false;

		return string[length - 2] == 'e' || string[length-2] == 'E';
	}

	return (isdigit(string[length - 1]));
}

/**
 * @brief Checks if the given string represents a valid macro.
 *
 * A string is considered a valid macro if it meets one of the following conditions:
 * - The string has a length of 1 and its only character is '#'.
 * - The string has a length greater than 1, starts with '#', and ends with an alphabetic character.
 *
 * @param string The input string to check.
 * @return true if the string is a valid macro, false otherwise.
 */
bool DataFileToken::check_macro(const std::string& string) {
	// valid if the first character is '#', and the rest is alpha
	return (string.length() == 1 && string[0] == '#') ||
	       (string.length() > 1 && isletter(string[string.length() - 1]));
}

/**
 * @brief Checks if the given string matches the specified token type.
 *
 * This function validates whether the provided string corresponds to the
 * expected format or pattern for a given token type. The token types
 * include various categories such as comments, brackets, operators,
 * spaces, names, literals, and macros.
 *
 * @param token_type The type of token to check against. This is an
 *                   enumeration value of DataFileToken::Type.
 * @param string The string to be validated against the specified token type.
 * @return true If the string matches the specified token type.
 * @return false If the string does not match the specified token type.
 */
bool DataFileToken::check(DataFileToken::Type token_type, const std::string& string) {
	switch (token_type) {
		case DataFileToken::Type::CommentBlockHash:
			return check_comment_block_hash(string);
		case DataFileToken::Type::CommentBlockCStyle:
			return check_comment_block_c_style(string);
		case DataFileToken::Type::CommentLineCStyle:
			return check_comment_line_c_style(string);
		case DataFileToken::Type::OpenCurlyBracket:
			return check_open_curly_bracket(string);
		case DataFileToken::Type::CloseCurlyBracket:
			return check_close_curly_bracket(string);
		case DataFileToken::Type::OpenParenthesis:
			return check_open_parenthesis(string);
		case DataFileToken::Type::CloseParenthesis:
			return check_close_parenthesis(string);
		case DataFileToken::Type::Operator: // note that we don't test for unary operators, regular operator will be promoted to unary if needed
			return check_operator(string);
		case DataFileToken::Type::Space:
			return check_space(string);
		case DataFileToken::Type::Name:
			return check_name(string);
		case DataFileToken::Type::LiteralString:
			return check_literal_string(string);
		case DataFileToken::Type::Int:
			return check_int(string);
		case DataFileToken::Type::Float:
			return check_float(string);
		case DataFileToken::Type::Macro:
			return check_macro(string);
		default:
			return false;
	}
}

/**
 * @brief Chooses the most appropriate token type from a set of possible types based on predefined rules.
 *
 * @param types A set of possible token types to choose from.
 * @param token_string The string representation of the token being evaluated.
 * @return The chosen token type based on the following rules:
 *         - If the 'Operator' type is present in the set, it is chosen.
 *         - If both 'Int' and 'Float' types are present, 'Int' is chosen.
 *         - If the set contains exactly 'Name' and 'Float', 'Float' is chosen.
 *         - If none of the above conditions are met, the function logs an error message
 *           and terminates the process.
 *
 * @note If the function encounters an ambiguous situation where it cannot choose a type,
 *       it outputs an error message listing the potential types and exits the program.
 *       A dummy return value is provided to avoid compilation errors, but it is never
 *       reached during normal execution.
 */
DataFileToken::Type DataFileToken::choose(const std::set<DataFileToken::Type>& types, const std::string & token_string) {
	// if token type 'operator' is possible, we choose it
	if (types.find(DataFileToken::Type::Operator) != types.end()) {
		return DataFileToken::Type::Operator;
	// if types Int and Floats are possible, choose Int
	} else if (types.find(DataFileToken::Type::Int) != types.end() &&
	           types.find(DataFileToken::Type::Float) != types.end()) {
		return DataFileToken::Type::Int;
	} else if (types.size() == 2 &&
	           types.find(DataFileToken::Type::Name) != types.end() &&
	           types.find(DataFileToken::Type::Float) != types.end()) {
		return DataFileToken::Type::Float;
	} else {
		Cerr << "Cannot choose with two potential types for token '" << token_string << "': ";
		for (DataFileToken::Type type: types)
			Cout << type << ' ';
		Cout << finl;

		Process::exit();
		return DataFileToken::Type::Space; // return dummy type to avoid compilation error
	}
}

/**
 * @brief Converts the DataFileToken object to its string representation.
 *
 * This method provides a string representation of the token based on its type.
 * It handles various token types such as integers, floats, brackets, comments,
 * macros, expressions, and literal strings. For tokens with assigned values,
 * it converts the stored value to a string. For tokens read from a file, it
 * returns the original string representation.
 *
 * @return A string representation of the token.
 *
 * Token types and their string representations:
 * - Int: Returns the integer value as a string or the original string if available.
 * - Float: Returns the float value as a string with precision or the original string if available.
 * - OpenCurlyBracket: Returns "{".
 * - CloseCurlyBracket: Returns "}".
 * - OpenParenthesis: Returns "(".
 * - CloseParenthesis: Returns ")".
 * - CommentBlockHash, CommentBlockCStyle, CommentLineCStyle: Returns the comment in "# ... #" format.
 * - Macro: Returns the macro in "#..." format.
 * - Expression: Converts the expression to a string using its `to_string` method.
 * - LiteralString: Returns the string enclosed in double quotes.
 * - Default: Returns the stored value as a string.
 */
std::string DataFileToken::to_string() const {
	switch (type) {
		case (DataFileToken::Type::Int):
			if (string == "") // token with assigned value
				return std::to_string(std::any_cast<int>(value));
			else  // token read from the file that still store the original string
				return string;
		case (DataFileToken::Type::Float):
			if (string == "") // token with assigned value
				// call the method to_string for double of DataFileToken to set the precision
				return DataFileToken::to_string(std::any_cast<double>(value));
			else  // token read from the file that still store the original string
				return string;
		case (DataFileToken::Type::OpenCurlyBracket):
			return "{";
		case (DataFileToken::Type::CloseCurlyBracket):
			return "}";
		case (DataFileToken::Type::OpenParenthesis):
			return "(";
		case (DataFileToken::Type::CloseParenthesis):
			return ")";
		case (DataFileToken::Type::CommentBlockHash):
		case (DataFileToken::Type::CommentBlockCStyle):
		case (DataFileToken::Type::CommentLineCStyle):
			// write every comment with '#' notation
			return "# " + std::any_cast<std::string>(value) + " #";
		case (DataFileToken::Type::Macro):
			return "#" + std::any_cast<std::string>(value);
		case (DataFileToken::Type::Expression): {
			DataFileExpression expression = std::any_cast<DataFileExpression>(value);
			return expression.to_string(); }
		case (DataFileToken::Type::LiteralString):
			return '"' + std::any_cast<std::string>(value) + '"';
		default:
			return std::any_cast<std::string>(value);
	}
}

/**
 * @brief Prints the details of the DataFileToken object to the output stream.
 *
 * This function outputs the type, value, start line, and start column of the
 * DataFileToken object. The output format depends on the type of the token:
 * - For `DataFileToken::Type::Int`, the value is cast to an integer.
 * - For `DataFileToken::Type::Float`, the value is cast to a double.
 * - For other types, the value is cast to a string.
 *
 * The output format is as follows:
 * `<type>(<value>):<start_line>:<start_column>`
 *
 * @note The function assumes that the `value` member contains a valid value
 *       that can be cast to the appropriate type using `std::any_cast`.
 */
void DataFileToken::print() const {
	switch (type) {
		case (DataFileToken::Type::Int):
			Cout << type << "(" << std::any_cast<int>(value) << "):"
			          << start_line << ":"
			          << start_column;
			break;
		case (DataFileToken::Type::Float):
			Cout << type << "(" << std::any_cast<double>(value) << "):"
			          << start_line << ":"
			          << start_column;
			break;
		default:
			Cout << type << "(" << std::any_cast<std::string>(value) << "):"
			          << start_line << ":"
			          << start_column;
			break;
	}
}

void DataFileToken::print_as_error(const std::string& message) const {
	print_as_message("Error", 31, message);
}

void DataFileToken::print_as_note(const std::string& message) const {
	print_as_message("Note", 32, message);
}

void DataFileToken::print_as_warning(const std::string& message) const {
	print_as_message("Warning", 33, message);
}

/**
 * @brief Prints a formatted message with source code context and highlights.
 *
 * This function generates a detailed error or informational message that includes
 * the location in the source file, the relevant lines of code, and highlights
 * the specific portion of the code related to the message. The output is formatted
 * with colors and indentation for readability.
 *
 * @param label A label describing the type of message (e.g., "Error", "Warning").
 * @param color The ANSI color code to use for highlighting the message and code.
 * @param message The detailed message to display.
 *
 * The function performs the following steps:
 * - Writes the file name and location (line and column range) where the issue occurred.
 * - Formats and appends the provided message.
 * - Reads the source file to extract the relevant lines of code.
 * - Highlights the specific portion of the code using the provided color.
 * - Shortens the output if the number of lines to display exceeds a predefined limit.
 *
 * Notes:
 * - Tabs in the source code are replaced with spaces for consistent formatting.
 * - If the highlighted range spans multiple lines, the underline adapts accordingly.
 * - The function ensures that the output is concise and skips excessive lines when necessary.
 *
 * Example usage:
 * ```
 * DataFileToken token;
 * token.print_as_message("Error", 31, "Syntax error detected.");
 * ```
 */
void DataFileToken::print_as_message(const std::string& label, int color, const std::string& message) const {
	std::ostringstream oss;

	// write localization
	oss << "In "
	          << TextFormatting::Err::start(0, 1)
	          << file_name << ":"
	          << start_line << ":"
			  << start_column << TextFormatting::Err::end()
			  << " to " << TextFormatting::Err::start(0, 1)
	          << end_line << ":"
			  << end_column << TextFormatting::Err::end() << ": ";

	// write the message
	oss << TextFormatting::Err::format(label + ": ", color, 1) << message << std::endl;

	// open file to get the source
	std::ifstream file(file_name);

	// get the correct line (the line before the first line
	size_t current_line_number = 0;
	std::string line_string;
	while (current_line_number < start_line && std::getline(file, line_string)) {
		current_line_number++;
	}

	int margin = 8;
	size_t tab_size = 4;
	int gap = 2;
	size_t max_lines_printed = 6;
	bool first_line_skip = true;

	for (size_t line = start_line; line <= end_line; line++) {
		// if there a more than max_lines_printed lines to print, we put '...' to shorten the error message
		size_t number_of_lines = end_line - start_line;
		if (number_of_lines > max_lines_printed) {
			size_t number_of_lines_start = max_lines_printed / 2;
			size_t number_of_lines_end = max_lines_printed - number_of_lines_start;
			size_t relative_line = line - start_line;

			if (relative_line >= number_of_lines_start &&
			    relative_line < number_of_lines - number_of_lines_end) {

				if (first_line_skip) {
					oss << std::endl << std::setw(margin + gap) << " "
					          << TextFormatting::Err::format("... (skip) ...", 33, 0)
							  << std::endl << std::endl;
					first_line_skip = false;
				}

				continue;
			}
		}

		// replace tabs with spaces
		std::string line_string_tabify = TextFormatting::tabify(line_string, tab_size);

		// compute underline position and size
		int first_column = 1;
		int last_column = static_cast<int>(line_string_tabify.size());

		if (line == start_line) {
			first_column = static_cast<int>(TextFormatting::tabify_get_index(start_column, line_string, tab_size));
		}
		if (line == end_line) {
			last_column = static_cast<int>(TextFormatting::tabify_get_index(end_column - 1, line_string, tab_size));
		}

		// print the line
		oss << TextFormatting::Err::start(32, 0)
		          << std::setw(margin) << line << ":"
				  << TextFormatting::Err::end()
		          << std::setw(gap) << " " << line_string_tabify.substr(0, first_column - 1);
		if (line_string_tabify.size() > 0) {
			oss << TextFormatting::Err::format(
				          line_string_tabify.substr(
						          first_column - 1,
								  last_column - first_column + 1),
				          color, 1)
				      << line_string_tabify.substr(last_column) << std::endl;
		}

		// write the underline
		oss << std::setw(margin + first_column + gap) << " ";
		oss << TextFormatting::Err::start(color, 1);
		for(int column = first_column; column <= last_column; column++) {
			// draw the '^' character on the first and last character
			if (line == start_line && column == first_column )
				oss << "^";
			else
				oss << "~";
		}
		oss << TextFormatting::Err::end() << std::endl;

		// read the next line
		std::getline(file, line_string);
	}

	// close the file
	file.close();

	Cerr << oss.str() << finl;;
}

bool DataFileToken::is_space() const {
	return type == DataFileToken::Type::Space;
}

bool DataFileToken::is_operator() const {
	return type == DataFileToken::Type::Operator ||
	       type == DataFileToken::Type::OperatorUnary;
}

/**
 * @brief Determines if the current token is an operand.
 *
 * This method checks the type of the token and returns true if the token
 * represents an operand. Operands are defined as tokens of type Int, Float,
 * or Name. For all other token types, this method returns false.
 *
 * @return true if the token is an operand (Int, Float, or Name), false otherwise.
 */
bool DataFileToken::is_operand() const {
	switch (type) {
		case DataFileToken::Type::Int:
		case DataFileToken::Type::Float:
		case DataFileToken::Type::Name:
			return true;
		default:
			return false;
	}
}

bool DataFileToken::is_parenthesis() const {
	switch (type) {
		case DataFileToken::Type::OpenParenthesis:
		case DataFileToken::Type::CloseParenthesis:
			return true;
		default:
			return false;
	}
}

bool DataFileToken::is_curly_bracket() const {
	switch (type) {
		case DataFileToken::Type::OpenCurlyBracket:
		case DataFileToken::Type::CloseCurlyBracket:
			return true;
		default:
			return false;
	}
}

bool DataFileToken::is_comment() const {
	switch (type) {
		case DataFileToken::Type::CommentBlockHash:
		case DataFileToken::Type::CommentBlockCStyle:
		case DataFileToken::Type::CommentLineCStyle:
			return true;
		default:
			return false;
	}
}

/**
 * @brief Determines if the current token represents a mathematical entity.
 *
 * This method checks the type of the token and returns true if the token
 * is of type Int, Float, or Expression, indicating that it represents
 * a mathematical value or expression. Otherwise, it returns false.
 *
 * @return true if the token is a mathematical entity (Int, Float, or Expression),
 *         false otherwise.
 */
bool DataFileToken::is_math() const {
	switch (type) {
		case DataFileToken::Type::Int:
		case DataFileToken::Type::Float:
		case DataFileToken::Type::Expression:
			return true;
		default:
			return false;
	}
}

bool DataFileToken::is_plus_or_minus() const {
	return (is_operator() &&
	   (string == "+" ||
	    string == "-"));
}

bool DataFileToken::is_name() const {
	return type == DataFileToken::Type::Name;
}

/**
 * @brief Determines if the current token represents a mathematical function.
 * 
 * This method checks if the token is a valid name and matches one of the 
 * predefined mathematical function names. The comparison is case-insensitive.
 * 
 * @return true if the token represents a mathematical function, false otherwise.
 * 
 * The recognized mathematical functions are:
 * - sin
 * - cos
 * - tan
 * - asin
 * - acos
 * - atan
 * - sinh
 * - cosh
 * - tanh
 * - exp
 * - ln
 * - log10
 * - sqrt
 * - abs
 * - sign
 * - rand
 * - int
 * - float
 */
bool DataFileToken::is_function() const {
	if (!is_name())
		return false;
	
	std::string lower = TextFormatting::lower(string);
	if (lower == "sin"   ||
	    lower == "cos"   ||
		lower == "tan"   ||
		lower == "asin"  ||
		lower == "acos"  ||
		lower == "atan"  ||
		lower == "sinh"  ||
		lower == "cosh"  ||
		lower == "tanh"  ||
		lower == "exp"   ||
		lower == "ln"    ||
		lower == "log10" ||
		lower == "sqrt"  ||
		lower == "abs"   ||
		lower == "sign"  ||
		lower == "rand"  ||
		lower == "int"   ||
		lower == "float")
		return true;
	else
		return false;
}

bool DataFileToken::is_expression() const {
	return type == DataFileToken::Type::Expression;
}

/**
 * @brief Determines the precedence level of the current operator token.
 *
 * This method evaluates the precedence of the token based on its type and string value.
 * It is used to establish the order of operations for operators in expressions.
 *
 * @return An integer representing the precedence level of the token.
 *         Higher values indicate higher precedence.
 *
 * @note The precedence levels are defined as follows:
 *       - Relational operators (">", "<", ">=", "<="): 0
 *       - Logical operators ("min", "max"): 1
 *       - Addition and subtraction ("+", "-"): 2
 *       - Multiplication and division ("*", "/"): 3
 *       - Exponentiation ("^"): 4
 *       - Composition ("c"): 6
 *       - Unary operators ("+", "-"): 4
 *
 * @throws If the token is not a valid operator or unary operator, an error message
 *         is printed, and the program exits.
 * @throws If the token is not of type Operator or OperatorUnary, an error message
 *         is printed, and the program exits.
 */
int DataFileToken::precedence() const {
	if (type == DataFileToken::Type::Operator) {
		if      (string == ">")
			return 0;
		else if (string == "<")
			return 0;
		else if (string == ">=")
			return 0;
		else if (string == "<=")
			return 0;
		else if (string == "^") // exponent
			return 4;
		else if (string == "c") // composition
			return 6;
		else if (string == "min")
			return 1;
		else if (string == "max")
			return 1;
		else if (string == "*")
			return 3;
		else if (string == "/")
			return 3;
		else if (string == "+")
			return 2;
		else if (string == "-")
			return 2;
		else {
			print_as_error("Invalid operator.");
			Process::exit(DONT_PRINT_TOKEN);
			return 0;
		}
	} else if (type == DataFileToken::Type::OperatorUnary) {
		if (string == "+")
			return 4;
		else if (string == "-")
			return 4;
		else {
			print_as_error("Invalid unary operator.");
			Process::exit(DONT_PRINT_TOKEN);
			return 0;
		}
	} else {
		print_as_error("This should have been an operator.");
		Process::exit(DONT_PRINT_TOKEN);
		return 0;
	}
}

/**
 * @brief Determines the associativity of the operator represented by the DataFileToken.
 *
 * @details This method checks the type of the token and determines whether the operator
 *          is left-associative ('l') or right-associative ('r'). It assumes that the token
 *          is of type `Operator` or `OperatorUnary`. If the token is not an operator, an
 *          error message is printed, and the program exits.
 *
 * @return A character indicating the associativity:
 *         - 'r' for right-associative operators or unary operators.
 *         - 'l' for left-associative operators.
 *
 * @note The method asserts that the token type is `Operator` before proceeding.
 *       If the token type is invalid, the program will terminate.
 */
char DataFileToken::associativity() const {
	assert(type == DataFileToken::Type::Operator);

	if (type == DataFileToken::Type::Operator) {
		if      (string == "^" || string == "c")
			return 'r'; // right
		else
			return 'l'; // left
	} else if (type == DataFileToken::Type::OperatorUnary) {
		return 'r'; // right
	} else {
		print_as_error("This should have been an operator.");
		Process::exit(DONT_PRINT_TOKEN);
		return 'r'; // return something to avoid compilation error
	}
}


