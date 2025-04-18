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

#include <stdlib.h> // to use drand48

#include <DataFileExpression.h>
#include <EntreeSortie.h>

//////////////////////////////////////
// DataFileExpression::Node methods //
//////////////////////////////////////

// create a Node by copying a DataFileToken
DataFileExpression::Node::Node(const DataFileToken& token): DataFileToken(token) {}

/**
 * @brief Constructs a Node object representing an operation or a value in a data file expression.
 * 
 * @param token The token representing the operation or value.
 * @param a Pointer to the left operand node (can be nullptr for unary operations).
 * @param b Pointer to the right operand node (can be nullptr for unary operations).
 * 
 * This constructor handles the following cases:
 * - If the operation is a composition (e.g., "c") and the right operand is numeric:
 *   - Applies specific functions such as `int()`, `float()`, or `rand()`.
 *   - Computes the result and sets the node's value and type accordingly.
 * - If at least one of the operands is not numeric:
 *   - Creates a node representing the operation without computing the result.
 * - If both operands are numeric:
 *   - Computes the result based on the operation.
 *   - Preserves the result as an integer if both operands are integers (except for division).
 *   - Promotes the result to a float if one or both operands are floats, or if the operation is division.
 * 
 * @note The constructor ensures deterministic behavior for pseudo-random operations (`rand`) 
 *       to validate test cases consistently.
 * 
 * @throws std::bad_any_cast If the value of the left operand cannot be cast to a string 
 *         when applying a function.
 * @throws std::invalid_argument If an unsupported function name is encountered during composition.
 */
DataFileExpression::Node::Node(const DataFileToken& token, std::shared_ptr<Node> a, std::shared_ptr<Node> b):
		Node(token) {
	assert(is_operator());

	// if the operation is a composition
	if (string == "c" && b->is_numeric()) {
		// compute function as float
		// but there a few special cases: the int(), float()
		std::string function_name = TextFormatting::lower(
				std::any_cast<std::string>(a->value));

		// apply all functions except the rand (rnd) so that the same
		// deterministic pseudo-random generated validate the test-cases
		if        (function_name == "int") {
			value = b->cast<int>();
			type = DataFileToken::Type::Int;
			is_original = false;
		} else if (function_name == "float") {
			value = b->cast<double>();
			type = DataFileToken::Type::Float;
			is_original = false;
		} else if (function_name == "rand") {
			// get the string representation of the double value
			std::string string_value = DataFileToken::to_string(b->cast<double>());

			// compose the expression
			value = "rnd(" + string_value + ")";
			type = DataFileToken::Type::Name;
			is_original = false;
		} else {
			value = apply_function<double>(function_name, b);
			type = DataFileToken::Type::Float;
			is_original = false;
		}

	// if at least one of the two operand a or b is not an int nor a float,
	// then we don't compute anything and create a node for that operation
	} else if (!a->is_numeric() || !b->is_numeric()) {
		left = a;
		right = b;
	} else {
		// if both operands are int, preserve the result as int
		// otherwise, promote the int to float
		// an exception with division that always return a float
		if (a->is_int() && b->is_int() && token.string != "/") {
			//Cout << "Compute as ints" << finl;
			value = compute<int>(a, b);
			type = DataFileToken::Type::Int;
			is_original = false;
		} else {
			//Cout << "Compute as floats" << finl;
			value = compute<double>(a, b);
			type = DataFileToken::Type::Float;
			is_original = false;
		}
	}
}

/**
 * @brief Constructs a Node object with a given token and a single operand.
 *
 * This constructor initializes a Node object representing an operation
 * or value in an expression tree. It ensures that the node is an operator
 * and determines how to handle the operand based on its numeric type.
 *
 * @param token The token representing the operator or value for this node.
 * @param a A pointer to the operand node.
 *
 * @note If the operand `a` is not numeric, the node is created with `a` as
 *       the right child and no left child. If the operand `a` is numeric,
 *       the value of the node is computed based on the operand's type:
 *       - If `a` is an integer, the result is computed as an integer.
 *       - If `a` is a float, the result is computed as a double.
 *       - Division operations always promote the result to an integer.
 *
 * @throws std::logic_error If the node is not an operator.
 */
