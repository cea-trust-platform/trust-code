# Script to convert .plan file (obtained using a plane of probes from TRUST) to LATA format
import sys
import os
from optparse import OptionParser
import numpy as np

# Read .plan file and get the different parameters
def get_parameters(PlanFile):
    f = open(PlanFile,'r')
    lines = f.readlines()
    DataFile = lines[1].rstrip()
    Probe = TestCase2.replace(DataFile+'_','')
    Nx=int(lines[3].split(' ')[3]) # Number of probes in 1st-direction
    Ny=int(lines[3].split(' ')[4]) # Number of probes in 2nd-direction

    if (len(lines[3].split(' ')[:])==11) :
        dimension=2
    else :
        dimension=3
        if twodimensions == 1 :
            dimension=2

    Nelem = int(int(lines[4].split(' ')[2])/2)
    Ncells = int(lines[2*Nelem+8]) # Total number of cells
    Field = lines[3].split(' ')[2]

    if (lines[2*Nelem+Ncells+14].split(' ')[0]=='type0'):
        comp='1'
        nature='scalar'
    elif (lines[2*Nelem+Ncells+14].split(' ')[0]=='type1'):
        comp='2'
        nature='vector'
        if (dimension==3) :
            comp='3'
    return DataFile,Probe,lines,Nx,Ny,Nelem, Ncells,Field,comp, nature, dimension

# Re-write the connection between the different elements
def save_connections(LataFile, Probe):
    nx1=[]
    for i in range((Nx-1)*(Ny-1)):
        nx1.append(0)
    nx1[0]=1
    for i in range((Nx-1)*(Ny-1)):
        nx1[i] = nx1[i-1]+1
        if (i%(Ny-1) == 0 and i >0):
            nx1[i] += 1
    nx2 = [ x+1 for x in nx1 ]
    ny1 = [ x+Ny for x in nx1 ]
    ny2 = [ x+1 for x in ny1 ]

    connections=np.array([nx1, nx2, ny1, ny2]).T
    GridConnections=LataFile+'.'+Probe+'.0.0.elem'
    with open(GridConnections, 'w+') as datafile_id:
        for item in connections:
            datafile_id.write("%s %s %s %s\n" % (item[0], item[1], item[2], item[3]) )

    return GridConnections

# Write the main lata file
def write_lata_file(LataFile,DataFile,Probe,GridCoordinates,GridConnections,Nelem,Ncells,post_processed_Fields,dimension):
    latafileout = open(LataFile,'w')
    latafileout.write('LATA_V2.1\n'+DataFile+'\nTrio_U verbosity=0')
    latafileout.write('\nFormat LITTLE_ENDIAN,INT32,F_INDEXING,C_ORDERING,F_MARKERS_SINGLE,REAL32'+'\nGEOM '+Probe+' type_elem=RECTANGLE')
    latafileout.write('\nCHAMP SOMMETS '+GridCoordinates+' geometrie='+Probe+' size='+str(Nelem)+' composantes='+str(dimension)+' format=ASCII,F_MARKERS_NO')
    latafileout.write('\nCHAMP ELEMENTS '+GridConnections+' geometrie='+Probe+' size='+str(Ncells)+' composantes=4 format=ASCII,INT32,F_MARKERS_NO\n')
    latafileout.write(post_processed_Fields)
    latafileout.write('FIN')
    latafileout.close()

# When dealing with 2D simulations the Z-coordinate is not necessary
# but when dealing with 3D simulations we don't know which coordinate is to remove
# if we want to get a 2D representation of that plane.
# The coordinate to be removed is the one that do not evolve
def determine_component_to_remove( Nelem ) :
    values = lines[ 5 ].split( )
    first_x = float( values[ 0 ] )
    first_y = float( values[ 1 ] )
    first_z = float( values[ 2 ] )
    values = lines[ Nelem+4 ].split( )
    last_x = float( values[ 0 ] )
    last_y = float( values[ 1 ] )
    last_z = float( values[ 2 ] )
    if( first_x == last_x ) :
        return 0
    elif( first_y == last_y ) :
        return 1
    elif( first_z == last_z ) :
        return 2
    else :
        print ("Error in determine_component_to_remove")
        if ( twodimensions==1 ):
            print ("It seems that your plane is inclined. You cannot project it into (x,y) plane.")
            print ("If you want a LATA file, please re-execute the conversion command without -p option.")
        sys.exit( 255 )
    return -1

