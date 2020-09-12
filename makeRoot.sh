if [[ "$1" == *"$Working"* ]]; then
    if [ "$3" == "PbPb" ]; then
        GetLOCALGROUPDISK_PbPb.sh $1 $2
    fi
    if [ "$3" == "pp" ]; then
        GetLOCALGROUPDISK_pp.sh $1 $2
    fi
    if [ "$4" == "true"]; then
        root -b -q -l 'get_to_root_pnfs.cpp("'$2'")'
    fi
    if [ "$4" == "true"]; then
        root -b -q -l 'get_to_root.cpp("'$2'")'
    fi
else
    ls /atlasgpfs01/usatlas/data/cher97/flav_$2_Akt4HIJets/$3mc16/$3mc16JZ*.root >~/GetStuff/$2_root.txt
fi