DataFileExpression::Node::Node(const DataFileToken& token, std::shared_ptr<Node> a):
		Node(token) {

	assert(is_operator());

	// if at least one of the two operand a or b is not an int nor a float,
	// then we don't compute anything and create a node for that operation
	if (!a->is_numeric()) {
		left = nullptr;
		right = a;
	} else {
		// if both operands are float, preserve the result as int
		// otherwise, promote the int to float
		// always promote to int on division
		if (a->is_int()) {
			value = compute<int>(a);
			type = DataFileToken::Type::Int;
			is_original = false;
		} else {
			value = compute<double>(a);
			type = DataFileToken::Type::Float;
			is_original = false;

		}
	}
}



/**
 * @brief Converts the current node of the expression tree into a string representation.
 *
 * This function recursively traverses the expression tree and generates a string
 * representation of the node and its children. The output format depends on the type
 * of the node (e.g., operator, operand, or function) and its associated value.
 *
 * @return A string representation of the node and its subtree.
 *
 * The behavior is as follows:
 * - If the node is a leaf (no children):
 *   - If the node is marked as original (`is_original`), the original string is returned.
 *   - Otherwise, the value is reinterpreted based on its type and converted to a string.
 * - If the node is an operator with both left and right children:
 *   - For function operators (e.g., "c"), the function name is processed and its arguments
 *     are enclosed in parentheses.
 *   - For binary operators, the left and right subtrees are recursively converted to strings,
 *     and the operator is inserted between them. Special cases for operators like "<=", ">=", 
 *     "min", "max", "mod", and "&&" are handled with custom formatting.
 * - If the node is a unary operator with only a right child:
 *   - The operator and the right subtree are combined into a string with appropriate formatting.
 * - If the node does not match any of the above cases, an error message is printed, and the
 *   program exits.
 *
 * @note This function assumes that the node's type and value are valid and consistent.
 *       If an invalid state is encountered, the program will terminate with an error message.
 */
std::string DataFileExpression::Node::to_string() const {
	// output recursively
	if (left == nullptr && right == nullptr) {
		// if the token is original (is_original flag is set; the value has not
		// been change by evuating an expression or something else), simply return
		// the original string
		if (is_original)
			return string;

		// otherwise, reinterpret the value with the correct type
		switch (type) {
			case DataFileToken::Type::Int:
				return std::to_string(std::any_cast<int>(value));
			case DataFileToken::Type::Float:
				return DataFileToken::to_string(std::any_cast<double>(value));
			default:
				return std::any_cast<std::string>(value);
		}
	} else if (left != nullptr && right != nullptr && type == DataFileToken::Type::Operator) {
		std::string output;
		if (std::any_cast<std::string>(value) == "c") { // its a function
			std::string function_name = left->to_string();
			if (function_name == "rand") {
				output = "rnd";
			} else if (function_name == "sign") {
				output = "sgn";
			} else {
				output = function_name;
			}

			output += "(" + right->to_string() + ")"; 
			return  output;
		} else {
			output = "(" + left->to_string() + ")";
			if (std::any_cast<std::string>(value) == "<=")
				output += "[";
			else if (std::any_cast<std::string>(value) == ">=")
				output += "]";
			else if (std::any_cast<std::string>(value) == "min")
				output += "_min_";
			else if (std::any_cast<std::string>(value) == "max")
				output += "_max_";
			else if (std::any_cast<std::string>(value) == "mod")
				output += "_mod_";
			else if (std::any_cast<std::string>(value) == "&&")
				output += "_and_";
			else
				output += std::any_cast<std::string>(value);

			output += "(" + right->to_string() + ")"; 
			return  "(" + output + ")";

		}

	} else if (left == nullptr && right != nullptr && type == DataFileToken::Type::OperatorUnary) {
		return "(" + std::any_cast<std::string>(value) + "(" + right->to_string() + "))";
	} else {
		Cerr << "Fail to print" << finl;
		Cerr << "type is " << type << finl;
		Process::exit(DONT_PRINT_TOKEN);
		return "";
	}
}