# Write the coordinates of each element
def write_coordinates(Nelem,LataFile,Probe,dimension):
    GridCoordinates = LataFile+'.'+Probe+'.0.0'
    f = open(GridCoordinates,'w')
    idx_to_remove = -1
    if (dimension==2) :
        idx_to_remove = determine_component_to_remove( Nelem )
    for i in range(5,Nelem+5):
        values = lines[ i ].split(' ')
        for idx in range(0, 3) :
            if idx != idx_to_remove :
                f.write( str(values[ idx ])+str(" " ) )
        if dimension==2 :
            f.write("\n")
    f.close()
    return GridCoordinates

# Save the field at a given time
def save_field(LataFile,Field,Probe):
    current_time = lines[current_line-4]
    Time = float(current_time.split(' ')[1])
    FieldFile = LataFile+'.'+Field+'.'+Probe+'.'+'%12.10f' % Time
    f = open(FieldFile,'w')
    for i in range(current_line,current_line+Nelem):
        t = ' '.join(lines[i].split(' ')[1:])
        f.write(t) #+'\n')
    f.close()
    print(("Writing the %s field at time=%12.10f" %(Field,Time)))
    return current_time, FieldFile

# Add the field being post-processed at the current time to the main lata file
# in order to be loaded when the main lata is opened using visit
def add_line_to_lata(post_processed_Fields):
    post_processed_Fields += current_time+'CHAMP '+Field+' '+FieldFile+' geometrie='+Probe+' size='+str(Nelem)+' composantes='+comp+' localisation=SOM nature='+nature+' format=ASCII,F_MARKERS_NO\n'
    return post_processed_Fields


if __name__=="__main__":
    parser = OptionParser()
    parser.set_usage("Convert a .plan file to LATA format.\n  python Plan_to_lata.py PlanFile.plan [LataFile.lata] [-l] [-p]\n or\n  python Plan_to_lata.py PlanFile [LataFile] [-l] [-p]")
    parser.add_option("-l", "--lasttime", action="store_true", dest="lasttime", default=False,
                        help="Store only the last time. By default all times are stored.")
    parser.add_option("-p", "--projection", action="store_true", dest="projection", default=False,
                        help="For 3D computations, get a 2D representation (x,y) of the generated lata. By default a .plan issued from a 3D computation will be represented in 3D. For inclined plane, do not use this option!")
    (opts, args) = parser.parse_args()

    if len(args) == 0:
        parser.print_help()
        print ("\nPlease specify a .plan file!")
        exit(1)

    InputFileName = sys.argv[1]
    if InputFileName.endswith('.plan'):
        TestCase = os.path.splitext(os.path.basename(InputFileName))[0]
        PlanFile = InputFileName
    else:
        TestCase = InputFileName
        PlanFile = TestCase+'.plan'
    TestCase2 = TestCase

    if len(args) == 2:
        TestCase = sys.argv[2]
        if TestCase.endswith('.lata'):
            TestCase = os.path.splitext(os.path.basename(TestCase))[0]

    LataFile = TestCase+'.lata'
    if not (os.path.isfile(PlanFile)):
        parser.print_help()
        print(("\nError: %s file does not exist!" %(PlanFile)))
        exit(1)
    parser.print_usage()

    # In the case of 3D simulations, if twodimensions = 1 then we will project the
    # plane to (x,y) plane by removing the non-evolving component
    twodimensions = 0
    if opts.projection:
        twodimensions = 1

    # Get the different parameters of the plane :
    DataFile,Probe,lines,Nx,Ny,Nelem,Ncells,Field,comp,nature,dimension = get_parameters(PlanFile)
    NumberOfLines = sum(1 for line in lines )
    # Parfois FIN dans un fichier .plan:
    if (lines[len(lines)-1].startswith("FIN")):
       NumberOfLines = NumberOfLines - 1

    # Grid
    GridCoordinates = write_coordinates(Nelem,LataFile,Probe,dimension)
    GridConnections = save_connections(LataFile,Probe)
    print(("%s grid has been correctly generated" %(Probe)))

    if opts.lasttime:
        current_line = NumberOfLines-(2*Nelem)
    else:
        current_line= 2*Nelem + Ncells + 15

    post_processed_Fields=''
    while current_line < NumberOfLines:
        current_time, FieldFile = save_field(LataFile,Field,Probe)
        post_processed_Fields = add_line_to_lata(post_processed_Fields)
        current_line += 2*Nelem+4

    # Write the main lata file
    write_lata_file(LataFile,DataFile,Probe,GridCoordinates,GridConnections,Nelem,Ncells,post_processed_Fields,dimension)
    print(("\n %s has been successfuly converted to %s" %(PlanFile,LataFile)))
    print(("You can open it with: \nvisit -o %s" %(LataFile)))
