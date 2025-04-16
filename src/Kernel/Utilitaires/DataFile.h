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

#ifndef DataFile_included
#define DataFile_included

#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <Lec_Diffuse_base.h>
#include <Entree.h>
#include <EChaine.h>
#include <DataFileToken.h>
#include <DataFileExpression.h>
#include <DataFileStream.h>

class Objet_U;

class DataFile: public Lec_Diffuse_base {
		
		///////////
		// TRUST //
		///////////

	private:

		Declare_instanciable_sans_constructeur(DataFile);

		/////////////
		// aliases //
		/////////////
	
	private:
		
		using Token = DataFileToken;
		using Tokens = std::list<Token>;
		
		//////////////////
		// constructors //
		//////////////////

	public:
		
		DataFile();
		DataFile(const char* root_data_file_name, IOS_OPEN_MODE mode=ios::in, bool do_check_obsolete_keywords_ = true);
		
		/////////////
		// methods //
		/////////////

	public:
		
		int ouvrir(const char* name, IOS_OPEN_MODE mode=ios::in ) override;
		Entree& get_entree_master() override;
	
	private:
		
		Tokens tokenize(const std::string& file_name, IOS_OPEN_MODE mode=ios::in) const;
		Tokens process(const std::string& file_name, IOS_OPEN_MODE mode=ios::in);
		Tokens::iterator apply_macro(Tokens& tokens, Tokens::iterator token, IOS_OPEN_MODE mode=ios::in);
		void replace(Tokens::iterator& token) const;
		Tokens::iterator next(Tokens& tokens, Tokens::iterator token) const;
		void check_curly_brackets() const;
		void check_obsolete_keywords() const;
		void check_reserved_keywords() const;

		std::string to_string(const Tokens& tokens) const;
		void write(const Tokens& tokens, const std::string& file_name) const;
	
		////////////////
		// attributes //
		////////////////

	public:

		Tokens tokens;
		std::vector<std::string> included_files;
	
	private:

		// stringstream-like object to store the parsed data
		// with their tokens
		DataFileStream data;
		
		// map the name of a constant to a
		// pair of {the token of the name, the token of the value}
		// initialize the constants with some predefined values (like pi, e…)
		std::map<std::string, std::pair<DataFileToken, DataFileToken>> constants = {
			{"PI", {
				DataFileToken("default", 0, 0, 0, 0, DataFileToken::Type::Name, "PI"),
				DataFileToken("default", 0, 0, 0, 0, DataFileToken::Type::Float, "3.14159265358979323846264338327950288419716939937511")}},
			{"E", {
				DataFileToken("default", 0, 0, 0, 0, DataFileToken::Type::Name, "E"),
				DataFileToken("default", 0, 0, 0, 0, DataFileToken::Type::Float, "2.71828182845904523536028747135266249775724709369996")}}
		};

		// map of unknowns with the name and their description
		// unknowns can appear in expression without having any values
		std::map<std::string, std::string> unknowns = {
			{"x", "X-component"},
			{"X", "X-component"},
			{"y", "Y-component"},
			{"Y", "Y-component"},
			{"z", "Z-component"},
			{"Z", "Z-component"},
			{"t", "Time component"},
			{"T", "Time component"},

		};
	
		// flag for checking or not obsolete keywords
		bool do_check_obsolete_keywords;
};

#endif
