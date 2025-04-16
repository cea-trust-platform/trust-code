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

#include <DataFile.h>
#include <EntreeSortie.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Type_Verifie.h>
#include <Type_info.h>

Implemente_instanciable_sans_constructeur(DataFile,"DataFile",Lec_Diffuse_base);

Entree& DataFile::readOn(Entree& s)
{
  throw;
}

Sortie& DataFile::printOn(Sortie& s) const
{
  throw;
}

// constructors
DataFile::DataFile() {}

DataFile::DataFile(const char* root_data_file_name, IOS_OPEN_MODE mode, bool do_check_obsolete_keywords_):
		do_check_obsolete_keywords(do_check_obsolete_keywords_) {
	ouvrir(root_data_file_name, mode);
}
	
int DataFile::ouvrir(const char* root_data_file_name, IOS_OPEN_MODE mode) {
	// only the master process load the file
	int ok = 0;

	if (Process::je_suis_maitre()) {
	
		Cerr << "Opening the data file" << finl;
	
		// register this file as included
		included_files.push_back(root_data_file_name);
	
		tokens = process(root_data_file_name, mode);
	
		// check syntax
		check_curly_brackets(); // only check after the includes has been made

		// check obsolete keywords
		if (do_check_obsolete_keywords)
			check_obsolete_keywords();

		// check reserved keyword
		check_reserved_keywords();
	
		// set this as the EChaine data
		data.init(to_string(tokens), tokens);
	
		// we also store the result in an file for debugging
		write(tokens, "." + std::string(root_data_file_name) + ".parsed");

		ok = 1;
	}

	// share data accross processors
	envoyer_broadcast(ok, 0);

	return ok;
}

/**
 * @brief Processes a file by tokenizing its content, analyzing the tokens, 
 *        and applying macros to compute expressions.
 * 
 * @param file_name The name of the file to process.
 * @param mode The mode in which the file should be opened (e.g., read or write).
 * @return DataFile::Tokens A list of processed tokens representing the file's content.
 * 
 * This function performs the following steps:
 * 1. Tokenizes the content of the specified file.
 * 2. Adds a dummy initial space token to handle cases where the file starts with an expression.
 * 3. Iterates through the tokens, applying macros and computing expressions as needed.
 * 
 * Note:
 * - The function uses a while loop to handle cases where applying a macro modifies 
 *   the size of the token list, ensuring the iteration remains valid.
 * - The function returns the final list of processed tokens.
 */
DataFile::Tokens DataFile::process(const std::string& file_name, IOS_OPEN_MODE mode) {
	// first get the token for that file
	std::list<DataFileToken> tokens_output = tokenize(file_name, mode);

	// add a dummy first space so that the following work in case the file start
	// with an expression
	tokens_output.push_front(DataFileToken("", 0, 0, 0, 0, DataFileToken::Type::Space, ""));

	// then analyze the tokens to compute expressions and apply macros
	// (use a while loop to avoid calling next with token == tokens_output.end())
	auto token = tokens_output.begin();
	while (token != tokens_output.end()) {
		
		if (token->type == DataFileToken::Type::Macro) {
			token = apply_macro(tokens_output, token, mode);
		} 

		// macro may have change the size of tokens_output and know token is equal o tokens_output.end()
		if (token != tokens_output.end())
			token = next(tokens_output, token);
	}

	return tokens_output;
}

/**
 * @brief Tokenizes the content of a file into a list of DataFileToken objects.
 *
 * This function reads the content of the specified file and breaks it into tokens
 * based on predefined token types. It handles various token types such as names,
 * operators, spaces, comments, and more. The function also tracks the line and column
 * positions of each token for precise error reporting and debugging.
 *
 * @param file_name The name of the file to be tokenized.
 * @param mode The mode in which the file should be opened (e.g., read or write).
 * @return A list of DataFileToken objects representing the tokens found in the file.
 *
 * @note If the file cannot be opened, the function will terminate the process.
 * @note If an unclosed comment block is encountered, the function will report an error
 *       and terminate the process.
 * @note The function stops reading further tokens if it encounters the keywords "fin" or "end".
 *
 * @throws The function terminates the process in case of critical errors such as:
 *         - Failure to open the file.
 *         - Parsing mistakes.
 *         - End-of-file reached inside a comment block.
 *         - Unprocessed token types at the end of the file.
 */
