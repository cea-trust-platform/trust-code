import sys
import os

#
# This script is meant to be used if the logs of each processor have been written in a shared file using the hdf5 format
# and one wishes to transform this file into a readable one
#

def usage():
    print("Usage: python hdf5_to_ascii.py file_name nb_proc")
    print("file_name : name of the HDF5 file we want to convert")
    print("nb_proc   : how many processors have written their logs into file_name")
    
def open_file(FILE_NAME):
    f=open (FILE_NAME, "r")
    data=f.readlines()
    f.close()
    return data

def write_file(FILE_NAME, data):
    f = open(FILE_NAME, "w")
    f.write(data)
    f.close()


def clean(FILE_NAME, NEW_FILE_NAME):

    raw_data = open_file(FILE_NAME)

    # first stage of cleaning :
    # joining all the tiny strings to recreate the original text
    tmp_str = " ".join(raw_data)
    tmp_list = tmp_str.split('", "')
    hdf5_text = "".join(tmp_list)

    # second stage of cleaning:
    # removing the unreadable binary part at the end of the log of each process    
    separator = '\n============================================================================================================================================================================='
    journalStart = 0
    proc = 1
    while True:
        #searching the end of the log of the current proc
        end = "End of Journal logging"
        journalEnd = hdf5_text.find(end, journalStart)
        if journalEnd == -1:
            end = "TRUST has caused an error and will stop."
            journalEnd = hdf5_text.find(end, journalStart)
        if journalEnd == -1:
            print("[Error] This file doesn't seem to be a shared journal...")
            print("[Error] To transform any other HDF5 file into ascii format, use h5dump")
            exit()
            
        journalEnd += len(end)
        #searching the beginning of the log of the next proc
        begin = "[Proc %d] : Journal logging started" % proc
        journalStart = hdf5_text.find(begin, journalEnd)

        #if the next log doesn't exist, we have reached the end of the HDF file
        if journalStart == -1:
            hdf5_text = hdf5_text[:journalEnd] 
            break
        #otherwise, we remove everything in between the logs
        hdf5_text = hdf5_text[:journalEnd] + separator*2 + '\n           ' + hdf5_text[journalStart:]
        proc+=1

    write_file(NEW_FILE_NAME, hdf5_text)
    print("The file %s has been written" % NEW_FILE_NAME)
    return

if __name__ == "__main__":
    
    if len(sys.argv) == 2:
        usage()
        exit()

    file_name = sys.argv[1]
    nb_proc = int(sys.argv[2])
    tmp_file = file_name+".tmp"
    datasets_name = ""
    for p in range(nb_proc):
        datasets_name += "-d /log_%d " % p

    cmd = "h5dump %s -o %s -y -w 1000 %s" % (datasets_name, tmp_file, file_name)
    os.system(cmd)

    basename = file_name.split(".")[0]
    new_file_name = basename + ".ascii"
    clean(tmp_file, new_file_name)
    os.system("[ -f %s ] && rm %s -f" % (tmp_file, tmp_file))
