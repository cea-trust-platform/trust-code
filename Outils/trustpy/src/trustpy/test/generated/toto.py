################################################################
# This file was generated automatically from :
# /export/home/adrien/Projets/TRUST/TRUST_LOCAL_bis/Outils/trustpy/src/trustpy/test/TRAD_2_adr_simple
# 24-07-28 at 09:56:22
################################################################

import sys
from typing_extensions import Annotated, Literal
from typing import ClassVar, List
from pydantic import BaseModel, Field
from trustpy.base import *

################################################################

# class Base_Common(object):
#     tutu = 0


class Objet_u(BaseModel, Base_common):
    __braces:int = 1
    __description: str = r""
    __synonyms: str = []

################################################################

class Interprete(Objet_u):
    __braces:int = 0
    __description: str = r"Basic class for interpreting a data file. Interpretors allow some operations to be carried out on objects."
    __synonyms: str = ['interprete']

################################################################

class Read_med(Interprete):
    __braces:int = 1
    __description: str = r"Keyword to read MED mesh files where \'domain\' corresponds to the domain name, \'file\' corresponds to the file (written in the MED format) containing the mesh named mesh_name. NL2 Note about naming boundaries: When reading \'file\', TRUST will detect boundaries between domains (Raccord) when the name of the boundary begins by type_raccord_. For example, a boundary named type_raccord_wall in \'file\' will be considered by TRUST as a boundary named \'wall\' between two domains. NL2 NB: To read several domains from a mesh issued from a MED file, use Read_Med to read the mesh then use Create_domain_from_sub_domain keyword. NL2 NB: If the MED file contains one or several subdomaine defined as a group of volumes, then Read_MED will read it and will create two files domain_name_ssz.geo and domain_name_ssz_par.geo defining the subdomaines for sequential and/or parallel calculations. These subdomaines will be read in sequential in the datafile by including (after Read_Med keyword) something like: NL2 Read_Med .... NL2 Read_file domain_name_ssz.geo ; NL2 During the parallel calculation, you will include something: NL2 Scatter { ... } NL2 Read_file domain_name_ssz_par.geo ;"
    __synonyms: str = ['lire_med']
    convertalltopoly: bool = Field(description=r"Option to convert mesh with mixed cells into polyhedral/polygonal cells", default=False)
    no_family_names_from_group_names: bool = Field(description=r"Awful option just to keep naked family names from MED file. Rarely used, to be removed very soon.", default=False)
    file: str = Field(description=r"File (written in the MED format, with extension \'.med\') containing the mesh", default="")
    mesh: str = Field(description=r"Name of the mesh in med file. If not specified, the first mesh will be read.", default="")
    exclude_groups: List[str] = Field(description=r"List of face groups to skip in the MED file.", default_factory=list)

################################################################

class Toto_base(Objet_u):
    __braces:int = 0
    __description: str = r"just so that it has _base at the end of the name ... this triggers type reading."
    __synonyms: str = ['toto_base']

################################################################

class Read_med_bidon(Toto_base):
    __braces:int = 1
    __description: str = r"Keyword to read MED mesh files where \'domain\' corresponds to the domain name, \'file\' corresponds to the file (written in the MED format) containing the mesh named mesh_name. NL2 Note about naming boundaries: When reading \'file\', TRUST will detect boundaries between domains (Raccord) when the name of the boundary begins by type_raccord_. For example, a boundary named type_raccord_wall in \'file\' will be considered by TRUST as a boundary named \'wall\' between two domains. NL2 NB: To read several domains from a mesh issued from a MED file, use Read_Med to read the mesh then use Create_domain_from_sub_domain keyword. NL2 NB: If the MED file contains one or several subdomaine defined as a group of volumes, then Read_MED will read it and will create two files domain_name_ssz.geo and domain_name_ssz_par.geo defining the subdomaines for sequential and/or parallel calculations. These subdomaines will be read in sequential in the datafile by including (after Read_Med keyword) something like: NL2 Read_Med .... NL2 Read_file domain_name_ssz.geo ; NL2 During the parallel calculation, you will include something: NL2 Scatter { ... } NL2 Read_file domain_name_ssz_par.geo ;"
    __synonyms: str = ['lire_med_bidon']
    convertalltopoly: bool = Field(description=r"Option to convert mesh with mixed cells into polyhedral/polygonal cells", default=False)
    no_family_names_from_group_names: bool = Field(description=r"Awful option just to keep naked family names from MED file. Rarely used, to be removed very soon.", default=False)
    file: str = Field(description=r"File (written in the MED format, with extension \'.med\') containing the mesh", default="")
    mesh: str = Field(description=r"Name of the mesh in med file. If not specified, the first mesh will be read.", default="")
    exclude_groups: List[str] = Field(description=r"List of face groups to skip in the MED file.", default_factory=list)

