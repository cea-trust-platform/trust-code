#! /usr/bin/env python
#****************************************************************************
# Copyright (c) 2015, CEA
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
#*****************************************************************************

# --
# Author : EL (CEA)
# --

# This script provides skeletons for classes added either to TRUST, or to a BALTIK project.

class ErrorManager( object ):
    def __init__( self, parser ):
        self.parser_ = parser

    def error( self, message ):
        self.parser_.error( message )

# location determination
def build_location_in_triou_mode( error_manager ):
    import os

    trio_u_root = os.getenv("TRUST_ROOT")

    if trio_u_root == None:
        error_manager.error( "Undefined environment variable: TRUST_ROOT" )

    if os.path.commonprefix( ( os.getcwd( ), trio_u_root ) ) != trio_u_root:
        error_manager.error( "Invalid location: 'baltik_gen_class --triou-mode' should be used in a subdirectory of $TRUST_ROOT." )        

    return os.path.join( "$TRUST_ROOT", os.path.relpath( os.getcwd( ), trio_u_root ) )

def build_location_in_baltik_mode( error_manager ):
    import os
    import ConfigParser

    directories = []
    ( head, tail ) = os.path.split(  os.getcwd( ) )
    directories.append( tail )
    while ( ( tail != "src" ) and ( head != "/" ) ) :
        ( head, tail ) = os.path.split( head )
        directories.append( tail )
        
    if ( tail != "src" ) :
        error_manager.error( "Invalid location: can't find the src directory of the BALTIK project.")

    project_configuration_file = os.path.join( head, "project.cfg" )

    if ( not os.path.exists( project_configuration_file ) ) :
        error_manager.error( "Invalid location: can't find BALTIK project configuration file." )

    if ( not os.path.isfile( project_configuration_file ) ) :
        error_manager.error( "Invalid location: can't find BALTIK project configuration file." )

    config_parser = ConfigParser.ConfigParser( )
    config_parser.read( project_configuration_file )
    project_name = config_parser.get( "description", "name" )

    directories.append( project_name )
    directories.reverse( )
    project_location = os.path.join( *directories )
    return "{0} {1}".format( project_name, project_location )

# classname analysis utility function
def classname_analysis( classname, error_manager ):
    import re

    root = classname
    root = re.sub( "^\s*",  "",  root )
    root = re.sub( "\s*$",  "",  root )
    root = re.sub( "\(\s*", "(", root )
    root = re.sub( "\s*\)", ")", root )

    macros = []

    pattern = "^(REF|DERIV|LIST|VECT)\((.*)\)$"

    match = re.match( pattern, root )
    while ( match ):
        macros += [ match.group( 1 ) ]
        root    = match.group( 2 ) 
        match   = re.match( pattern, root )
        
    if ( ' ' in root ) or ( '(' in root ) or ( ')' in root ):
        error_manager.error( "Invalid classname: '{0}'".format( classname ) )

    return ( macros, root )


# code for standard classes
def get_header_template_for_standard_class( ):
    return """/////////////////////////////////////////////////////////////////////////////
//
// File      : {header_filename}
// Directory : {location}
//
/////////////////////////////////////////////////////////////////////////////

#ifndef {inclusion_guard}
#define {inclusion_guard}

{header_inclusions}

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class {classname}
//
// <Description of class {classname}>
//
/////////////////////////////////////////////////////////////////////////////

class {classname} : public {superclassname}
{opening_brace}

  {declaration_macro}( {classname} ) ;

public :

protected :

{closing_brace};

#endif /* {inclusion_guard} */
"""

def get_source_template_for_standard_class( ):
    return """/////////////////////////////////////////////////////////////////////////////
//
// File      : {source_filename}
// Directory : {location}
//
/////////////////////////////////////////////////////////////////////////////

{source_inclusions}

{implementation_macro}( {classname}, \"{classname}\", {superclassname} ) ;

Sortie& {classname}::printOn( Sortie& os ) const
{opening_brace}
  {superclassname}::printOn( os );
  return os;
{closing_brace}

Entree& {classname}::readOn( Entree& is )
{opening_brace}
  {superclassname}::readOn( is );
  return is;
{closing_brace}
"""