/**
 * @brief Checks if the current node represents an integer type.
 * 
 * This method determines whether the type of the current node
 * is of type `DataFileToken::Type::Int`.
 * 
 * @return true if the node type is `DataFileToken::Type::Int`, false otherwise.
 */
bool DataFileExpression::Node::is_int() const {
	return type == DataFileToken::Type::Int;
}

/**
 * @brief Checks if the current node represents a floating-point value.
 * 
 * @return true if the node's type is a floating-point value, false otherwise.
 */
bool DataFileExpression::Node::is_float() const {
	return type == DataFileToken::Type::Float;
}

/**
 * @brief Checks if the current node represents a numeric value.
 * 
 * This method determines whether the node is numeric by verifying
 * if it is either an integer or a floating-point number.
 * 
 * @return true if the node is numeric (integer or float), false otherwise.
 */
bool DataFileExpression::Node::is_numeric() const {
	return is_int() || is_float();
}

/**
 * @brief Computes the result of an operation between two nodes based on the operator stored in the current node.
 * 
 * @tparam Type The data type of the values being computed (e.g., int, double).
 * @param a Pointer to the left operand node.
 * @param b Pointer to the right operand node.
 * @return Type The result of the computation.
 * 
 * @note The function supports the following operators:
 *       - Arithmetic: "+", "-", "*", "/", "^" (power)
 *       - Comparison: "<", ">", "<=", ">="
 *       - Functions: "min", "max", "mod" (modulus)
 * 
 * @warning Division by zero will terminate the program with an error message.
 * @warning The operation 0^0 is considered undefined and will terminate the program with an error message.
 * @warning If an unsupported operator is encountered, the program will terminate with an error message.
 * 
 * @throws Process::exit(DONT_PRINT_TOKEN) If the right operand of a division is zero or if the operation is 0^0.
 * @throws Process::exit(DONT_PRINT_TOKEN) If an unsupported operator is used.
 */
template <class Type>
Type DataFileExpression::Node::compute(const std::shared_ptr<Node> a, const std::shared_ptr<Node> b) {
	assert(is_operator());


	Type a_value = a->cast<Type>();
	Type b_value = b->cast<Type>();

	Type output = 0;

	if        (string == "+") {
		output = a_value + b_value;
	} else if (string == "-") {
		output = a_value - b_value;
	} else if (string == "*") {
		output = a_value * b_value;
	} else if (string == "/") {
		if (b_value == 0) {
			print_as_error("The right side of this division evaluates to 0, leading to undefinied result.");
			Process::exit(DONT_PRINT_TOKEN);
		} else {
			output = a_value / b_value;
		}
	} else if (string == "^") {
		if (a_value == 0 && b_value == 0) {
			print_as_error("This operation evaluates to 0^0, which is undefined.");
			Process::exit(DONT_PRINT_TOKEN);
		}
		output = static_cast<Type>(std::pow(static_cast<double>(a_value), static_cast<double>(b_value)));
	} else if (string == "<") {
		return static_cast<Type>(a_value < b_value);
	} else if (string == ">") {
		return static_cast<Type>(a_value > b_value);
	} else if (string == "<=") {
		return static_cast<Type>(a_value <= b_value);
	} else if (string == ">=") {
		return static_cast<Type>(a_value >= b_value);
	} else if (string == "min") {
		return std::min(a_value, b_value);
	} else if (string == "max") {
		return std::min(a_value, b_value);
	} else if (string == "mod") {
		return static_cast<Type>(std::fmod(a_value, b_value));
	} else {
		print_as_error("Operator not supported for expression evaluation.");
		Process::exit(DONT_PRINT_TOKEN);
	}

	return output;
}

