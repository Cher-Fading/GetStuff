if [ "$3" == "true" ]; then
    SUFFIX=_pnfs
fi
#cd /atlasgpfs01/usatlas/data/cher97/$1$2_Counts$SUFFIX
#ls *.txt > ~/GetStuff/$1_countsdone$SUFFIX.txt

cd /usatlas/scratch/cher97/$1$2_smalls$SUFFIX
ls $5_*.root > ~/GetStuff/$1$2_donesmalls$SUFFIX.txt

cd ~/GetStuff
echo 'listing done'

if [ "$2" == "PbPb" ]; then
    PbPb=true
fi

if [ "$2" == "pp" ]; then
    PbPb=false
fi

root -b -q -l 'count_missing_smalls.cpp("'$1'","'$5'",'$PbPb','$3','$4')'