std::list<DataFileToken> DataFile::tokenize(const std::string& file_name, IOS_OPEN_MODE mode) const {
	// the output
	std::list<DataFileToken> output_tokens;

	// open the file
	EFichier file;
	if (!file.ouvrir(file_name.c_str(), mode)) {
		Cerr << "Failed to open file '" << file_name << "'." << finl;
		Process::exit(DONT_PRINT_TOKEN);
	}

	// read the first character
	std::string token_string;
	char character;
	if (!file.get(character)) {
		// the file is empty, we can return
		file.close();
		return output_tokens;
	}

	std::set<DataFileToken::Type> potential_token_types = DataFileToken::get_types_full_set();

	// temporaty stored valid potential tokens
	std::set<DataFileToken::Type> new_potential_token_types;

	size_t line = 1;
	size_t column = 1;
	size_t token_start_line = line;
	size_t token_start_column = column;
	while (true) {
		// for each potential token types, check if the new token_string is a valid type
		new_potential_token_types.clear();
		for (DataFileToken::Type token_type: potential_token_types) {
			if (DataFileToken::check(token_type, token_string + character)) {
				new_potential_token_types.insert(token_type);
			}
		}

		if (new_potential_token_types.empty()) {

			// get the token type
			DataFileToken::Type token_type = DataFileToken::Type::Space;
			if		(potential_token_types.size() == 0) {
				Cerr << "A mistake occure during parsing." << finl;
				Process::exit(DONT_PRINT_TOKEN);
			} else if (potential_token_types.size() == 1) {
				token_type = *potential_token_types.begin();
			} else {
				// choose among the potential token types the most appropriate
				token_type = DataFileToken::choose(potential_token_types, token_string);
			}

			// its possible than some names could be interpreted has operators
			// (min, max…), so correct the type if it is the case
			if (token_type == DataFileToken::Type::Name) {
				if (DataFileToken::check_operator(token_string))
					token_type = DataFileToken::Type::Operator;
			}

			// append the token to the tokens vector
			DataFileToken token(
					file_name,
					token_start_line,
					token_start_column,
					line,
					column,
					token_type,
					token_string);
			output_tokens.push_back(token);

			// capture the 'end' keyword to stope reading
			if (token_type == DataFileToken::Type::Name) {
				// if the keyword is name as is 'fin' or 'end', we stop reading
				// only to it after the occurent of the word to keep in the tokens list
				std::string lower_string = TextFormatting::lower(token_string);
				if (lower_string == "fin" || lower_string == "end") {
					file.close();
					return output_tokens;
				}
			}

			// reset the token_string and the potential_token_types
			token_string = "";
			potential_token_types = DataFileToken::get_types_full_set();

			token_start_line = line;
			token_start_column = column;
		} else {
			// add the previously read character to the current token_string
			token_string += character;

			// swap the new_potential_token_types with potential_token_types
			std::swap(potential_token_types, new_potential_token_types);

			// update the cursor
			if (character == '\n') {
				line++;
				column = 1;
			} else {
				column++;
			}

			// read a new character
			if (!file.get(character))
				break;
		}
	}

	file.close();

	// check for unclosed comments
	if (!potential_token_types.empty()) {
		if (potential_token_types.size() == 1) {

			// get the token type
			DataFileToken::Type token_type = *potential_token_types.begin();

			// create a token with the last token properties
			DataFileToken token(
					file_name,
					token_start_line,
					token_start_column,
					line,
					column,
					token_type,
					token_string);

			switch (token_type) {
				case DataFileToken::Type::CommentBlockHash:
				case DataFileToken::Type::CommentBlockCStyle:
					token.print_as_error("End-of-file reached inside a comment.");
					Process::exit(DONT_PRINT_TOKEN);
					break;
				case DataFileToken::Type::Name:
				case DataFileToken::Type::Space:
				case DataFileToken::Type::OpenParenthesis:
				case DataFileToken::Type::CloseParenthesis:
					// add the last token
					output_tokens.push_back(token);
					break;
				default:
					token.print_as_error("End-of-file while still been process.");

					Process::exit(DONT_PRINT_TOKEN);
			}
		} else {
			Cerr << "End-of-file error with token types ";
			for (auto& type: potential_token_types) {
				Cerr << type << " ";
			}
			Cerr << "still not process." << finl;

			if (output_tokens.size() > 0)
				output_tokens.back().print_as_note("After this token.");

			Process::exit(DONT_PRINT_TOKEN);
		}
	}

	file.close();
	return output_tokens;
}

