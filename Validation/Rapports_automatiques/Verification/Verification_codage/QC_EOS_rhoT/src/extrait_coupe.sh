extrait_coupe Cas PRESSION
extrait_coupe Cas VITESSE

cat Cas_PRESSION.coupe | awk '{print $1" "$2}' > pression.dat
cat Cas_VITESSE.coupe | awk '{print $1" "$2}' > u1.dat
cat Cas_VITESSE.coupe | awk '{print $1" "$3}' > u2.dat

rm -f *.coupe
