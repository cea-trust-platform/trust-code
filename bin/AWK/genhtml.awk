ub
# Awk program that takes as input a TRUST header file and outputs
# a HTML page.
#

# exemple de lien :
#<a href="Probleme.html"><strong><em>Probleme</strong></em></a>
#
BEGIN {
    library= "trio_u"
    sourcefilecount= 1
    definecount= 1
    constcount= 1
    includecount= 1
    typedefcount= 1
    enumcount= 1
    structcount= 1
    externcount= 0
    classcount= 0
    predoccount= 0
    doccount= 0
    motclecount= 0
    lp_indent= "    "
    bold= ""
    directory="*"
    longtmp="*"
    slongtmp="*"

    sourcefile[0]=  "%h" "fichiers sources"
    reffile[0]=    "%h" "fichiers sources"
    define[0]=      "%h" "macros definies"
    const[0]=       "%h" "constantes"
    include[0]=     "%h" "fichiers inclus"
    typedef[0]=     "%h" "definitions de types"
    enum[0]=        "%h" "enumerations"
    struct[0]=      "%h" "structures"
    }

/^#define/ {
    tmp= substr( $0, 8 )
    tmp= strip_white( tmp )
    gsub( "\\\\$", "", tmp );
    define[ definecount++ ] = tmp
    }

/^const/ {
    tmp= substr( $0, 6 )
    tmp= strip_white( tmp )
    gsub( "//", "", tmp );
    const[ constcount++ ] = tmp
    }

/^#include/ {
    tmp= substr( $0, 9 )
    tmp= strip_white( tmp )   
    gsub( "\"", "", tmp )
    gsub( "<", "", tmp )
    gsub( ">", "", tmp )
    include[ includecount++ ] = tmp
    }

/^extern/ {
    tmp= substr( $0, 7 )
    tmp= strip_white( tmp )
    extern[ externcount++ ] = tmp
    }

/^typedef/ {
    tmp= substr( $0, 8 )
    tmp= strip_white( tmp )
    typedef[ typedefcount++ ] = tmp
    }

/^enum/ {
    tmp= substr( $0, 5 )
    fin=""
    if( tmp ~ ".*\\}.*" )
     fin="fin";
    gsub( ",", " ", tmp );
    gsub( "//", "", tmp );
    gsub( "\{", ": ", tmp );
    gsub( "\}", ".", tmp );
    gsub( ";", "", tmp );
    tmp= strip_white( tmp )
    enum[ enumcount++ ] = tmp
    if (fin !="fin")
    while( (getline tmp) > 0 )
        {
        if( tmp ~ ".*\\}.*" )
           fin="fin";
        gsub( ",", " ", tmp );
        gsub( "//", "", tmp );
        gsub( "\{", ":", tmp );
        gsub( "\}", "", tmp );
        gsub( ";", "", tmp );
        tmp= strip_white( tmp )
	if( length(tmp) > 0 )
	    enum[ enumcount++ ] = "%@" tmp
        if (fin =="fin")
           break;
        }
    }

/^struct/ {
    process_struct( $0 )
    }

/^class/ {
    process_class( $0 )
    classcount++
    }