/**
 * @brief Replaces a token with its corresponding constant value if it exists in the constants map.
 * 
 * This function checks if the provided token is of type `Name`. If it is, the function looks up
 * the token's string value in the `constants` map. If a matching constant is found, the token's
 * type and value are updated to match the type and value of the constant.
 * 
 * @param token An iterator pointing to the token to be replaced. The token's type and value
 *              may be modified if a matching constant is found.
 * 
 * @note If the token is not of type `Name` or if no matching constant is found in the `constants`
 *       map, the function does nothing.
 */
void DataFile::replace(Tokens::iterator& token) const {
	// if the token is not a Name, it cannot be a constant
	if (token->type != DataFileToken::Type::Name)
		return;

	if (auto it = constants.find(token->string); it != constants.end()) {
		token->type = it->second.second.type;
		token->value = it->second.second.value;

		// and set the string to "" to signify that the value changed
		token->string = "";
	}
}

/**
 * @brief Processes the next token in a sequence of tokens, handling expressions and ensuring
 *        syntactic correctness.
 *
 * This function advances the iterator to the next token in the provided sequence of tokens.
 * If the next token is an open parenthesis, it processes the entire expression enclosed by
 * the parentheses, validates its syntax, and replaces it with a single token representing
 * the expression. The function ensures that the expression adheres to the expected rules
 * for operators, operands, and parentheses.
 *
 * @param input_tokens The sequence of tokens to process.
 * @param token An iterator pointing to the current token in the sequence.
 *              It must not point to `input_tokens.end()`.
 * @return An iterator pointing to the next token after processing.
 *
 * @note This function performs several validations:
 *       - Ensures that expressions are syntactically correct.
 *       - Validates alternation between operators and operands.
 *       - Checks for mismatched or misplaced parentheses.
 *       - Ensures no invalid token types are present in expressions.
 *       - Handles unary operators and function composition.
 *
 * @throws The function terminates the program with an error message if:
 *         - An invalid token type is encountered in an expression.
 *         - Parentheses are mismatched or misplaced.
 *         - Consecutive operators or operands are found where not allowed.
 *         - An operand is followed directly by an open parenthesis without an operator.
 *         - The end of the file is reached before the expression is closed.
 *
 * @details If the next token is an expression, the function:
 *          - Identifies the range of tokens forming the expression.
 *          - Validates the expression's syntax.
 *          - Replaces the tokens of the expression with a single token representing it.
 *          - If the expression is numeric, replaces it with its computed value.
 *
 * @pre The `token` iterator must not point to `input_tokens.end()`.
 * @post The `token` iterator is updated to point to the next token after processing.
 */
