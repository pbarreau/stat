rem del D_ZIP\*.*
rem wget -S -P D_ZIP https://media.fdj.fr/generated/game/loto/loto2017.zip  --no-check-certificate
rem wget -S -P D_ZIP https://media.fdj.fr/generated/game/loto/nouveau_loto.zip  --no-check-certificate
rem wget -S -P D_ZIP https://media.fdj.fr/generated/game/loto/loto.zip  --no-check-certificate
rem wget -S -P D_ZIP https://media.fdj.fr/generated/game/loto/superloto2017.zip  --no-check-certificate
rem wget -S -P D_ZIP https://media.fdj.fr/generated/game/loto/nouveau_superloto.zip  --no-check-certificate
rem wget -S -P D_ZIP https://media.fdj.fr/generated/game/loto/superloto.zip  --no-check-certificate
rem wget -S -P D_ZIP https://media.fdj.fr/generated/game/loto/lotonoel2017.zip  --no-check-certificate

set var1=loto2017 nouveau_loto loto superloto2017 nouveau_superloto superloto lotonoel2017
 
rem for %%f in (loto2017 nouveau_loto loto superloto2017 nouveau_superloto superloto lotonoel2017) do (
for %%f in (%var1%) do (
@echo Traitement fichier %%f.zip
@echo "-------------------------------"
del D_ZIP\%%f.zip
del D_CSV\%%f.csv
wget -S -P D_ZIP https://media.fdj.fr/generated/game/loto/%%f.zip  --no-check-certificate
unzip -o -d D_CSV D_ZIP/%%f.zip
@echo 
)
