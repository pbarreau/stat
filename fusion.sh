#!/usr/bin/sh

if [ -e nouveau_loto.zip ] 
then 
 rm nouveau_loto.zip
 rm nouveau_loto.csv
fi
wget https://media.fdj.fr/generated/game/loto/nouveau_loto.zip --no-check-certificate
unzip -o nouveau_loto.zip


if [ -e nouveau_superloto.zip ] 
then 
 rm nouveau_superloto.zip
 rm nouveau_superloto.csv
fi
wget https://media.fdj.fr/generated/game/loto/nouveau_superloto.zip --no-check-certificate
unzip -o nouveau_superloto.zip

sed -e "1,1d" nouveau_loto.csv > tmp_tir
sed -e "1,1d" nouveau_superloto.csv >> tmp_tir

head -n1 nouveau_superloto.csv > nouveau_loto.csv
sort -t ";" -r -n -k3.9 -k3.4 -k3 tmp_tir >> nouveau_loto.csv

#rm tmp_tir