DataFile::Tokens::iterator DataFile::next(Tokens& input_tokens, Tokens::iterator token) const {
	// should not be call on input_tokens.end()
	assert(token != input_tokens.end());

	// go to the next token
	token = std::next(token);

	// replace it with constant if possible
	replace(token);

	// if the next token is not a parenthesis, then it is not an expression
	if (token->type != DataFileToken::Type::OpenParenthesis) {
		return token;
	}

	// it is an expression, we search the closing parenthesis
	int number_of_consecutive_operators = 0;
	std::optional<Tokens::iterator> last_operator_or_operand;
	std::optional<Tokens::iterator> previous_non_space;
	bool last_was_close_parenthesis = false;

	size_t depth = 0;
	auto start = token;
	auto end = token;
	do {
		// replace with constant if possible
		replace(end);

		// check the depth
		if (end->type == DataFileToken::Type::OpenParenthesis) {
			depth++;
		} else if (end->type == DataFileToken::Type::CloseParenthesis) {
			depth--;
			if (depth == 0) { // reach the end of the expression {
				break;
			}
		}

		// check the type
		if (!end->is_space() && !end->is_operand() && !end->is_operator() && !end->is_parenthesis() && !end->is_name()) {
			std::ostringstream oss;
			oss << end->type;
			end->print_as_error("An expression cannot contain a value type '" +
						TextFormatting::Err::format(oss.str(), 0,1) + "'.");
			Process::exit(DONT_PRINT_TOKEN);
		}

		// if the previous token was a ')' and the new token is '(', then it is not a
		// valid expression
		if (end->type == DataFileToken::Type::OpenParenthesis && last_was_close_parenthesis) {
			end->print_as_error("')' cannot be followed by a '(' inside an expression.");
			Process::exit(DONT_PRINT_TOKEN);
		}

		// if it is not a space nor a parenthesis, check if we alternate between operands
		// and operators
		if (!end->is_space() && !end->is_parenthesis()) {
			if (last_operator_or_operand.has_value()) {
				// expressions can have two consecutives operators if the second one is
				// the unary + or - operator
				if (end->is_operator() && last_operator_or_operand.value()->is_operator()) {
					if (number_of_consecutive_operators >= 2) {
						end->print_as_error("Cannot have a unary operator following two other operators.");
						last_operator_or_operand.value()->print_as_note("Previous operator.");
						Process::exit(DONT_PRINT_TOKEN);

					} else if (!end->is_plus_or_minus())  {
						end->print_as_error("Cannot have two following binary operators.");
						last_operator_or_operand.value()->print_as_note("Previous operator.");
						Process::exit(DONT_PRINT_TOKEN);
					}
				}
				// if two consecutive operands
				else if (end->is_operand() && last_operator_or_operand.value()->is_operand()) {
					end->print_as_error("Cannot have two consecutive operands in an expression.");
					last_operator_or_operand.value()->print_as_note("The previous operand.");
					Process::exit(DONT_PRINT_TOKEN);
				}
			}

			// check if it is a function
			if (end->is_function()) {
				// insert a composition operator after the function
				// we set the same file and position then the function for error
				// messages
				input_tokens.insert(
						std::next(end),
						DataFileToken(
								end->file_name,
								end->start_line,
								end->start_column,
								end->end_line,
								end->end_column,
								DataFileToken::Type::Operator,
								"c")); // 'c' for composition
				end = std::next(end);
				number_of_consecutive_operators = 0;
			}

			last_operator_or_operand = end;
			if (end->is_operator()) {
				number_of_consecutive_operators++;
			}
			else
				number_of_consecutive_operators = 0;
		} else {
			if (previous_non_space.has_value()) {
				// if operand followed by an open parenthesis
				if (previous_non_space.value()->is_operand() && end->type == DataFileToken::Type::OpenParenthesis) {
					end->print_as_error("Missing operator between the open parenthesis and the previous operand.");
					previous_non_space.value()->print_as_note("Previous operand.");
					Process::exit(DONT_PRINT_TOKEN);
				}
			}
		}

		if (!end->is_space())
			previous_non_space = end;

		// update the last_was_close_parenthesis flag
		if (end->type != DataFileToken::Type::Space)
			last_was_close_parenthesis = (end->type == DataFileToken::Type::CloseParenthesis);

		// update end
		end = std::next(end);
	} while (end != input_tokens.end());

	if (end == input_tokens.end() && depth != 0) {
		std::prev(end)->print_as_error("End-of-file before the end of the expression.");
		Process::exit(DONT_PRINT_TOKEN);
	}

	// then create an token expression from those input_tokens
	DataFileToken token_expression(
		start->file_name,
		start->start_line,
		start->start_column,
		end->end_line,
		end->end_column,
		DataFileToken::Type::Expression,
		"" // we don't use the string representation of the expression
	);

	// create list of the tokens for that expression and create the expression object
	assert(end != input_tokens.end());
	std::list tokens_expression(start, std::next(end));

	DataFileExpression expression(tokens_expression.begin(), std::prev(tokens_expression.end()));

	// set the expression as value of the token_expression and replace the token byt the expression value if the expression could be fully evaluated (without unknown field in it)
	token_expression.value = expression;

	if (expression.is_numeric())
		token_expression = expression.value();

	// replace the tokens from the expression with token_expression, encapsulating
	// all the tokens of the expression
	// first add the token_expression after end, otherwise we would not have a
	// valid iterator to insert it after
	auto inserted = input_tokens.insert(std::next(end), token_expression);
	// then erase the tokens of the expression
	input_tokens.erase(start, inserted);

	// and set end as the inserted element
	end = inserted;

	// then go to the end of the expression to continue
	token = end;
	return token;
}

