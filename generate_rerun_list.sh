if [ "$2" == "true" ]; then
    SUFFIX=_pnfs
fi
for ptLim in 50; do
    for trkLim in 0.5; do
	echo ~/GetStuff/$1_done$3$SUFFIX.txt
        ls /usatlas/scratch/cher97/$1_$3$SUFFIX/*_$ptLim.00_$trkLim.root >~/GetStuff/$1_done$3$SUFFIX.txt
        root -q 'missing_rerun.cpp("'$1'",'$2','$ptLim','$trkLim',"'$3'")'
    done
done
