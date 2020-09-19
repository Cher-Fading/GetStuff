if [ "$3" == "true" ]; then
    SUFFIX=_pnfs
fi
cd /usatlas/scratch/cher97/$1$2_small$SUFFIX
ls *.root > ~/GetStuff/$1$2_donesmall$SUFFIX.txt
cd ~/GetStuff
echo 'listing done'

if [ "$2" == "PbPb" ]; then
    PbPb=true
fi

if [ "$2" == "pp" ]; then
    PbPb=false
fi

root -b -q -l 'count_missing_small.cpp("'$1'","'$5'",'$PbPb','$3','$4')'