/**
 * @brief Processes a macro directive in the input tokens and applies the corresponding action.
 * 
 * This function handles two types of macros: `#include` and `#define`. It modifies the input tokens
 * list based on the macro's behavior and updates internal state as necessary.
 * 
 * @param input_tokens The list of tokens to process.
 * @param token An iterator pointing to the macro token in the input tokens list.
 * @param mode The mode in which the file is being processed (e.g., read or write).
 * @return DataFile::Tokens::iterator An iterator pointing to the next token after the macro processing.
 * 
 * @details
 * - **#include**:
 *   - Expects the next non-space token to be the name of the file to include.
 *   - Validates that the file name is either a literal string or a name.
 *   - Checks for circular dependencies by ensuring the file has not already been included.
 *   - Tokenizes the included file and inserts its tokens into the current token list.
 *   - Removes the `#include` directive and the file name from the token list.
 * 
 * - **#define**:
 *   - Expects the next non-space token to be the name of the constant.
 *   - Expects the token after the constant name to be its value (name, int, float, or expression).
 *   - Validates that the constant name is not already defined or used as an unknown symbol.
 *   - Adds the constant to the list of defined constants.
 *   - Removes the `#define` directive, constant name, and value from the token list.
 * 
 * @note If an error occurs (e.g., missing tokens, invalid types, circular dependencies, or redefinitions),
 *       the function prints an error message and terminates the process.
 * @note If the value for the constant is an expression, it spaces can still be placed in the expression inside its outer parenthesis.
 * 
 * @throws std::bad_any_cast If a token's value cannot be cast to the expected type.
 */
