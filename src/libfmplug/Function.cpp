/*
libfmplug, a C++ library to create FileMaker Pro (TM) plugins.
Copyright (C) 2014 University of Lausanne, Switzerland
Author: Thibault Genessay
https://github.com/tibogens/libfmplug

This file is part of libfmplug.

libfmplug is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libfmplug is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libfmplug.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "pch.h"
#include <fmplug/Setup.h>
#include <fmplug/Function.h>
#include <fmplug/UtfConv.h>
#include <fmplug/Utils.h>
#include <fmplug/Error.h>
#include <sstream>
#include <vector>
#include <FMWrapper/FMXTypes.h>
#include <FMWrapper/FMXCalcEngine.h>
#include <FMWrapper/FMXText.h>
using namespace fmplug;

Function::Function(const std::string& name, const Arguments& args, unsigned int flags)
	: _name(name),
	_args(args),
	_flags(flags),
	_boundId(-1),
	_retType(AT_VARIANT)
{
}

Function::Function(const std::string& declaration, unsigned int flags)
	: _boundId(-1),
	_flags(flags)
{
	parseDeclaration(declaration);
}

short int Function::getId() const
{
	if (_boundId < 0)
		throw Error("Unbound function '%s' has no Id (yet)", _name.c_str());
	return _boundId;
}

std::string Function::getPrototype() const
{
	std::stringstream ss;
	ss << _name;
	if (!_args.empty())
	{
		ss << " ( ";
		for (size_t i=0;i<_args.size();++i)
			ss << (i>0 ? " ; " : "") << _args[i].name;
		ss << " )";
	}
	return ss.str();
}

void Function::bind(short int id)
{
	_boundId = id;
}

int Function::getMinArgs() const
{
	int count = 0;
	for (Arguments::const_iterator it = _args.begin(); it != _args.end(); ++it)
	{
		if (it->optional)
			return count;
		count++;
	}
	return count;
}

int Function::getMaxArgs() const
{
	if (_flags & F_UNLIMITED_ARGUMENTS)
		return -1;
	else
		return (int)_args.size();
}

void Function::parseDeclaration(const std::string& declaration)
{
	// Grammar for the declaration:
	// ===================================================
	// declaration ::= funcname '(' arglist ')' ':' ret
	// arglist     ::= arg { ',' arg }
	// arg         ::= argtype ' ' argname
	// argtype     ::= type-id
	// ret         ::= type-id
	// funcname    ::= identifier
	// argname     ::= identifier
	// identifier  ::= [a-zA-Z] strings // not so formal but you get the point
	// type-id     ::= 'var' | 'int' | 'text' | 'string' | 'float' | 'double' | 'date'

	// Note that the scanner (tokenizer) and parser probably do not match this
	// grammar perfectly. When in doubt, the grammar tells you how things should
	// have been done, and the implementation how they have been actually done.
	// Note: if you see that the implementation misbehaves (w.r.t grammar), please
	// fix the implementation.
	// Note 2: if you find that the grammar is wrong, update it AND FIX THE 
	// IMPLEMENTATION so that it stays in sync.

	// Cut the string into tokens
	enum TokenType {
		TOK_IDENTIFIER,
		TOK_COMMA,
		TOK_LPAREN,
		TOK_RPAREN,
		TOK_COLON
	};
	struct Token {
		Token(TokenType type, const std::string& value) : type(type), value(value) {}
		Token(TokenType type, char value) : type(type), value("") { this->value += value; }
		TokenType type;
		std::string value;
	};
	typedef std::map<char,TokenType> TokenMap;
	TokenMap tokenMap;
	tokenMap[','] = TOK_COMMA;
	tokenMap['('] = TOK_LPAREN;
	tokenMap[')'] = TOK_RPAREN;
	tokenMap[':'] = TOK_COLON;

	typedef std::vector<Token> Tokens;
	Tokens tokens;

	std::string curToken;
	for (std::string::const_iterator it = declaration.begin(); it != declaration.end(); ++it)
	{
		TokenMap::const_iterator typeit = tokenMap.find(*it);
		bool isSpace = *it == ' ';
		if (isSpace || typeit != tokenMap.end())
		{
			if (!curToken.empty())
				tokens.push_back(Token(TOK_IDENTIFIER,curToken));
			curToken = "";
			if (!isSpace)
				tokens.push_back(Token(typeit->second,*it));
		}
		else
		{
			curToken += *it;
		}
	}
	if (!curToken.empty())
		tokens.push_back(Token(TOK_IDENTIFIER,curToken));

	typedef std::map<std::string,Function::ArgumentType> ArgMap;
	ArgMap am;
	am["var"] = Function::AT_VARIANT;
	am["int"] = Function::AT_INTEGER;
	am["text"] = Function::AT_TEXT;
	am["string"] = Function::AT_TEXT;
	am["float"] = Function::AT_FLOAT;
	am["double"] = Function::AT_FLOAT;
	am["date"] = Function::AT_DATETIME;

	Function::Arguments args;

	int state = 0;
	
	std::string name;
	Function::ArgumentType retType;
	Function::ArgumentType type;

	for (Tokens::const_iterator it=tokens.begin();it!=tokens.end();++it)
	{
		if (state == 0)
		{
			// function name
			if (it->type != TOK_IDENTIFIER)
				throw Error("Identifier expected, found '%s'", it->value.c_str());
			name = it->value;
			++state;
		}
		else if (state == 1)
		{
			// opening parenthesis
			if (it->type != TOK_LPAREN)
				throw Error("Expected '(' but found '%s'", it->value.c_str());
			++state;
		}
		else if (state == 2)
		{
			// type
			if (it->type != TOK_IDENTIFIER)
				throw Error("Identifier expected, found '%s'", it->value.c_str());
			ArgMap::const_iterator typeit = am.find(it->value);
			if (typeit == am.end())
				throw Error("Unknown argument type: '%s'", it->value.c_str());
			type = typeit->second;
			++state;
		}
		else if (state == 3)
		{
			// variable name
			if (it->type != TOK_IDENTIFIER)
				throw Error("Identifier expected, found '%s'", it->value.c_str());
			args.push_back(Function::Argument(type, it->value));
			++state;
		}
		else if (state == 4)
		{
			// comma
			if (it->type == TOK_COMMA)
			{
				state = 2;
			}
			else if (it->type == TOK_RPAREN)
			{
				++state;
			}
			else
			{
				throw Error("Expected ',' or ')' but found '%s'", it->value.c_str());
			}
		}
		else if (state == 5)
		{
			// colon before return type
			if (it->type != TOK_COLON)
				throw Error("Expected ':' but found '%s'", it->value.c_str());
			++state;
		}
		else if (state == 6)
		{
			// return type
			if (it->type != TOK_IDENTIFIER)
				throw Error("Identifier expected, found '%s'", it->value.c_str());
			ArgMap::const_iterator typeit = am.find(it->value);
			if (typeit == am.end())
				throw Error("Unknown argument type: '%s'", it->value.c_str());
			retType = typeit->second;
			++state;
		}
		else
		{
			throw Error("Syntax error");
		}
	}
	
	if (state != 7)
		throw Error("Syntax error");

	_args.swap(args);
	_retType = retType;
	_name = name;
}

void Function::addArgument(const Argument& arg)
{
	// Constraint: all optional arguments must be at the end, consecutively
	bool hasOpts = false;
	for (Arguments::const_iterator it=_args.begin();it!=_args.end();++it)
	{
		if (it->optional)
		{
			hasOpts = true;
			break;
		}
	}
	
	if (hasOpts && !arg.optional)
		throw Error("Optional arguments may be not be followed by mandatory arguments");

	_args.push_back(arg);
}

void Function::call(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
{
	// Check we have the correct number of arguments (if for some reason FileMaker happens to call
	// us oddly)
	int min = getMinArgs(), max = getMaxArgs();
	if ((int)parms.Size() < min)
		throw Error("Not enough arguments: requires at least %d, got %u", min, parms.Size());
	if (max >= 0 && (int)parms.Size() > max)
		throw Error("Too many arguments: requires at most %d, got %u", max, parms.Size());

	// Call the actual implementation
	(*this)(env, parms, result);
}


void Function::operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
{
	fmx::TextAutoPtr text;
	text->Assign("pure virtual function call");
	result.SetAsText(*text, parms.At(0).GetLocale());
}
