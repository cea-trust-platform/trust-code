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
#ifndef DataFileExpression_included
#define DataFileExpression_included

#include <list>
#include <vector>
#include <map>
#include <cmath>

#include <DataFileToken.h> 

class DataFileExpression {

		/////////////////////	
		// class / structs //
		/////////////////////	

	private:
		
		struct Node: public DataFileToken {

			//////////////////
			// constructors //
			//////////////////

			// copy from a DataFileToken
			Node(const DataFileToken& token);
			
			// performe computation if possible
			Node(const DataFileToken& token, std::shared_ptr<Node> a, std::shared_ptr<Node> b);

			// performe computation if possible on unary operators
			Node(const DataFileToken& token, std::shared_ptr<Node> a);

			/////////////
			// methods //
			/////////////

			bool is_int() const;
			bool is_float() const;
			bool is_numeric() const;

			template <class Type>
			Type compute(const std::shared_ptr<Node> a, const std::shared_ptr<Node> b);

			template <class Type>
			Type compute(const std::shared_ptr<Node> a);

			template <class Type>
			Type apply_function(const std::string& function_name, const std::shared_ptr<Node> a);

			// cast the value to the given Type
			template <class Type>
			Type cast() const;

			// return string representation
			std::string to_string() const;

			////////////////
			// attributes //
			////////////////

			std::shared_ptr<Node> left = nullptr;
			std::shared_ptr<Node> right = nullptr;

			// boolean to register if the value store in the node is the one from the
			// original token or not
			bool is_original = true;
		};

		
		/////////////
		// aliases //
		/////////////
	
	using Tokens = std::list<DataFileToken>;
		
		//////////////////
		// constructors //
		//////////////////

	public:
		
		DataFileExpression(
				const Tokens::iterator& start,
				const Tokens::iterator& end);

		/////////////
		// methods //
		/////////////

	public:
		
		void evaluate();
		bool is_numeric();
		DataFileToken value() const;
		std::string to_string() const;

	private:
		
		static void check_parenthesis(const Tokens& tokens);

		static std::shared_ptr<Node> treefy(
				const Tokens::iterator& start,
				const Tokens::iterator& end);

		static std::vector<DataFileToken> Shuting_yard(
				const DataFileExpression::Tokens::iterator& start,
				const DataFileExpression::Tokens::iterator& end);

		////////////////
		// attributes //
		////////////////

	public:
		
		std::list<DataFileToken> tokens;

	private:

		std::shared_ptr<Node> root = nullptr;
};

#endif