DataFile::Tokens::iterator DataFile::apply_macro(Tokens& input_tokens, Tokens::iterator token, IOS_OPEN_MODE mode) {
	assert(token->type == DataFileToken::Type::Macro);

	std::string macro_name = std::any_cast<std::string>(token->value);

	if (macro_name == "include") {
		// the next non-space token is the name of the file to include
		auto token_file_name = std::next(token);
		while (token_file_name != input_tokens.end() && token_file_name->type == DataFileToken::Type::Space)
			token_file_name = std::next(token_file_name);

		if (token_file_name == input_tokens.end()) {
			token->print_as_error("End of file reached before the name of the file to include could be specified.");
			Process::exit(DONT_PRINT_TOKEN);
		}
		
		// me expect a literal string or a name
		if (token_file_name->type != DataFileToken::Type::Name && token_file_name->type != DataFileToken::Type::LiteralString) {
			std::ostringstream oss;
			oss << token_file_name->type;
			token_file_name->print_as_error("Expected a literal-string or a name as file-name, not a '" + oss.str() + "'.");
			Process::exit(DONT_PRINT_TOKEN);
		}

		// get the file name
		std::string include_file_name = std::any_cast<std::string>(token_file_name->value);

		// check if this file has not already been included
		if (std::find(included_files.begin(), included_files.end(), include_file_name) != included_files.end()) {
			token_file_name->print_as_error("This file as already been included (circular dependency).");
			Process::exit(DONT_PRINT_TOKEN);
		}

		// register this file has being included
		included_files.push_back(include_file_name);

		// tokenize this file
		std::list<DataFileToken> tokens_include = process(include_file_name, mode);

		// then insert this new tokens inside the current tokens list, just before the '#include' token
		input_tokens.splice(token, tokens_include);

		// then remove the '#include' and file name from the tokens list
		while (token != token_file_name)
			token = input_tokens.erase(token);
		token = input_tokens.erase(token); // delete the include file name 

		return token;

	} else if (macro_name == "define") {
		// the next token is the Name of the constant
		// and the token after is the value of the constant 
		auto token_constant_name = std::next(token);
		while (token_constant_name != input_tokens.end() && token_constant_name->type == DataFileToken::Type::Space)
			token_constant_name = std::next(token_constant_name);

		if (token_constant_name == input_tokens.end()) {
			token->print_as_error("End of file reached before the name of the constant could be specified.");
			Process::exit(DONT_PRINT_TOKEN);
		}
		
		// me expect a name
		if (token_constant_name->type != DataFileToken::Type::Name) {
			std::ostringstream oss;
			oss << token_constant_name->type;
			token_constant_name->print_as_error("Expected a name as constant name, not a '" + oss.str() + "'.");
			Process::exit(DONT_PRINT_TOKEN);
		}

		// then get the value
		auto token_constant_value = std::next(token_constant_name);
		while (token_constant_value != input_tokens.end() && token_constant_value->type == DataFileToken::Type::Space) {
			//token_constant_value = next(input_tokens, token_constant_value);
			token_constant_value = next(input_tokens, token_constant_value);
		}

		if (token_constant_value == input_tokens.end()) {
			token->print_as_error("End of file reached before the value of the constant could be specified.");
			Process::exit(DONT_PRINT_TOKEN);
		}

		// me expect a name, an int or a float, or an expression
		if (token_constant_value->type != DataFileToken::Type::Name &&
			token_constant_value->type != DataFileToken::Type::Int &&
			token_constant_value->type != DataFileToken::Type::Float &&
			token_constant_value->type != DataFileToken::Type::Expression) {

			std::ostringstream oss;
			oss << token_constant_value->type;
			token_constant_value->print_as_error("Expected a name, an int, a float  or an expression as constant value, not a '" + oss.str() + "'.");
			Process::exit(DONT_PRINT_TOKEN);
		}

		// check if this constant has already been definied
		std::string constant_name =
				std::any_cast<std::string>(token_constant_name->value);

		if (auto it = constants.find(constant_name); it != constants.end()) {
			// it may be a native constant (it is not associated with a file)
			if (it->second.second.file_name == "") {
				token_constant_name->print_as_error("Constant '" +
					TextFormatting::Err::format(constant_name, 0,1) +
				    "' is already definied as a native constant..");
			} else {
				token_constant_name->print_as_error("Constant '" +
					TextFormatting::Err::format(constant_name, 0,1) +
				    "' is already definied.");

				it->second.first.print_as_note("First declared here.");
			}

			Process::exit(DONT_PRINT_TOKEN);
		}

		// check if this constant name is already use as an unknown (symbol that can appear in an expression but don't have any value)
		if (auto it = unknowns.find(constant_name); it != unknowns.end()) {
			// it may be a native constant (it is not associated with a file)
			token_constant_name->print_as_error("The symbol '" +
					TextFormatting::Err::format(constant_name, 0,1) +
			        "' is already definied as an unknown for '" + it->second + "'.");

			Process::exit(DONT_PRINT_TOKEN);
		}

		// check if its the type
		if (Type_info::est_un_type(constant_name.c_str())) {
			token_constant_name->print_as_error("The symbol  '" +
					TextFormatting::Err::format(constant_name, 0,1) +
					"' is already definied as a type in TRUST.");
			Process::exit(DONT_PRINT_TOKEN);
		}

		// append this constant to the list of constants (constants
		constants.insert({constant_name,
				{*token_constant_name, *token_constant_value}});

		// then remove the '#define' and parameters 
		while (token != token_constant_value)
			token = input_tokens.erase(token);
		token = input_tokens.erase(token); // delete the include file name 

		return token;


	} else {
		token->print_as_error(
				"Unknown macro name '" +
				TextFormatting::Err::format(macro_name, 0,1) + "'.");
		Process::exit(DONT_PRINT_TOKEN);
		return token; // return token any way to avoid compilation error
	}
}

