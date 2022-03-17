for pt in 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135
do
    root -l -q graph1.C
    pt_new=`expr $pt + 5`
    echo $pt_new
    sed -i s/${pt}/${pt_new}/g graph1.C
done
sed -i 's/140./65./g' graph1.C