/**
 * @brief Computes the result of applying a unary operator to a given node.
 * 
 * @tparam Type The type of the value to be computed.
 * @param a Pointer to the node whose value will be used in the computation.
 * @return Type The result of applying the unary operator to the value of the node.
 * 
 * @note This function assumes that the current node represents an operator.
 *       It asserts that the node is an operator before proceeding.
 * 
 * @details The function supports the following unary operators:
 *          - "+" (positive sign, returns the value as is)
 *          - "-" (negative sign, returns the negated value)
 * 
 * @throws This function terminates the program if an unsupported unary operator
 *         is encountered, printing an error message before exiting.
 * 
 * @warning Ensure that the node represents a valid unary operator before calling
 *          this function. Unsupported operators will cause the program to exit.
 */
template <class Type>
Type DataFileExpression::Node::compute(const std::shared_ptr<Node> a) {
	assert(is_operator());

	Type a_value = a->cast<Type>();

	//Cout << "Compute " << token.string << a_value << finl;

	if        (string == "+") {
		return +a_value;
	} else if (string == "-") {
		return -a_value;
	} else {
		print_as_error("Unary operator not supported for expression evaluation.");
		Process::exit(DONT_PRINT_TOKEN);
		return +a_value; // return to avoid compilation error
	}
}

/**
 * @brief Applies a mathematical function to the given node's value.
 * 
 * This template function evaluates a mathematical function specified by its name
 * on the value of the provided node. The node's value must be either an integer
 * or a floating-point number. The function handles various mathematical operations
 * and performs domain checks where applicable.
 * 
 * @tparam Type The type of the value to be processed (e.g., float, double).
 * @param function_name The name of the mathematical function to apply. Supported
 *                      functions include:
 *                      - Trigonometric: "sin", "cos", "tan", "asin", "acos", "atan"
 *                      - Hyperbolic: "sinh", "cosh", "tanh"
 *                      - Exponential and logarithmic: "exp", "ln", "log10"
 *                      - Miscellaneous: "sqrt", "rand", "abs", "sign"
 * @param a A pointer to the node containing the value to process.
 * @return The result of applying the specified function to the node's value.
 * 
 * @throws std::runtime_error If the function name is not recognized or if the
 *                            input value is outside the domain of the specified
 *                            function.
 * 
 * @note The function performs domain checks for operations like "tan", "asin",
 *       "acos", "ln", "log10", and "sqrt". If the input value is outside the
 *       valid domain, an error message is printed, and the program exits.
 * 
 * @warning The "tan" function is undefined at odd multiples of π/2. The "ln" and
 *          "log10" functions require positive input values. The "sqrt" function
 *          requires non-negative input values.
 */
template <class Type>
Type DataFileExpression::Node::apply_function(const std::string& function_name, const std::shared_ptr<Node> a) {
	assert(a->is_int() || a->is_float());
	Type a_value = a->cast<Type>();

	Type output = 0;

	if        (function_name == "sin") {
		output = sin(a_value);
	} else if (function_name == "cos") {
		output = cos(a_value);
	} else if (function_name == "tan") {
		if (fmod(fabs(a_value - M_PI_2), M_PI) < 1e-9) {
			print_as_error("Domain error: tan() is undefined at odd multiples of π/2, not " + std::to_string(a_value) + ".");
			Process::exit(DONT_PRINT_TOKEN);
		}
		output = tan(a_value);
	} else if (function_name == "asin") {
		if (a_value < -1 || a_value > 1) {
			print_as_error("Domain error: asin() input must be in [-1, 1], not " + std::to_string(a_value) + ".");
			Process::exit(DONT_PRINT_TOKEN);
		}
		output = asin(a_value);
	} else if (function_name == "acos") {
		if (a_value < -1 || a_value > 1) {
			print_as_error("Domain error: acos() input must be in [-1, 1], not " + std::to_string(a_value) + ".");
			Process::exit(DONT_PRINT_TOKEN);
		}
		output = acos(a_value);
	} else if (function_name == "atan") {
		output = atan(a_value);
	} else if (function_name == "sinh") {
		output = sinh(a_value);
	} else if (function_name == "cosh") {
		output = cosh(a_value);
	} else if (function_name == "tanh") {
		output = tanh(a_value);
	} else if (function_name == "exp") {
		output = exp(a_value);
	} else if (function_name == "ln") {
		if (a_value <= 0) {
			print_as_error("Domain error: ln() input must be greater than 0, not " + std::to_string(a_value) + ".");
			Process::exit(DONT_PRINT_TOKEN);
		}
		output = log(a_value);
	} else if (function_name == "log10") {
		if (a_value <= 0) {
			print_as_error("Domain error: log10() input must be greater than 0, not " + std::to_string(a_value) + ".");
			Process::exit(DONT_PRINT_TOKEN);
		}

		output = log10(a_value);
	} else if (function_name == "sqrt") {
		if (a_value < 0) {
			print_as_error("Domain error: sqrt() input must be greater or equal to 0, not " + std::to_string(a_value) + ".");
			Process::exit(DONT_PRINT_TOKEN);
		}

		output = sqrt(a_value);
	} else if (function_name == "rand") {
		output = a_value * drand48();
	} else if (function_name == "abs") {
		output = std::abs(a_value);
	} else if (function_name == "sign") {
		output = static_cast<Type>(a_value > 0) - static_cast<Type>(a_value < 0);
	} else {
		print_as_error("Could not compute the value for this function.");
		Process::exit(DONT_PRINT_TOKEN);
	}

	return static_cast<Type>(output);
}