/**
 * @brief Validates the matching of curly brackets in the token list.
 *
 * This function checks whether all curly brackets in the `tokens` list are properly
 * opened and closed. It ensures that:
 * - Every closing curly bracket has a corresponding opening curly bracket.
 * - Every opening curly bracket has a corresponding closing curly bracket.
 *
 * If an unmatched closing curly bracket is found, an error message is printed
 * using `token.print_as_error`, and the program exits.
 *
 * If there are unmatched opening curly brackets, the function identifies the
 * last unmatched opening curly bracket, prints an error message, and exits
 * the program.
 *
 * @throws This function terminates the program using `Process::exit(DONT_PRINT_TOKEN)` if
 *         unmatched curly brackets are detected.
 */
void DataFile::check_curly_brackets() const {
	size_t depth = 0;
	for (auto& token: tokens) {
		if (token.type == DataFileToken::Type::OpenCurlyBracket) {
			depth++;
		} else if (token.type == DataFileToken::Type::CloseCurlyBracket) {
			if (depth == 0) {
				token.print_as_error("This curly bracket has never been opened.");
				Process::exit(DONT_PRINT_TOKEN);
			} else {
				depth--;
			}
		}
	}

	// if one curly brackets has not been closed
	if (depth != 0) {
		// search the last opened curly brackets at the same depth
		size_t depth_tmp = depth;
		for (auto token = tokens.rbegin(); token != tokens.rend(); token++) {
			if (token->type == DataFileToken::Type::OpenCurlyBracket)
				depth_tmp--;
			else if (token->type == DataFileToken::Type::CloseCurlyBracket)
				depth_tmp++;

			if (depth_tmp == depth - 1) {
				token->print_as_error("This curly brackets has never been closed.");
				Process::exit(DONT_PRINT_TOKEN);
			}
		}
	}
}

/**
 * @brief Checks for obsolete keywords in the data file.
 *
 * This method iterates through the tokens in the data file and identifies
 * keywords that may be considered obsolete. It performs the following checks:
 * - Ensures the token is a name.
 * - Verifies that the token is followed by either a space or an opening curly
 *   bracket, which indicates the end of the word. This accounts for cases
 *   where special characters like '-' or '%' are used in names.
 *
 * If a valid keyword is identified, it is passed to the `verifie` method for
 * further validation.
 *
 * @note This function assumes that the `tokens` container is properly populated
 *       and that each token has a valid type and string representation.
 */
void DataFile::check_obsolete_keywords() const {
	for (auto token = tokens.begin(); token != tokens.end(); token++) {
		// if this token is followed by a space or a curyl bracket, it means it the 'real'
		// end of this word (because some people use like '-' or '%' in their name
		if (token->is_name() &&
			token != tokens.end() &&
			(std::next(token)->is_space() ||
			 std::next(token)->type == DataFileToken::Type::OpenCurlyBracket)) {

			Nom word = token->string;
			verifie(word);
		}
	}
}

/**
 * @brief Checks for reserved keywords in the tokens and issues warnings if any
 *        token could be interpreted as a function name or an operator.
 * 
 * This method iterates through the list of tokens and examines each token to
 * determine if it is a name. If the token is identified as a name, it further
 * checks whether it could be interpreted as a function or an operator in an
 * expression. If so, a warning is printed to notify the user that the token
 * might need to be enclosed in parentheses to avoid ambiguity in expressions.
 * 
 * @note This function does not modify the tokens; it only performs checks and
 *       prints warnings if necessary.
 */
