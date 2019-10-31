from triou import read_file_data,write_file_data,write_file_python,verify_all
import sys
def test_cas(file0):
    file=file0+".data"
    print("test_cas",file)
    listclass1=read_file_data(file)
    file1="test_"+file
    file2="test2_"+file
    file3="test3_"+file
    namepy=filepy="p_"+file0
    if (namepy.find("-")>=0):
        namepy=namepy.replace("-","moins")
        pass
    if (namepy.find(".")>=0):
        namepy=namepy.replace(".","dot")
        pass
    filepy=namepy+".py"
    filepy2=namepy+".py2"
    write_file_data(file1,listclass1)
    write_file_python(filepy,listclass1)
    verify_all(listclass1)


    cdm="from "+namepy+" import *"
    print("cmd ",cdm)
    exec(cdm)
    #print "ici",listclass
    write_file_data(file2,listclass)
    toto=read_file_data(file2)
    verify_all(toto)
    write_file_python(filepy2,toto)
    write_file_data(file3,toto)
    ti=open('lutil.py','w')
    ti.write("lutil="+str(lutil)+'\n')
    ti.close()
    s1=open(file1,'r')
    s2=open(file2,'r')
    s3=open(file3,'r')
    c1=s1.read()
    c2=s2.read()
    c3=s3.read()
    if (c1!=c2):
        msg=" cas "+file+" rate"
        raise Exception(msg)
    else:
        print("cas "+file+" Ok niveau 2")
        pass
    if (c1!=c3):
        msg=" cas "+file+" rate"
        raise Exception(msg)
    else:
        print("cas "+file+" Ok niveau 3")
        pass
    # raise Exception("expres")
    pass

if __name__ == '__main__':
    test_cas(sys.argv[1])
    pass