/^\/\// {
    tmp= substr( $0, 4 )
    striptmp= strip_white( tmp )
    if( gsub( "Directory:", "", striptmp ) == 1 )
        { 
	  directory=striptmp
	  sub("\\$TRUST_ROOT","..",directory)
        }
    else
    if( gsub( "Directory :", "", striptmp ) == 1 )
        { 
	  directory=striptmp
	  sub("\\$TRUST_ROOT","..",directory)
        }
    else
    if( gsub( "Directory", "", striptmp ) == 1 )
        { 
	  directory=striptmp
	  sub("\\$TRUST_ROOT","..",directory)
        }
    else
    if( gsub( "^\.FILE", "", striptmp ) == 1 )
        {
	headtmp="<a href=\""
	fintmp="\">"
	finfintmp="</a>"
        nomtmp= strip_white( striptmp )
        sourcefile[ sourcefilecount ] = nomtmp
	if(directory=="*")
	{
          print nomtmp > "ERREURS.credoc"
	  print nomtmp > "/tmp/tmp2"
	  close "/tmp/tmp2"
	  "dojob" | getline longtmp 
	  close "dojob"
	}
	else
          longtmp=directory"/"nomtmp
	# Rajout pour avoir le fichier s. accessible   
	snomtmp="s."nomtmp
	if(directory=="*")
	{
	  print snomtmp > "/tmp/tmp2"
	  close "/tmp/tmp2"
	  "dojob" | getline slongtmp
	  close "dojob"
	}
        else
          slongtmp=directory"/.sccs/"snomtmp

	# striptmp=headtmp longtmp fintmp nomtmp finfintmp
	striptmp=headtmp longtmp fintmp "Voir " nomtmp finfintmp "\t\t" headtmp slongtmp fintmp "Modifier " nomtmp finfintmp
        reffile[ sourcefilecount++ ] = striptmp
        }
    else
    if( gsub( "^\.LIBRAIRIE", "", striptmp ) == 1 )
        {
        striptmp= strip_white( striptmp )
        library= tolower( striptmp )
        }
    else
    if( gsub( "^\.NOM", "", striptmp ) == 1 )
        {
        namehead= strip_white( striptmp )
        }
    else
    if( gsub( "^\.ENTETE", "", striptmp ) == 1 )
        {
        striptmp= strip_white( striptmp )
        }
    else
    if( gsub( "^\.INCLUDE", "", striptmp ) == 1 )
        {
        includefile= strip_white( striptmp )
        }
    else
    if( gsub( "^\.MOTCLES", "", striptmp ) == 1 )
        {
        motscle[motclecount++ ]= ".\\\" entree par motcle " striptmp
        }
    else
    if( gsub( "^\.DESCRIPTION", "", striptmp ) == 1 )
        {
	if( predoccount > 0 )
            predoc[ predoccount++ ]= ".SK"
        striptmp= strip_white( striptmp )
        striptmp= toupper( striptmp )
        predoc[ predoccount++ ]= "%h" striptmp
        predoc_flag= "true"
        }
    else
    if( gsub( "^\.SECTION", "", striptmp ) == 1 )
        {
	if( doccount > 0 )
            doc[ doccount++ ]= ".SK"
        striptmp= strip_white( striptmp )
        striptmp= toupper( striptmp )
        doc[ doccount++ ]= "%h" striptmp
        doc_flag= "true"
        }
    else 
    if( doc_flag == "true" )
        {
        doc[ doccount++ ]= tmp
        }
    else 
    if( predoc_flag == "true" )
        {
        predoc[ predoccount++ ]= tmp
        }
    }

NF == 0 {
    if( doc_flag == "true" )
	{
        doc_flag= ""
	}
    else
    if( predoc_flag == "true" )
	{
        predoc_flag= ""
	}
    }

END {
    # get the modify date of the file
    if( length(mtime) == 0 )
	{
	"ls -l " FILENAME | getline tmp
	date= substr( tmp, 33, 12 );
	}
    else
	{
	mtime " " FILENAME | getline date
	}

    # If .NOM not specified, then make one up
    if( length( namehead ) == 0 )
	namehead=  FILENAME " - ???"

    # Make a name for the man page from the first word in the
    # NOM section.
    pos= match( namehead, "[ ,]" );
    if( pos == 0 )
	titlename= namehead
    else
	titlename= substr( namehead, 1, pos-1 )
    titlename= toupper(titlename)

    # If .ENTETE not specified then use C++
    if( length( header ) == 0 )
        header= "TRUST"

    # If .LIBRAIRIE not specified then use c++
    if( length( library ) == 0 )
        library= "c++"

    # Start the formatting and print the title and name
    format_start( titlename, library, header, date, namehead );

    # print the synopsis header
    format_line( "%h" "INTERFACE" )
    format_line( ".BO" )
    format_line( ".NF" )

    if( length( includefile ) == 0 )
        includefile= FILENAME;
    format_line( ".NB" )
    format_line( "#include < " includefile  " >" )
    format_line( ".SK" )

    output_section( predoc )
    output_class()
    output_section( extern )

    format_line( ".FI" )

    format_line( ".NF" )
    if( structcount > 1 ) output_section( struct )
    if( enumcount > 1 ) output_section( enum )
    if( typedefcount > 1 ) output_section( typedef )
    if( constcount > 1 ) output_section( const )

    output_summarys()

    output_section( doc )
    
    if( definecount > 1 ) output_section( define )
    if( includecount > 1 ) output_section( include )
    if( sourcefilecount > 1 ) output_section( reffile )

    if( motclecount > 0 ) {
    	output_section( motscle )
    }
    # end the formatting
    format_end()
    }