////////////////////////////////
// DataFileExpression methods //
////////////////////////////////

/**
 * @brief Casts the stored value in the node to the specified type.
 * 
 * This function attempts to cast the value stored in the node to the 
 * template type `Type`. The function checks the type of the stored 
 * value and performs the appropriate cast. If the type of the stored 
 * value is not supported, the function logs an error message and 
 * terminates the program.
 * 
 * @tparam Type The type to which the stored value should be cast.
 * @return The casted value of type `Type`.
 * 
 * @throws std::bad_any_cast If the stored value cannot be cast to the 
 *         expected type (e.g., if the type does not match the stored value).
 * 
 * @note The function will terminate the program if the stored value 
 *       is of an invalid type.
 * 
 * @warning Ensure that the stored value is compatible with the 
 *          requested cast type to avoid unexpected behavior.
 */
template <class Type>
Type DataFileExpression::Node::cast() const {
	if        (is_int()) {
		return static_cast<Type>(std::any_cast<int>(value));
	} else if (is_float()) {
		return static_cast<Type>(std::any_cast<double>(value));
	} else {
		Cerr << "Cannot cast value during evaluation of expression, invalid type." << finl;
		Process::exit(DONT_PRINT_TOKEN);
		return static_cast<Type>(0); // dummy return to avoid compilation error
	}
}

/**
 * @brief Constructs a DataFileExpression object by parsing a range of tokens.
 * 
 * This constructor processes a range of tokens representing a mathematical or logical
 * expression. It validates the tokens, removes spaces, checks for balanced parentheses,
 * and prepares the expression for evaluation by constructing a tree representation.
 * 
 * @param start An iterator pointing to the beginning of the token range.
 * @param end An iterator pointing to the end of the token range.
 * 
 * @details
 * - Tokens are validated to ensure they are of valid types (space, operator, operand, 
 *   parenthesis, or name). Invalid tokens result in an error message and program termination.
 * - Spaces are ignored, and only meaningful tokens are stored.
 * - Parentheses are checked for balance.
 * - If the expression starts with a '+' or '-' operator, a '0' is prepended to ensure
 *   compatibility with the Shunting Yard algorithm. Any other starting operator results
 *   in an error message and program termination.
 * - A tree representation of the expression is constructed for further evaluation.
 * 
 * @throws Terminates the program if invalid tokens are encountered, parentheses are unbalanced,
 *         or the expression starts with an invalid operator.
 */
