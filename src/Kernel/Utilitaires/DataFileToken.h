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
#ifndef DataFileToken_included
#define DataFileToken_included

#include <set>
#include <list>
#include <any>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>

#include <TextFormatting.h> 
#include <EntreeSortie.h> 

class DataFileToken {
		
		/////////////////
		// Token types //
		/////////////////
	
	public:

		enum Type {
			CommentBlockHash,
			CommentBlockCStyle,
			CommentLineCStyle,
			OpenCurlyBracket,
			CloseCurlyBracket,
			OpenParenthesis,
			CloseParenthesis,
			Operator,
			OperatorUnary,
			Space,
			Name,
			LiteralString,
			Int,
			Float,
			Macro,
			Expression	
		};

		// Method that return a set of all token types
		static std::set<Type> get_types_full_set();

		// Use operator<< to print token type
		friend std::ostream& operator<<(std::ostream& os, const Type& type);
		friend Sortie& operator<<(Sortie& os, const Type& type);
		
		// Methods that check if a given string string is a valid representation
		// a of each type.
		// Those methods assume for a string of N+1 character that the N first characters
		// of the string give a valid representation of the type, and only test the last
		// character.
		// For example, if '10.38' is a valid representation of a Float, the '10.384' 
		// will return true.
		static bool check_comment_block_hash(const std::string& token_string);
		static bool check_comment_block_c_style(const std::string& token_string);
		static bool check_comment_line_c_style(const std::string& token_string);
		static bool check_open_curly_bracket(const std::string& token_string);
		static bool check_close_curly_bracket(const std::string& token_string);
		static bool check_open_parenthesis(const std::string& token_string);
		static bool check_close_parenthesis(const std::string& token_string);
		static bool check_operator(const std::string& token_string);
		static bool check_space(const std::string& token_string);
		static bool check_name(const std::string& token_string);
		static bool check_literal_string(const std::string& token_string);
		static bool check_int(const std::string& token_string);
		static bool check_float(const std::string& token_string);
		static bool check_macro(const std::string& token_string);

		// this last method test for any type
		static bool check(Type type, const std::string& token_string);

		// choose the DataFileToken::Type to keep among a set of types
		static Type choose(const std::set<Type>& types, const std::string & string);

		std::string to_string() const;

	public:
	
		/////////////////
		// constructor //
		/////////////////

	public:
		
		DataFileToken(
				const std::string& file_name_,
				size_t start_line_,
				size_t start_column_,
				size_t end_line_,
				size_t end_column_,
				Type type_,
				std::string string_); 

		/////////////
		// methods //
		/////////////

		void print() const;

	private:

		void print_as_message(const std::string& label, int color, const std::string& message) const;
	
	public:

		void print_as_error(const std::string& message) const;
		void print_as_note(const std::string& message) const;
		void print_as_warning(const std::string& message) const;

		std::string str() const;
		bool is_space() const;
		bool is_operator() const;
		bool is_operand() const;
		bool is_parenthesis() const;
		bool is_curly_bracket() const;
		bool is_comment() const;
		bool is_math() const;
		bool is_plus_or_minus() const;
		bool is_name() const;
		bool is_function() const;
		bool is_expression() const;

		// for operator manipulations in expressions
		int precedence() const;
		char associativity() const;

		// convert a double to string representation
		static std::string to_string(double value);

	private:

		static bool isletter(char c);

		////////////////
		// attributes //
		////////////////

	public:
		
		std::string file_name;
		size_t start_line, start_column, end_line, end_column;

		Type type;          // type of this token
		std::string string; // string representation of token
		std::any value;     // value of this token
};

#endif