def build_keywords_for_standard_class( arguments, error_manager ):
    keywords = { }

    ( macros, root ) = classname_analysis( arguments.classname, error_manager )
    if len( macros ) != 0:
        error_manager.error( "Invalid class name: '{0}'".format( arguments.classname ) )

    keywords[ "classname" ] = root
    if ( arguments.classtype ) == "base":
        keywords[ "classname" ] += "_base"

    if ( arguments.classtype not in ( "instanciable", "base" ) ):
        error_manager.error( """Invalid class type: '{0}'
  Expected class types for standard class are 'instanciable' and 'base'.
""".format( arguments.classtype ) )

    keywords[ "declaration_macro"    ] = "Declare_{0}".format( arguments.classtype )
    keywords[ "implementation_macro" ] = "Implemente_{0}".format( arguments.classtype )

    keywords[ "inclusion_guard" ] = "{0}_included".format( keywords[ "classname" ] )
    
    keywords[ "header_filename" ] = "{0}.h".format( keywords[ "classname" ] )
    keywords[ "source_filename" ] = "{0}.cpp".format( keywords[ "classname" ] )

    ( macros, root ) = classname_analysis( arguments.superclass, error_manager )

    superclasslist = [ "{0}(".format( x ) for x in macros ]
    superclasslist.append( root )
    superclasslist.extend( [ ")" for x in macros ] )
    keywords[ "superclassname" ] = " ".join( superclasslist )
    
    prefixes_ = {}
    prefixes_[ "REF" ]   = "Ref"
    prefixes_[ "DERIV" ] = "Deriv"
    prefixes_[ "LIST" ]  = "List"
    prefixes_[ "VECT" ]  = "Vect"

    inclusions = [ "{0}.h".format( root ) ]
    macros.reverse( )
    for m in macros:
        inclusions.append( "_".join( ( prefixes_[ m ], inclusions[ -1 ] ) ) )
    keywords[ "header_inclusions" ] = "#include <{0}>".format( inclusions.pop( ) )
    inclusions.append( keywords[ "header_filename" ] )
    inclusions.reverse( )
        
    keywords[ "source_inclusions" ] = "\n".join( [ "#include <{0}>".format( x ) for x in inclusions ] )
    
    if ( arguments.triou_mode ):
        keywords[ "location" ] = build_location_in_triou_mode( error_manager )
    else:
        keywords[ "location" ] = build_location_in_baltik_mode( error_manager )
        
    return keywords


def build_templates_and_keywords_for_standard_class( arguments, error_manager ):
    header_template = get_header_template_for_standard_class( )
    source_template = get_source_template_for_standard_class( )
    keywords = build_keywords_for_standard_class( arguments, error_manager )
    return ( header_template, source_template, keywords )





# code for macro classes
def get_header_template_for_macro_class( ):
    return """/////////////////////////////////////////////////////////////////////////////
//
// File      : {header_filename}
// Directory : {location}
//
/////////////////////////////////////////////////////////////////////////////

#ifndef {inclusion_guard}
#define {inclusion_guard}

{header_inclusions}
class {classname} ;
{declaration_macro}( {classname} ) ;

#endif /* {inclusion_guard} */
"""

def get_source_template_for_macro_class( ):
    return """/////////////////////////////////////////////////////////////////////////////
//
// File      : {source_filename}
// Directory : {location}
//
/////////////////////////////////////////////////////////////////////////////

{source_inclusions}

{implementation_macro}( {classname} ) ;
"""

def build_keywords_for_macro_class( arguments, error_manager ):
    keywords = { }

    ( macros, root ) = classname_analysis( arguments.classname, error_manager )
    
    classlist = [ "{0}(".format( x ) for x in macros ]
    classlist.append( root )
    classlist.extend( [ ")" for x in macros ] )
    keywords[ "classname" ] = " ".join( classlist )

    classtypes_                 = { }
    classtypes_[ "reference"  ] = "ref"
    classtypes_[ "derivation" ] = "deriv"
    classtypes_[ "vector"     ] = "vect"
    classtypes_[ "list"       ] = "list"

    classtype = classtypes_[ arguments.classtype ]

    keywords[ "declaration_macro" ] = "Declare_{0}".format( classtype )
    keywords[ "implementation_macro" ] = "Implemente_{0}".format( classtype )

    macros.insert( 0, classtype.upper( ) )

    prefixes_ = { }
    prefixes_[ "REF" ]   = "Ref"
    prefixes_[ "DERIV" ] = "Deriv"
    prefixes_[ "LIST" ]  = "List"
    prefixes_[ "VECT" ]  = "Vect"

    expanded_classlist = [ prefixes_[ x ] for x in macros ]
    expanded_classlist.append( root )
    expanded_classname = "_".join( expanded_classlist )
    
    keywords[ "header_filename" ] = "{0}.h".format( expanded_classname )
    keywords[ "source_filename" ] = "{0}.cpp".format( expanded_classname )
    keywords[ "inclusion_guard" ] = "{0}_included".format( expanded_classname )

    header_inclusions = [ x for x in set( macros ) ]
    header_inclusions.remove( classtype.upper( ) )
    header_inclusions.insert( 0, classtype.upper( ) )
    keywords[ "header_inclusions" ] = "\n".join( [ "#include <{0}.h>".format( prefixes_[ x ] ) for x in header_inclusions ] )

    source_inclusions = [ "{0}.h".format( root ) ]
    macros.reverse( )
    for m in macros:
        source_inclusions.append( "_".join( ( prefixes_[ m ], source_inclusions[ -1 ] ) ) )
    source_inclusions.reverse( )
        
    keywords[ "source_inclusions" ] = "\n".join( [ "#include <{0}>".format( x ) for x in source_inclusions ] )
    
    if ( arguments.triou_mode ):
        keywords[ "location" ] = build_location_in_triou_mode( error_manager )
    else:
        keywords[ "location" ] = build_location_in_baltik_mode( error_manager )
        
    return keywords