################################################################

class Field_base(Objet_u):
    __braces:int = 0
    __description: str = r"Basic class of fields."
    __synonyms: str = ['champ_base']

################################################################

class Champ_don_base(Field_base):
    __braces:int = -1
    __description: str = r"Basic class for data fields (not calculated), p.e. physics properties."
    __synonyms: str = ['champ_don_base']

################################################################

class Uniform_field(Champ_don_base):
    __braces:int = -1
    __description: str = r"Field that is constant in space and stationary."
    __synonyms: str = ['champ_uniforme']
    flag_bidon: Literal["flag_bidon"] = Field(description=r"Un flag bidon optionnel juste pour test", default="flag_bidon")
    val: List[float] = Field(description=r"Values of field components.", default_factory=list)
    flag_bidon2: Literal["flag_bidon2"] = Field(description=r"Un autre flag bidon optionnel juste pour test", default="flag_bidon2")
    val2: List[float] = Field(description=r"Values of field components.", default_factory=list)

################################################################

class Coucou_base(Objet_u):
    __braces:int = 1
    __description: str = r"not set"
    __synonyms: str = ['coucou_base']

################################################################

class Coucou(Coucou_base):
    __braces:int = -1
    __description: str = r"Basic class for medium (physics properties of medium)."
    __synonyms: str = ['coucou']
    attr_bidon: Uniform_field = Field(description=r"just to have a leaf type (unif field).", default_factory=lambda: eval("Uniform_field()"))
    gravite: Field_base = Field(description=r"Gravity field (optional).", default_factory=lambda: eval("Field_base()"))
    porosites_champ: Field_base = Field(description=r"The porosity is given at each element and the porosity at each face, Psi(face), is calculated by the average of the porosities of the two neighbour elements Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.", default_factory=lambda: eval("Field_base()"))

################################################################

class Nom(Objet_u):
    __braces:int = 0
    __description: str = r"Class to name the TRUST objects."
    __synonyms: str = ['nom']
    mot: Chaine = Field(description=r"Chain of characters.", default="foo", validate_default=True, max_length=3)

################################################################


if __name__ == "__main__":
    
    # type Chaine car validate_default=True
    a = Nom()
    print('a = Nom()                                ', a.mot, type(a.mot))

    # type Chaine car la validation est faite à l'instantiation
    a = Nom(mot="bar")    
    print('a = Nom(mot="bar")                       ', a.mot, type(a.mot))
    
    # pas de type Chaine car pas de validation sur setter
    a.mot = "str"
    print('a.mot = "str"                            ', a.mot, type(a.mot))

    # type Chaine car validation du modèle (penser à récupérer la sortie de validation)
    a = a.model_validate(a.model_dump())
    print('a = a.model_validate(a.model_dump())     ', a.mot, type(a.mot))

    # exemple erreur de validation 
    from pydantic import ValidationError
    print('-'*16)
    a.mot = "more than max_length"
    try: a.model_validate(a.model_dump())
    except ValidationError as e: print(e)

