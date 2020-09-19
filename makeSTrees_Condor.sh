#!/bin/bash
#input="/atlasgpfs01/usatlas/data/cher97/mc16_5TeV.txt"
if [ "$4" == "true" ]; then
	SUFFIX=_pnfs
fi

if [ "$6" == "true" ]; then
input="../GetStuff/$2$3_donesmall$SUFFIX.txt"
else
input="../GetStuff/$2$SUFFIX.txt"
fi
#input="mc16_5TeV_short.txt"

mkdir -p /usatlas/scratch/cher97/$2$3_smalls$SUFFIX

#indexline=$1
linenumber=0
while IFS= read -r line; do
	if [ $1 -eq $linenumber ]; then
		if [ "$6" == "true" ]; then
filename=/usatlas/scratch/cher97/$2$3_small/$line
else
mkdir -p '/usatlas/scratch/cher97/tempin'$linenumber
xrdcp 'root://dcgftp.usatlas.bnl.gov:1096/'$line '/usatlas/scratch/cher97/tempin'$linenumber
filename=/usatlas/scratch/cher97/tempin$linenumber/$(ls /usatlas/scratch/cher97/tempin$linenumber)
fi		

		root -b -q -l 'makeSTree.C("'$5'","'$line'","'/usatlas/scratch/cher97/$2$3_smalls$SUFFIX'")'
		sleep 2
		rm -rf '/usatlas/scratch/cher97/tempin'$linenumber
	fi
	linenumber=$((linenumber + 1))
done <"$input"
