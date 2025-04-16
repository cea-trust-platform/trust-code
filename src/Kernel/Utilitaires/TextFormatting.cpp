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
#include <TextFormatting.h>

/**
 * @namespace TextFormatting
 * @brief Provides utilities for text formatting, including color and style
 *        manipulation for terminal output, tab replacement, and string
 *        transformations.
 * Contains functions to format text using ANSI escapement sequences.
 * Also contain methods to emulate tab formatting in a terminal.
 *
 * The main purpose of the formatting method is to check the stream redirection before 
 * applying the formatting ANSI sequences: if the stream does not go to a terminal,
 * then no formatting occurs.
 *
 * For example
 *
 *     std::cout << "This is unformatted"
 *               << TextFormat::Out::format(" but this is bold and red", 31, 1)
 *               << "will this is unformatted." << std::endl;
 *
 * will print the message "Test message" in bold-red only if std::cout go to the terminal.
 * It is also possible to avoid reseting the formatting using the 'start' and
 * 'end' (reset) functions.
 *
 * The functions are provided for Out (stdout) and Err (stderr) streams.
 */
namespace TextFormatting {
	
	std::string start(FILE* stream, int color, int mode) {
		std::stringstream oss;
		if (isatty(fileno(stream))) {
			if (mode == 0)
				oss << "\033[" << color << "m";
			else
				oss << "\033[" << color << ";" << mode << "m";
			return oss.str();
		} else
			return "";
	}
		
	std::string end(FILE* stream) {
		if (isatty(fileno(stream))) {
			return "\033[0m";
		} else
			return "";
	}
		
	std::string format(FILE* stream, const std::string& message, int color, int mode) {
		return start(stream, color, mode) + message + end(stream);
	}

	namespace Out {
		std::string start(int color, int mode) {
			return TextFormatting::start(stdout, color, mode);
		}

		std::string end() {
			return TextFormatting::end(stdout);
		}

		std::string format(const std::string& message, int color, int mode) {
			return TextFormatting::format(stdout, message, color, mode);
		}
	}

	namespace Err {
		std::string start(int color, int mode) {
			return TextFormatting::start(stderr, color, mode);
		}

		std::string end() {
			return TextFormatting::end(stderr);
		}

		std::string format(const std::string& message, int color, int mode) {
			return TextFormatting::format(stderr, message, color, mode);
		}
	}

	/**
	 * @brief Replaces tab characters in a string with spaces, ensuring proper alignment based on the specified tab size.
	 * 
	 * @param string The input string that may contain tab characters.
	 * @param tab_size The number of spaces that represent a single tab stop.
	 * @return A new string where all tab characters are replaced with the appropriate number of spaces.
	 * 
	 * @details
	 * This function processes the input string character by character. When a tab character ('\t') is encountered,
	 * it calculates the number of spaces required to align to the next tab stop and appends those spaces to the output.
	 * For newline characters ('\n'), it increments the column count and appends a space. All other characters are
	 * directly appended to the output string.
	 */
	std::string tabify(const std::string& string, size_t tab_size) {
		std::string out;
	
		size_t column = 0;
		for (char c: string) {
			if (c == '\t') {
				size_t number_of_spaces = ((column / tab_size) + 1) * tab_size - column;
				for (size_t index = 0; index < number_of_spaces; index++)
					out += " ";
	
				column += number_of_spaces;
			} else if (c == '\n') {
				column++;
				out += " ";
			} else {
				out += c;
				column++;
			}
		}
	
		return out;
	}

	/**
	 * @brief Computes the new index in a string where tab characters are expanded to spaces.
	 *
	 * This function calculates the adjusted index in a string when tabs are replaced
	 * with spaces, based on a specified tab size. It iterates through the string,
	 * accounting for the expansion of tab characters and other characters until the
	 * original index is reached.
	 *
	 * @param original_index The original index in the string to be adjusted.
	 * @param string The input string containing characters, including potential tab characters.
	 * @param tab_size The number of spaces a tab character should expand to.
	 * @return The adjusted index in the string after accounting for tab expansion.
	 */
	size_t tabify_get_index(size_t original_index, const std::string& string, size_t tab_size) {

		size_t new_index = 0;
		for (char c: string) {
			if (original_index == 0)
				return new_index;

			if (c == '\t') {
				size_t number_of_spaces = ((new_index / tab_size) + 1) * tab_size - new_index;

				new_index += number_of_spaces;
			} else if (c == '\n') {
				new_index++;
			} else {
				new_index++;
			}

			original_index--;
		}

		return new_index;
	}

	// convert a string to lowercase
	std::string lower(std::string string) {
		std::transform(
			string.begin(),
			string.end(),
			string.begin(),
			[](unsigned char c) {
				return std::tolower(c);
			}
		);

		return string;
	}
}