void DataFile::check_reserved_keywords() const {
	for (auto token = tokens.begin(); token != tokens.end(); token++) {
		// if a name could be a function name or an operator, print a warning
		if (token->is_name()) {
			if (token->is_function())
				token->print_as_warning("The keyword '" +
						TextFormatting::Err::format(token->string, 0,1) +
						"' could be use as a function in an expression. Are you sure this should not be an expression (surounded in parenthesis)?.");

			else if (token->is_operator())
				token->print_as_warning("The keyword '" +
						TextFormatting::Err::format(token->string, 0,1) +
						"' could be use as an operator in an expression. Are you sure this should not be an expression (surounded in parenthesis)?.");
		}
	}
}

/**
 * @brief Converts a list of DataFileToken objects into a formatted string representation.
 *
 * This function iterates through a list of tokens and constructs a string that represents
 * the tokens in a readable format. It handles special formatting for curly brackets, comments,
 * operators, and functions to ensure the output closely resembles the original data file's
 * structure and spacing.
 *
 * @param input_tokens A list of DataFileToken objects to be converted into a string.
 * @return A formatted string representation of the input tokens.
 *
 * @details
 * - Curly brackets and comments are followed by a newline character.
 * - Comments are excluded from the output string.
 * - Operators and functions are formatted to suppress unnecessary spaces around them
 *   when adjacent to mathematical expressions or names.
 * - Spaces are added after tokens unless they are operators or functions.
 */
std::string DataFile::to_string(const std::list<DataFileToken>& input_tokens) const {
	std::string output;
	for (auto token = input_tokens.begin(); token != input_tokens.end(); token++) {
		if (token->is_curly_bracket() || token->is_comment())
			output += '\n';

		if (!token->is_comment()) {
			// small hack to make expressions appearing outside of parenthesis in original
			// data files still look identical

			// spaces that would have been added around the expressions are suppressed
			// if there is operator around them
			if (token->is_operator() || token->is_function()) {
				if (token != input_tokens.begin() && (
						std::prev(token)->is_math() || std::prev(token)->is_name())) {
					while (output.length() > 0 && output.back() == ' ')
						// remove the last space
						output.pop_back();
				}

				output += token->to_string();
			} else {
				output += token->to_string();
				output += ' ';
			}
		}

		if (token->is_curly_bracket())
			output += '\n';

	}

	return output;
}

/**
 * @brief Writes a list of DataFileToken objects to a file after processing.
 * 
 * This function converts a list of tokens into a string representation and writes it to a file.
 * The file name is sanitized to ensure it is written in the current folder by replacing '/' with '_'.
 * This operation is performed only by the master process.
 * 
 * @param input_tokens A list of DataFileToken objects to be written to the file.
 * @param file_name The name of the file where the data will be written. Any '/' in the file name
 *                  will be replaced with '_' to ensure the file is created in the current directory.
 * 
 * @note This function is executed only by the master process (Process::je_suis_maitre()).
 *       If the file cannot be opened, the program will terminate using Process::exit(DONT_PRINT_TOKEN).
 */
void DataFile::write(const std::list<DataFileToken>& input_tokens, const std::string& file_name) const {
	
	if (Process::je_suis_maitre()) {
		// convertl the list of tokens to a string
		std::string output = to_string(input_tokens);
	
		// remove the '/' from the file name to always write the result in the current folder
		std::string corrected_file_name = file_name;
		std::replace(corrected_file_name.begin(), corrected_file_name.end(), '/', '_');

		Cout << "Dump the data file after pre-processor in file '" << corrected_file_name << "'." << finl;

		// open file
		SFichier file;
		if (!file.ouvrir(corrected_file_name.c_str(), ios::out)) {
			Cerr << "Failed to open file '" << corrected_file_name << "'." << finl;
			Process::exit(DONT_PRINT_TOKEN);
		}
	
		// write data
		file << output;
	
		// close file
		file.close();
	}
}

/**
 * @brief Retrieves the master input stream associated with the DataFile object.
 * 
 * This function provides access to the `data` member, which represents the 
 * main input stream for the DataFile. It allows reading data from the 
 * associated input source.
 * 
 * @return A reference to the `Entree` object representing the master input stream.
 */
Entree& DataFile::get_entree_master() {
  return data;
}