DataFileExpression::DataFileExpression(
		const DataFileExpression::Tokens::iterator& start,
		const DataFileExpression::Tokens::iterator& end) {
	
	// copy the tokens of the expression without the spaces
	auto token = start;
	do {
		// check the types
		if (!token->is_space() && !token->is_operator() && !token->is_operand() && !token->is_parenthesis() && !token->is_name()) {
			token->print_as_error("Invalid token inside an expression.");
			Process::exit(DONT_PRINT_TOKEN);
		}
		
		if (!token->is_space())
			tokens.push_back(*token);
	} while (token++ != end);

	// check parenthesis
	check_parenthesis(tokens);

	// if the expression start with an operator + or -, add a 0 in front
	// to make the Shuting Yard algorithm
	// if we have any other operators it is an error
	if (tokens.front().is_operator()) {
		if (tokens.front().string == "+" || tokens.front().string == "-") {
			tokens.push_front(DataFileToken("", 0, 0, 0, 0, DataFileToken::Type::Int, "0"));
		} else {
			tokens.front().print_as_error("Expression cannot start with an operator '" + tokens.front().string + "'.");
			Process::exit(DONT_PRINT_TOKEN);
		}
	}

	// construct the tree of the expression
	root = treefy(tokens.begin(), std::prev(tokens.end()));
}

/**
 * @brief Retrieves the value of the DataFileExpression as a DataFileToken.
 *
 * This method constructs and returns a DataFileToken object representing
 * the value of the DataFileExpression. The token is initialized using
 * the file name, start line, start column, end line, and end column
 * from the first and last tokens in the `tokens` list. The type and
 * value of the token are then set based on the `root` node of the
 * expression.
 *
 * @return A DataFileToken object representing the value of the expression.
 */
DataFileToken DataFileExpression::value() const {

	DataFileToken token(
		tokens.front().file_name,
		tokens.front().start_line,
		tokens.front().start_column,
		tokens.back().end_line,
		tokens.back().end_column,
		DataFileToken::Type::Name,
		"");
	token.type= root->type;
	token.value = root->value;

	return token;
}

/**
 * @brief Determines if the expression represented by the DataFileExpression object is numeric.
 * 
 * This method checks whether the root node of the expression tree is numeric.
 * A numeric expression typically consists of constant values or expressions
 * that can be evaluated to a numeric result without any symbolic variables.
 * 
 * @return true if the expression is numeric, false otherwise.
 */
bool DataFileExpression::is_numeric() {
	return root->is_numeric();
}

/**
 * @brief Validates the proper usage of parentheses in a sequence of tokens.
 *
 * This function performs the following checks:
 * - Ensures that all opened parentheses are properly closed.
 * - Verifies that an open parenthesis is preceded by another open parenthesis 
 *   or an operator, and is followed by an operand, a unary operator, or another open parenthesis.
 * - Ensures that a closing parenthesis is preceded by another closing parenthesis 
 *   or an operand, and is followed by an operator or another open parenthesis.
 * - Detects unmatched parentheses and provides detailed error messages indicating the issue.
 *
 * If any of the above conditions are violated, the function prints an error message 
 * and terminates the program.
 *
 * @param tokens A collection of tokens representing the expression to be validated.
 */
