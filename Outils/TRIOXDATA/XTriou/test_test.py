
from triou import read_file_data

from test_cas import test_cas

import os
import sys


def test_all(list,s,fast):
	list2=[]
	nb=str(len(list))+" "
	compt=0
	for file in list:
		cmd="cp -H $TRUST_TESTS/*/"+file+"/*.data . ; [ $? != 0 ] && cp $TRUST_TESTS/*/*/"+file+".data . "
		cmd2="cp -H $TRUST_TESTS/*/"+file+"/*.geo $TRUST_TESTS/*/*/"+file+"/*.geo . 2>/dev/null;chmod +w *"
		os.system(cmd)
		os.system(cmd2)
		os.system("make_PAR.data "+ file +" 2 no_exec >>log 2>/dev/null") 
		compt+=1
		list_cas=[ file , "DEC_"+file ]
		if fast:
			#list_cas=[ file ]
			pass
		for file0 in list_cas:
			
			cmd="rm -f "+file0+".data "+"test*_"+file0+".data p_"+file0+".* *.geo" 
			print cmd
			
			try:
				if (fast):
					listclass1=read_file_data(file0+".data")
				else:
					test_cas(file0)
					pass
				chaine=str(compt)+"/"+nb+file0+" OK"
				os.system(cmd)
			except KeyError,x:
				print "KeyError",x
				chaine=file0+" rate"
				list2.append(chaine)
			except:	
				try:
					# plus de semi ok....
					f=open("test3_"+file0+".data")
					chaine=file0+" semi ok"
				except:
					chaine=file0+" rate"
					pass
				list2.append(chaine)
				pass
			print chaine
			s.write(chaine+'\n')
			s.flush()
			pass
		pass
	return list2

if __name__ == '__main__':
	fast=0
	list=[]
         
        if (len(sys.argv)>1):
	  if (sys.argv[1]=="-fast"):
		fast=1
	  else:
		list=[sys.argv[1]]
		pass
	for i in range(2,len(sys.argv)):
		list.append(sys.argv[i])
		pass
	print "nb_cas",len(list)
	print list
	s=open("res","w")
	list2=test_all(list,s,fast)
	s.close()
	for cas in list2:
		print cas #+ " rate"
		#test_cas(cas)
		#s.write(cas+'\n')
		pass
	
	pass
