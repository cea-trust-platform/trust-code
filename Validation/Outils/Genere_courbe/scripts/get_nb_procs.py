#
# Given a PRM file, extract for each test case the number of procs required.
# This is echoed on stdout in the same order the tests were found in the PRM.
#

def get_nb_procs( filename ) :
    test_cases=["TESTCASE","CASTEST"]
    
    all_nb_procs=[]
    with open( filename, "r" ) as f:
        for line in f :
            split = line.split( )
            nb_words = len( split )
            if nb_words > 0 :
                first_word = split[ 0 ]
                if first_word.upper() in test_cases :
                    rep = split[ 1 ]
                    test = split[ 2 ]
                    if nb_words == 3 :
                        all_nb_procs.append( 1 )
                    else :
                        word = split[ 3 ]
                        if word.isdigit( ) :
                            all_nb_procs.append( int(word) )
                        else :
                            all_nb_procs.append( 1 )
                            
    nb_procs = max( all_nb_procs )
    return nb_procs, all_nb_procs

if __name__ == "__main__":
    import sys
    filename = sys.argv[ 1 ]
    nb_procs, all_nb_procs = get_nb_procs( filename )
    print ("\n".join([str(n) for n in all_nb_procs]))

