#from gentriopy import *

from triou import *

from test_cas import *

import os
import sys


def trouve_classe_type(type,ll):
	for cl in ll:
		if (isinstance(cl,type)):
			return cl
		pass
	return None

def get_eq_turb(ll):
	
	for cl in ll:
		if isinstance(cl,Pb_base):
			try:
				# on recupere le premier attribut
				eq1=getattr(cl,cl.__init__xattributes__[0].name)
			except:
				eq1=None
				pass
			
			if hasattr(eq1,"modele_turbulence"):
				return eq1
			pass
		pass
	
	return None
def get_pb_fluide(cl,ll):
	if cl.pb_1:
		pb_1=trouve_class_list(cl.pb_1,ll)
		if (not isinstance(pb_1,pb_conduction)):
			return pb_1
		pass
	pass
	if cl.pb_2:
		pb_2=trouve_class_list(cl.pb_2,ll)
		if (not isinstance(pb_2,pb_conduction)):
			return pb_2
		pass
	pass
	if cl.pb_3:
		pb_3=trouve_class_list(cl.pb_3,ll)
		if (not isinstance(pb_3,pb_conduction)):
			return pb_3
		pass
	pass
	if cl.pb_4:
		pb_4=trouve_class_list(cl.pb_4,ll)
		if (not isinstance(pb_4,pb_conduction)):
			return pb_4
		pass
	pass
	return pb_1

def get_fluide(ll):
	for cl in ll:
		if isinstance(cl,probleme_couple):
			pb=get_pb_fluide(cl,ll)
			print pb
			if (not isinstance(pb,probleme_ft_disc_gen)):
				if pb.milieu:
					fluide=trouve_class_list(pb.milieu,ll)
					return fluide
				pass
			else:
				if pb.mm:
					fluide=trouve_class_list(pb.mm,ll)
					return fluide
				pass
			pass
		elif isinstance(cl,Pb_base):
			if (not isinstance(cl,probleme_ft_disc_gen)):
				if cl.milieu:
					fluide=trouve_class_list(cl.milieu,ll)
					return fluide
				pass
			else:
				if cl.mm:
					fluide=trouve_class_list(cl.mm,ll)
					return fluide
				pass
			pass
		pass
	
	return None
def analyse_cas(file):
	res=""
	ll=read_file_data(file)
	pbc=trouve_classe_type(probleme_couple,ll)
	if pbc and trouve_classe_type(pb_conduction,ll):
		res=" Pb_Couple"
		pass
	else:
		res=" Pb_Simple"
		pass
	fluide=get_fluide(ll)
	if (isinstance(fluide,fluide_quasi_compressible)):
		res+=" + QC"
		pass
	elif (isinstance(fluide,fluide_incompressible)):
		res+=" + Incompressible"
		pass
	elif (isinstance(fluide,fluide_diphasique)):
		res+=" + Diphasique"
		pass
	elif (isinstance(fluide,solide)):
		res+=" + Solide"
		pass
	else:
		res+=" + Autre fluide"
		pass
	eq_turb=get_eq_turb(ll)
	if eq_turb:
		res+=" + turb"
		mod_turb=eq_turb.modele_turbulence
		if not isinstance(mod_turb,mod_turb_hyd_ss_maille):
			res+=" K_eps"
			pass
		pass
	elif (isinstance(fluide,solide)):
		res+=""
		pass
	else:
		res+=" + laminaire"
		pass
	if pbc:
		if (isinstance(pbc,pb_couple_rayo_semi_transp)):
			res+=" + rayo_semi_transp"
		elif (isinstance(pbc,probleme_couple_rayonnement)):
			res+=" + rayo_transparent"
			pass
		pass
	if trouve_classe_type(constituant,ll):
		res+=" + constituant"
		pass
	if trouve_classe_type(vdf,ll):
		res+=" + VDF"
	elif trouve_classe_type(vef,ll):
		res+=" + VEF"
	else:
		res+= " + unknown discr"
		pass
	return res
def test_all(list,s):
	dico={}
	nb=str(len(list))+" "
	compt=0
	for file in list:
		#cmd="cp $TRUST_ROOT/tests/*/"+file+"/"+file+".data ."
		#cmd2="chmod +w "+file+".data"
		#os.system(cmd)
		#os.system(cmd2)
		
		compt+=1
		print "cas ",compt," sur ",nb
		try: 
			res=analyse_cas(file+'.data')
			if dico.has_key(res):
				dico[res].append(file)
			else:
				dico[res]=[file]
				pass
			pass
		except:
			print "cas rate " ,file
			pass
		#print dico
		pass
	return dico

if __name__ == '__main__':
	list=[]
	for i in range(1,len(sys.argv)):
		list.append(sys.argv[i])
		pass
	print "nb_cas",len(list)
	print list
	s=open("res","w")
	list2=test_all(list,s)
	
	for cas in list2.keys():
		res=list2[cas]
		chaine=cas + " : "+str(len(res))+ " : "+str(res)
		print chaine
		s.write(chaine+'\n')
		pass
	s.close()
	for cas in list2.keys():
		from string import split
		print split(cas,'+')
	pass
