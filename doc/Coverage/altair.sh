#!/bin/bash
# See https://codev-tuleap.intra.cea.fr/plugins/mediawiki/wiki/trust/index.php?title=Super_Build
mac=altair.intra.cea.fr
root=/volatile/catA/pl254994/Super_Build
scp $mac:$root/build/tests/GCDA/TRUST_cpp.gcov.tgz 			cpp.gcov.tar.gz
scp $mac:$root/build/Coverage_baltik/list_methodes.gz			list_methodes.gz
scp $mac:$root/build/Coverage_baltik/dico.pkl.gz			dico.pkl.gz
scp $mac:$root/build/Coverage_baltik/list_methodes_non_appelees.gz	list_methodes_non_appelees.gz

