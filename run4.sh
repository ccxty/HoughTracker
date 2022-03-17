for pt in 65 75 85 95 105 115 125 135
do
    root -l -q graph3.C
    pt_new=`expr $pt + 10`
    echo $pt_new
    sed -i s/${pt}/${pt_new}/g graph3.C
done
sed -i 's/145./65./g' graph3.C