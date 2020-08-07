if [ "$2" == "true" ]; then
    SUFFIX=_pnfs
fi
for ptLim in 50 100; do
    for trkLim in 0.5 1.5 2.0 4.0; do
        ls /usatlas/scratch/cher97/%s_IPEval$SUFFIX/*_$ptLim.00_$trkLim.root >~/GetStuff/$1_$ptLim.00_$trkLim\ _done$SUFFIX.txt
        root -q 'missing_rerun.cpp("WorkingDefaultpp",'$2',ptLim,trkLim)'
    done
done
