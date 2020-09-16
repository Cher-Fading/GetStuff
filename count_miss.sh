if [ "$3" == "true" ]; then
    SUFFIX=_pnfs
fi
ls /atlasgpfs01/usatlas/data/cher97/$1$2_Counts$SUFFIX/*.txt >../GetStuff/$1_countsdone$SUFFIX.txt

if [ "$2" == "PbPb" ]; then
    PbPb = true
fi

if ["$2" == "pp" ]; then
    PbPb = false
fi

root -b -q -l 'count_missing_counts.cpp("'$1'","'$5'",'$PbPb','$3','$4')'