# Call to process a class.  Pass the name of the class.
function process_class( name,    inputstr,ii,bers )
    {
    privatecount= 1
    publiccount= 1
    protectedcount= 1
    indent_level= "%@"
    indent_post= "%@"
    
#modif :    
    if (name ~ ".*;.*" ) 
    {
       classcount--
       return;
    }

    # We do not record the curly braces surrounding the class
    # but we record all others.

    # If an open curly brace on the "class" line then incriment the
    # indent level.
    if( name ~ ".*\\{.*" )
	{
	indent_post= indent_post "%@"
	sub( "\{", "", name );
	}
	
    # record the name
    name= strip_white( name )
    class[ classcount, "head", 0 ]= name

    # get lines of input until EOF
    ii= "private"
    while( (getline inputstr) > 0 )
        {
	# on vire les declare...
	if( inputstr ~ ".*declare.*" )
	   inputstr=""
        # just get the token(s), ignore blank lines
        inputstr= strip_white( inputstr )
	if( length(inputstr) == 0 )
	    continue;

        # see if should adjust the indent level
	
	if( inputstr ~ ".*\\{.*" )
	{
	   indent_post= indent_post "%@"
	
	   if( length(indent_post) == 4 )
	      continue;
	}
	else
	if( inputstr ~ "}.*" )
	{
   	   if( length(indent_post) == 4 )
	      break;
	}
	
	if( inputstr ~ ".*\\(.*" )
	{
	   indent_post= indent_post "%@"
	
	   if( length(indent_post) == 4 )
	      continue;
	}
	else
	if( inputstr ~ ".*\\).*" )
	{
   	   if( length(indent_post) == 4 )
	      break;
	}

        # public members follow
        if( inputstr ~ "public *:" )
            ii= "public"

        # private members follow
        else if( inputstr ~ "private *:" )
            ii= "private"

        # protected members follow
        else if( inputstr  ~ "protected *:" )
            ii= "protected"

        else
            {
            gsub( "//", "  ", inputstr )
	    tmp= indent_level inputstr
	    if( ii == "private" )
		class[ classcount, "private", privatecount++ ]= tmp
	    else
	    if( ii == "public" )
		class[ classcount, "public", publiccount++ ]= tmp
	    else
	    if( ii == "protected" )
		class[ classcount, "protected", protectedcount++ ]= tmp
            }

        
	    if( inputstr ~ "}.*" )
	        {
	        indent_post= substr( indent_post, 3 )
	        if( length(indent_post) == 2 )
                    break;
	        }
	    if( inputstr ~ ".*\\).*" )
	        {
	        indent_post= substr( indent_post, 3 )
	        if( length(indent_post) == 2 )
                    break;
	        }
	    indent_level = indent_post
	}

    class[ classcount, "public", 0 ]= publiccount
    class[ classcount, "protected", 0 ]= protectedcount
    class[ classcount, "private", 0 ]= privatecount
    }

