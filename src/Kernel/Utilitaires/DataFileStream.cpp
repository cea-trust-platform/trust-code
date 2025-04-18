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

#include <algorithm>
#include <DataFileStream.h>

// initialize the tokens list with an empty list and the current list iterator
DataFileStream::Tokens DataFileStream::tokens = Tokens();
DataFileStream::Tokens::iterator DataFileStream::current_token_iterator = tokens.begin();
size_t DataFileStream::tokens_to_skip = 0;

DataFileStream::DataFileStream() :
  Entree(), istrstream_(0)
{
  set_check_types(1);
}

DataFileStream::DataFileStream(const std::string& string, const Tokens& tokens_) :
  Entree(),  istrstream_(0)
{
  set_check_types(1);
  init(string, tokens_);
}

DataFileStream::~DataFileStream()  { }

/**
 * @brief Initializes the DataFileStream with a given input string and a list of tokens.
 *
 * This function sets up the DataFileStream by copying the provided list of tokens
 * and initializing an input string stream with the given string. It also sets the
 * current token iterator to the first non-space or non-comment token in the list.
 *
 * @param string The input string to be processed by the DataFileStream.
 * @param tokens_ The list of tokens to be used for parsing the input string.
 *
 * @note The first token is skipped during the next read operation.
 * @note If an existing input string stream is present, it will be deleted before
 *       creating a new one.
 */
void DataFileStream::init(const std::string& string, const Tokens& tokens_)
{
	// takes a string and a list of tokens (words in the string and tokens list should match)

	Cerr << "Initialize the DataFileStream" << finl;
	// copy the list of tokens
	tokens = tokens_;

	// set the current token iterator to the first non-space token or non-comment
	current_token_iterator = tokens.begin();
	while (std::next(current_token_iterator) != tokens.end() && (current_token_iterator->is_space() || current_token_iterator->is_comment()))
	  	current_token_iterator++;

	// sinc the first token is already set, we will skip the next read
	tokens_to_skip = 1;

	// creat a istringstream with the given input string
  if (istrstream_)
    delete istrstream_;
  istrstream_ = new istringstream(string);  // a copy of str is taken
  set_istream(istrstream_);
}

Entree& DataFileStream::operator>>(True_int& ob) { return operator_template<True_int>(ob); }
Entree& DataFileStream::operator>>(double& ob) { return operator_template<double>(ob); }

/**
 * @brief Reads data into the provided buffer while maintaining synchronization
 *        with the internal token iterator and handling spaces and comments.
 *
 * This function overrides the `get` method to ensure that the internal token
 * iterator of the `DataFileStream` remains synchronized with the input stream.
 * It skips over spaces and comments in the token stream and adjusts the number
 * of tokens to skip based on the spaces in the current token's string.
 *
 * @param ob Pointer to the buffer where the data will be stored.
 * @param bufsize The size of the buffer.
 * @return The number of characters read into the buffer.
 */
int DataFileStream::get(char *ob, std::streamsize bufsize)
{
	// Increment the current_token_index and skip spaces
	// Some tokens may store in their string spaces. We must avoid incrementing the
	// token iterator as much as the number of spaces in that literal string
	// so that the internal istringstream of DataFileStream stay synchronyze with
	// the token iterator
	if (tokens_to_skip == 0 && current_token_iterator != tokens.end() && std::next(current_token_iterator) != tokens.end()) {
		current_token_iterator++;
	  	while (std::next(current_token_iterator) != tokens.end() && (current_token_iterator->is_space() || current_token_iterator->is_comment())) {
	  		current_token_iterator++;
		}

		// count the number of spaces and set this number as the number of
		// tokens to skip
		std::string str = current_token_iterator->string;
		tokens_to_skip = static_cast<size_t>(std::count(str.begin(), str.end(), ' '));

	} else if (tokens_to_skip > 0) {
		tokens_to_skip--;
	}

  int ret = Entree::get(ob,bufsize);
  return ret;
}