void DataFileExpression::check_parenthesis(const DataFileExpression::Tokens& tokens) {
	// check that all opened parenthesis are properly closed
	// also check if if open parenthesis are precede by another
	// open parenthesis or an operator, and are followed by an operand or and unary operator or and another open parenthesis
	// and check if closing parenthesis are precede by another closing parenthesis 
	// or and operand, and followed by an operator or another open parenthesis
	size_t depth = 0;
	for (auto token = tokens.begin(); token != tokens.end(); token++) {
		if (token->type == DataFileToken::Type::OpenParenthesis) {
			depth++;
			if (token != tokens.begin() && std::prev(token)->is_operand()) {
				token->print_as_error("An operand cannot be followed by an open parenthesis.");
				std::prev(token)->print_as_note("The operand.");
				Process::exit(DONT_PRINT_TOKEN);
			}
		} else if (token->type == DataFileToken::Type::CloseParenthesis) {
			if (depth == 0) {
				token->print_as_error("This parenthesis has never been opened.");
				Process::exit(DONT_PRINT_TOKEN);
			} else {
				depth--;
			}

			if (token != tokens.begin() && std::prev(token)->is_operator()) {
				token->print_as_error("An operator cannot be followed by a close parenthesis.");
				std::prev(token)->print_as_note("The operator.");
				Process::exit(DONT_PRINT_TOKEN);
			}
		}
	}

	// if one parentesis has not been closed
	if (depth != 0) {
		// search the last open parenthesis at the same depth
		size_t depth_tmp = depth;
    	for (auto token = tokens.rbegin(); token!= tokens.rend(); token++) {
			if (token->type == DataFileToken::Type::OpenParenthesis)
				depth_tmp--;
			else if (token->type == DataFileToken::Type::CloseParenthesis)
				depth_tmp++;

			if (depth_tmp == depth - 1) {
				token->print_as_error("This parenthesis has never been closed.");
				Process::exit(DONT_PRINT_TOKEN);
			}
		}

		Cerr << "Not matching parenthesis. Failed to identify where." << finl;
		Process::exit(DONT_PRINT_TOKEN);
	}
}

/**
 * @brief Converts a sequence of tokens into an expression tree.
 *
 * This function takes a range of tokens and constructs an expression tree
 * using the Shunting Yard algorithm to first convert the tokens into postfix
 * notation (Reverse Polish Notation). It then processes the postfix tokens
 * to build a tree structure representing the expression.
 *
 * @param start Iterator pointing to the beginning of the token sequence.
 * @param end Iterator pointing to the end of the token sequence.
 * @return A pointer to the root node of the constructed expression tree.
 *
 * @details
 * The function performs the following steps:
 * 1. Converts the input token sequence into postfix notation using the
 *    Shunting Yard algorithm.
 * 2. Processes the postfix tokens to construct the expression tree:
 *    - If a token is an operand, it is wrapped in a Node and pushed onto a stack.
 *    - If a token is an operator:
 *      - For unary operators, one operand is popped from the stack, and a new
 *        Node is created with the operator and operand.
 *      - For binary operators, two operands are popped from the stack, and a
 *        new Node is created with the operator and the two operands.
 * 3. Ensures that only one node remains in the stack, which becomes the root
 *    of the expression tree.
 *
 * @note The function assumes that the input tokens are valid and properly
 *       formatted. If an invalid token type is encountered, the program
 *       terminates with an error message.
 *
 * @throws std::assertion_failure If the stack does not contain exactly one
 *         node at the end of processing.
 */
std::shared_ptr<DataFileExpression::Node> DataFileExpression::treefy(
		const DataFileExpression::Tokens::iterator& start,
		const DataFileExpression::Tokens::iterator& end) {
	
	// first convert the token string to postfix notation / reverse Polish notation
	// using the Shuting yard algorithm
	std::vector<DataFileToken> tokens_postfix = Shuting_yard(start, end);

	// convert to tree
	// since we have simple reversed Polish notation, we read each token:
	//   if an operand is read, wrap it in a Node and put it in a stack.
	//   if it is an operator, we pop the last two operand of the stack, perform
	//     the operation (may result in Int, Float, or an expression if there is unknown
	//     terms, and put it back on the stack.
	//     Additional support is added for unary operator
	// at the end, we only have one node.
	std::vector<std::shared_ptr<Node>> stack;

	for (auto& token: tokens_postfix) {
		if (token.is_operand()) {
			stack.push_back(std::make_shared<Node>(token));
		} else if (token.is_operator()) {
			if (token.type == DataFileToken::OperatorUnary) {
				// pop one value
				std::shared_ptr<Node> a = stack.back();
				stack.pop_back();
				
				// then compute the unary operator
				stack.push_back(std::make_shared<Node>(token, a)); // perform computation
			} else {
				// dual operator
				assert(stack.size() >= 2);
				std::shared_ptr<Node> b = stack.back();
				stack.pop_back();
				std::shared_ptr<Node> a = stack.back();
				stack.pop_back();
				stack.push_back(std::make_shared<Node>(token, a, b)); // perform computation
			}
		} else {
			Cerr << "Invalid type " << token.type << " in expression tree." << finl;
			Process::exit(DONT_PRINT_TOKEN);
		}
	}

	assert(stack.size() == 1); // check if we only have one node as the root of the tree

	// return the root of the tree
	return stack.front();
}

