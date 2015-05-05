# Script Python
from string import *
#import string
def get_d(l):
	d=split(l,'\n')[0]
	i=0
	l=len(d)
	while ((i<l) and(d[i]==' ')):
		i=i+1
		pass
	d=d[i:]
	return d
import cPickle

def ecrit_dico(dico):
	#print "Ecriture du dico.pkl"
        f = open("dico.pkl", "wb")
	cPickle.dump(dico, f)
	f.close()
	f=open('dico.py','w')
	f.write("from Couv import lire_dico_pkl \ndico=lire_dico_pkl()\n")
	f.close()
	return
	f=open('dico.py','w')
        f.write("dico="+str(dico))
        f.close()
        pass
def lire_dico_pkl():
    #print "debut lecture dico.pkl"
    f = open("dico.pkl", "rb")
    toto = cPickle.load(f)
    f.close()
    #print "fin lecture"
    return toto
def lire_dico():
    from dico import dico
    return dico
def create_dico():
	dico={}
	c=open('list_methodes')
	i=0
	l=1
	while(l):
		i=i+1
		l=c.readline()
		d=get_d(l)
		dico[d]=[]
		#print i,d,dico[d]
		pass
	ecrit_dico(dico)
	
  	pass
def traite_file(file,dico):
	c=open(file)
	l=1
	while(l):
		l=c.readline()
		d=get_d(l)
		try:
			dico[d].append(file)
			#print dico[d]	
		except:
			print "pb avec ",d
			pass
		pass
  	pass	
def traite(file):
	dico=lire_dico()
	

	traite_file(file,dico)
	ecrit_dico(dico)
	pass
def traite_tout(list):
	dico=lire_dico()
	
	for file in list:
		traite_file(file,dico)
		pass
	ecrit_dico(dico)
	pass
def util(m=0):
	dico=lire_dico()
	for key in dico.keys():
		if (len(dico[key])==0):
			if (m==0):
				print "",key
				pass
			pass
		elif(m!=0):
			print key,dico[key]
			pass
def test2():
	dico=lire_dico()
	for key in dico.keys():
		print key,dico[key]
		pass
	pass
	 
def test(mot):
	dico=lire_dico()
	print mot,dico[mot]
	for cas in dico[mot]:
		print cas," CAS_liste"
	pass


def nombre_de_cas_par_classe():
	dico=lire_dico()
	
	dico3={}
	i=0
	invtot=1./len(dico.keys())*1.
	for mot in dico.keys():
		mot2=mot.split("::")
		# print i*invtot
		i+=1
		if (len(mot2)>1):
			mot_ref=mot2[0]
			mot_ref=mot_ref.replace("non-virtual thunk to ","")
			if not (mot_ref in dico3.keys()):
				m=[]
			
			else:
				m=dico3[mot_ref]
				pass
			nn=[]
			for mm in dico[mot]:
				if mm not in m : nn.append(mm)
			m.extend(nn)
			# m.extend(dico[mot])
			# m+=(dico[mot])
			
			dico3[mot_ref]=m
			
			pass
		pass
	#print "Traitement_particulier_NS_canal",dico3["Traitement_particulier_NS_canal"]
	for mot in dico3.keys():
		# if (len(dico3[mot])==0):
		par=0
		for m in dico3[mot]:
			if m[0:4]=="PAR_": par+=1
	       	print mot, len(dico3[mot])-par,par
			
		pass
	pass