# processes a structure
function process_struct( tmp,     style,indent_level )
    {
    indent_level= ""

    if( tmp ~ ".*\\{.*" )
	style= "k&r"
    else
	style= "custom"

    do
        {
        tmp= strip_white( tmp )
        gsub( "//", "  ", tmp );

	if( style == "k&r" )
	    {
	    if( tmp ~ "}.*" )
	        {
	        indent_level= substr( indent_level, 3 )
	        if( length(indent_level) == 0 )
		    {
		    struct[ structcount++ ] = indent_level tmp
                    break;
		    }
	        }
	    struct[ structcount++ ] = indent_level tmp
	    if( tmp ~ ".*\\{.*" )
	        {
	        indent_level= indent_level "%@"
	        }
	    }
	else
	    {
	    if( tmp ~ ".*\\{.*" )
	        {
	        indent_level= indent_level "%@"
	        }
	    struct[ structcount++ ] = indent_level tmp
	    if( tmp ~ "}.*" )
	        {
	        indent_level= substr( indent_level, 3 )
	        if( length(indent_level) == 0 )
                    break;
	        }
	    }

        }while( (getline tmp) > 0 )
    }

#
# Outputs the specified array.
#
function output_section( data,   tmp,xx )
    {
    # Traverse the list of lines.
    xx= 0
    while( xx in data )
	{
	format_line( data[xx] )
	xx++
        }
    if( xx != 0 )
        format_line( ".SK" )
    }

# Special funtion for printing out classes because the class
# was built as a multi-dimensional array.
function output_class(   xx,hd,cname)
    {
    # for each class
    for( xx= 0; xx < classcount; xx++ )
	{
	# output the name of the class
        format_line( "</p>" )
        format_line( "<p>" )
        format_line( ".BO" )
	format_line( class[ xx, "head", 0 ] )
        format_line( ".NB" )
	format_line( ".SK" )

	output_part_class( "%@" "Membres publics :", "public", xx );
	output_part_class( "%@" "Membres proteges :", "protected", xx );
	output_part_class( "%@" "Membres prives :", "private", xx );
	}
    }

# Prints the specified portion of the class.
function output_part_class( header, idname, idnum, yy,amount )
    {
    # get the number of items
    amount= class[ idnum, idname, 0 ];
    if( amount == 1 )
        return;

    # output the header
    format_line( ".BO" )
    format_line( header )
    format_line( ".NB" )

    for( yy= 1; yy < amount; yy++ )
	{
	format_line( class[ idnum, idname, yy ] );
	}
    format_line( ".SK" )
    }

# Call to get the function summarys from the source files and print them
function output_summarys(data,title,xx,datacount,titlecount )
    {
    # just return if no source files listed
    if( sourcefilecount == 1 )
        return;

    # print the header
    format_line( "%h" "DESCRIPTION DES METHODES" )

    # number of descriptions found
    descriptioncount= 0;

    # walk the list of source files
    for( xx = 1; xx < sourcefilecount; xx++ )
        {
        # loop reading each line of the source file
	while( (getline inputstr <sourcefile[xx]) > 0 )
            {
            # see if the start of the function summary
	    if( inputstr == "// Description:" )
		{
		if( descriptioncount > 0 )
		    format_line( ".SK" );
		descriptioncount++

		# number of lines to output for this description
		datacount= 0;
		titlecount= 0;

                # loop reading the summary
		while( (getline inputstr <sourcefile[xx]) > 0 )
		    {
                    # see if all done with this description
		    if( inputstr ~ ".*\\{.*" )
			{
			# output what we collected for the description
			format_line( ".BO" )
			for( yy= 0; yy < titlecount; yy++ )
			    format_line( title[yy] );
			format_line( ".NB" )
			for( yy= 0; yy < datacount; yy++ )
			    format_line( data[yy] );
			break;
			}

                    # strip the comment and leading whitespace
		    if( gsub( "//[\t ]*", "", inputstr ) != 0 )
			{
                        # only keep lines not ending in :
			if( inputstr !~ ".*:$" )
			    {
                            # save the line of data
			    data[ datacount++ ] = "%@" inputstr
			    }
			}
		    else
			{
                        # only keep lines that are not blank
			if( length( inputstr ) )
			    {
                            # save the line of the function name
			    title[ titlecount++ ] = inputstr
			    }
			}
		    }
		}
	    }
	}
    }

# strips leading and trailing whitespace from the specified string and
# returns the string
function strip_white( data )
    {
    gsub( "^[\t ]+", "", data )
    gsub( "[\t ]+$", "", data )
    return data
    }