/**
 * @brief Converts a sequence of tokens into Reverse Polish Notation (RPN) using the Shunting-yard algorithm.
 * 
 * This function processes a range of tokens and rearranges them into a format suitable for evaluation
 * by converting infix expressions to postfix (RPN). It handles operators, operands, parentheses, and
 * operator precedence and associativity.
 * 
 * @param start An iterator pointing to the beginning of the token sequence.
 * @param end An iterator pointing to the end of the token sequence.
 * @return A vector of DataFileToken objects representing the expression in RPN.
 * 
 * @details
 * - Operands are directly added to the output.
 * - Operators are pushed onto a stack, respecting precedence and associativity rules.
 * - Parentheses are used to group expressions and are handled accordingly:
 *   - Open parentheses are pushed onto the stack.
 *   - Close parentheses cause the stack to be unwound until the matching open parenthesis is found.
 * - Unary operators are identified and promoted when necessary.
 * - Any remaining operators in the stack are added to the output at the end.
 * 
 * @note The function assumes that the input tokens are well-formed and does not handle malformed expressions.
 *       If mismatched parentheses are encountered, an error message is printed.
 */
std::vector<DataFileToken> DataFileExpression::Shuting_yard(
		const DataFileExpression::Tokens::iterator& start,
		const DataFileExpression::Tokens::iterator& end) {
		
	// implementation from
	//     https://en.wikipedia.org/wiki/Shunting_yard_algorithm
	// with inspiration from
	//     https://inspirnathan.com/posts/155-handling-unary-operations-with-shunting-yard-algorithm
	// to make it work with unary operators
	auto token = start;
	std::vector<DataFileToken> out;
	std::vector<DataFileToken> stack;

	// initialize previous with a dummy token
	DataFileToken previous("", 0, 0, 0, 0, DataFileToken::Type::Space, "");

	do {
		if      (token->is_operand()) {
			out.push_back(*token);
		} else if (token->is_operator()) {
			if (previous.is_operator() || previous.is_space() || previous.type == DataFileToken::Type::OpenParenthesis) {
				
				// promote this operator to unary
				token->type = DataFileToken::Type::OperatorUnary;
			}
			while (stack.size() > 0 &&
			       stack.back().is_operator() &&
				   (stack.back().precedence() > token->precedence() || (
				    stack.back().precedence() == token->precedence() && token->associativity() == 'l')
				   )) {
				
				out.push_back(stack.back());
				stack.pop_back();
			}

			stack.push_back(*token);
		// skip the ',' case
		} else if (token->type == DataFileToken::Type::OpenParenthesis) {
			stack.push_back(*token);
		} else if (token->type == DataFileToken::Type::CloseParenthesis) {
			while (stack.size() > 0 &&
			       stack.back().type != DataFileToken::Type::OpenParenthesis) {
				
				out.push_back(stack.back());
				stack.pop_back();
			}
			if (stack.back().type != DataFileToken::Type::OpenParenthesis) {
				stack.back().print_as_error("We expected an open parenthesis.");
			}
			stack.pop_back(); // remove the parenthesis
		}

		previous = *token;
			
	} while (token++ != end);

	while (stack.size() > 0) {
		out.push_back(stack.back());
		stack.pop_back();
	}

	return out;
}	

/**
 * @brief Converts the DataFileExpression object to its string representation.
 *
 * This method generates a string representation of the DataFileExpression
 * by invoking the `to_string` method on the root node of the expression tree.
 *
 * @return A string that represents the root of the expression tree.
 */
std::string DataFileExpression::to_string() const {
	// print the root of the tree
	return root->to_string();
}
