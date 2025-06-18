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
#ifndef TEXT_FORMATTING_H
#define TEXT_FORMATTING_H

#include <unistd.h>
#include <algorithm>
#include <string>
#include <sstream>

/*
 * Contains functions to format text using ANSI escapement sequences.
 * Also contain methods to emulate tab formatting in a terminal.
 *
 * The main purpose of the formatting method is to check the stream redirection before 
 * applying the formatting ANSI sequences: if the stream does not go to a terminal,
 * then no formatting occure.
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
	
	std::string start(FILE* stream, int color, int mode);
	std::string end(FILE* stream);
	std::string format(FILE* stream, const std::string& message, int color, int mode);

	namespace Out {
		std::string start(int color, int mode);
		std::string end();
		std::string format(const std::string& message, int color, int mode);
	}

	namespace Err {
		std::string start(int color, int mode);
		std::string end();
		std::string format(const std::string& message, int color, int mode);
	}

	/*
	 * @brief Take a string as input and return a string that replace the tabs
	 *        with spaces, 'rounding' up to the next tab each time like in a terminal.
	 * @return (std::string) Formatted string.
	 */
	std::string tabify(const std::string& string, size_t tab_size = 4);
	
	/*
	 * @brief Compute the new index of a character of a string after the tabify
	 *        function has been applied.
	 * @return (size_t) New index.
	 */
	size_t tabify_get_index(size_t original_index, const std::string& string, size_t tab_size = 4);

	// convert a string to lowercase
	std::string lower(std::string string);
}

#endif