def build_templates_and_keywords_for_macro_class( arguments, error_manager ):
    header_template = get_header_template_for_macro_class( )
    source_template = get_source_template_for_macro_class( )
    keywords = build_keywords_for_macro_class( arguments, error_manager )
    return ( header_template, source_template, keywords )


# argument parser
def parse_arguments( ):
    import argparse

    parser = argparse.ArgumentParser( )
    error_manager = ErrorManager( parser )

    parser.add_argument( "--triou-mode", 
                         action = "store_true",
                         help   = "provides a skeleton for a TRUST class instead of a BALTIK project class" )

    group = parser.add_mutually_exclusive_group( )

    group.add_argument( "-i", "--instanciable",
                        dest   = "classtype", 
                        action = "store_const", 
                        const  = "instanciable",
                        help   = "build a instanciable class skeleton" )

    group.add_argument( "-b", "--base", 
                        dest   = "classtype", 
                        action = "store_const", 
                        const  = "base",
                        help   = "build a base class skeleton" )

    group.add_argument( "-r", "--ref",
                        dest   = "classtype", 
                        action = "store_const", 
                        const  = "reference",
                        help   = "build a reference class" )

    group.add_argument( "-d", "--deriv",
                        dest   = "classtype", 
                        action = "store_const",
                        const  = "derivation",
                        help   = "build a derivation class" )

    group.add_argument( "-l", "--list", 
                        dest   = "classtype",
                        action = "store_const", 
                        const  = "list",
                        help   = "build a list class" )

    group.add_argument( "-v", "--vect",
                        dest   = "classtype", 
                        action = "store_const", 
                        const  = "vector",
                        help   = "build a vector class" )
    
    parser.add_argument( "-s", "--superclass", 
                         dest = "superclass",
                         help = "for instanciable and base classes only : specifies the superclass" )

    parser.add_argument( "classname" )

    parser.set_defaults( classtype  = "instanciable" ) 

    args = parser.parse_args( )
    
    if ( args.superclass is not None ) and ( args.classtype not in ( "instanciable", "base" ) ):
        error_manager.error("-S/--superclass should only be used for specifying instanciable and base classes")

    if ( args.superclass is None ) and ( args.classtype in ( "instanciable", "base" ) ):
        args.superclass = "Objet_U"

    return ( args, error_manager )

# main dispatcher
def build_templates_and_keywords( arguments, error_manager ):
    functions_ = { }
    functions_[ "instanciable" ] = build_templates_and_keywords_for_standard_class
    functions_[ "base"         ] = build_templates_and_keywords_for_standard_class
    functions_[ "reference"    ] = build_templates_and_keywords_for_macro_class
    functions_[ "derivation"   ] = build_templates_and_keywords_for_macro_class
    functions_[ "list"         ] = build_templates_and_keywords_for_macro_class
    functions_[ "vector"       ] = build_templates_and_keywords_for_macro_class
   
    return functions_[ arguments.classtype ]( arguments, error_manager )
        
def main( ):
    ( arguments, error_manager ) = parse_arguments( )

    ( header_template, source_template, keywords ) = build_templates_and_keywords( arguments, error_manager )
    
    keywords[ "opening_brace" ] = "{"
    keywords[ "closing_brace" ] = "}"

    header = header_template.format( **keywords )
    source = source_template.format( **keywords )

    headerfile = open( keywords[ "header_filename" ], 'w' )
    headerfile.write( header )
    headerfile.close( )
                  
    sourcefile = open( keywords[ "source_filename" ], 'w' )
    sourcefile.write( source )
    sourcefile.close( )
    import os
    os.system("check_license_in_file.sh --mode c -add "+keywords[ "header_filename" ])
    os.system("check_license_in_file.sh --mode c -add "+keywords[ "source_filename" ])
if __name__ == '__main__':
    main( )
