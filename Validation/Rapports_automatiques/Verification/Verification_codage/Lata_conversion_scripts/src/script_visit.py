nb_args = len( sys.argv )

if( nb_args < 4 ) :
    print "Erreur, pas assez d'arguments"
    y=1/0
if ( nb_args > 4 ) :
    print "Erreur, il y a trop d'arguments"
    y=1/0

input_file_name=sys.argv[1]
output_file_name=sys.argv[2]

OpenDatabase(input_file_name,0)
nts = TimeSliderGetNStates()

with open(output_file_name, 'wb') as output_file:
    output_file.write( "Number of detected time steps for "+str(input_file_name)+" : "+str(nts) )


exit()