# This routine takes a line of data and converts it into
# the appropriate format for the output device.
#
# Documentation of imbedded formatting commands.
#   .FI on a line by itself means turn on fill mode
#   .NF on a line by itself means turn off fill mode
#   .BO on a line by itself means turn on bold mode
#   .NB on a line by itself means turn off bold mode
#   .SK on a line by itself means skip a line
#   %h means the rest of the line is a section header
#   %@ means indent one level
function format_line( data )
    {
    # This section is for producing troff output.
    if( device == "html" )
	{
	# see if a special formatting command
	if( length(data) == 3 )
	    {
	    if( data == ".FI" )
		{
		nofill= ""
		return;
		}
	    else
	    if( data == ".NF" )
		{
		nofill= ""
		return;
		}
	    else
	    if( data == ".BO" )
		{
		bold= ""
		return;
		}
	    else
	    if( data == ".NB" )
		{
		bold= ""
		return;
		}
	    else
	    if( data == ".SK" )
		{
		return;
		}
	    }

	# See if a heading.
	if( data ~ "%h.*" )
	    {
	    print "</p>"
	    data= substr( data, 3 );
	    print toupper(data) 
	    print "<p>"
	    }

	# else a regular line
	else
	    {
	    # convert indent marks to spaces
	    gsub( "%@", "   ", data )

	    # add embolding
	    data= bold data

	    # print the data
            print data
	    }
	}

    # This section is for producing ascii formatted output.
    else
	{
	# see if a special formatting command
	if( length(data) == 3 )
	    {
	    # Ignore fill and bold transitions.
	    if( (data == ".FI") || (data == ".NF") ||
		(data == ".BO") || (data == ".NB") )
		return;

	    # see if should skip a line
	    if( data == ".SK" )
		{
		print
		return;
		}
	    }

	# See if a heading.
	if( data ~ "%h.*" )
	    {
	    data= substr( data, 3 );
	    print toupper(data)
	    }

	# else a regular line
	else
	    {
	    # convert indent marks to spaces
	    gsub( "%@", lp_indent, data )

	    # add default indent amount
	    data= lp_indent data

	    # if more then 79 characters, trim it
	    if( length(data) > 79 )
	        data= substr( data, 1, 79 );
            print data
	    }
	}
    }

# This routine is called once to perform any device specific initialization
# and to setup the title line and the date line.
function format_start( titlename, library, header, date, namehead )
    {
    # This section is for producing html output.
    if( device == "html" )
	{
	printf("<!--NewPage-->\n")
	printf("<html>\n")
	printf("<head>\n")
	printf("<BODY BGCOLOR=\"#C0D9D9\" TEXT=\"#23238E\" LINK=\"#E47833\" ALINK=\"#007FFF\" VLINK=\"#007FFF\">\n")
	printf( "<title> %s </title>\n", titlename)
	printf( "<h2> %s </h2>\n", header) ;
	printf( "<h3> %s %s %s </h3> \n",
	       titlename, library, date );
	sub( "-", "\\-", namehead )
	format_line( namehead )
	format_line( ".SK" )
	printf("</head>\n")
	printf("<body>\n")
	printf("<pre>\n")
	}

    # This section is for producing ascii formatted output.
    else
	{
	hlen= (length( header ) +1) / 2;
	hlen= hlen * 2;
	tlen= (78 - hlen) / 2;
	fmtstr= sprintf( "%%-%ds%%-%ds%%%ds\n", tlen, hlen, tlen );
	titlename= titlename "(" library ")"
	print
	printf( fmtstr, titlename, header, titlename );
	print
	printf( "%77s\n", date )
	format_line( "%h" "name" )
	format_line( namehead )
	format_line( ".SK" )
	}
    }

# This routine is called once to perform any device specific
# cleanup.
function format_end()
    {
    # This section is for producing troff output.
    if( device == "html" )
	{
	printf("</pre>\n")
	printf("</body>\n")
	printf("</html>\n")
	}

    # This section is for producing ascii formatted output.
    else
	{
	}
    }
