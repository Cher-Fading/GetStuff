#!/bin/bash
#input="/atlasgpfs01/usatlas/data/cher97/mc16_5TeV.txt"
if [ "$4" == "true" ]; then
	SUFFIX=_pnfs
fi
input="../GetStuff/$2$3_misssmall$SUFFIX.txt"
#input="mc16_5TeV_short.txt"

mkdir -p /atlasgpfs01/usatlas/data/cher97/$2$3_Counts$SUFFIX

#indexline=$1
linenumber=0
while IFS= read -r line; do
	if [ $1 -eq $linenumber ]; then
		#mkdir -p '/usatlas/scratch/cher97/tempin'$linenumber
		echo $line
		#xrdcp 'root://dcgftp.usatlas.bnl.gov:1096//pnfs/usatlas.bnl.gov/users/cher97/WorkingDefaultpppp_small/'$line '/usatlas/scratch/cher97/'$2$3'_small'$SUFFIX	
		cp '/pnfs/usatlas.bnl.gov/users/cher97/WorkingDefaultpppp_small/'$line '/usatlas/scratch/cher97/'$2$3'_small'$SUFFIX			
		#filename=/usatlas/scratch/cher97/tempin$linenumber/$(ls /usatlas/scratch/cher97/tempin$linenumber)
		#root -b -q -l 'CountJets.cpp("'$5'","'$filename'")'
		sleep 2
		#rm -rf '/usatlas/scratch/cher97/tempin'$linenumber
	fi
	linenumber=$((linenumber + 1))
done <"$input"
