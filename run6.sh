make ./bin/HoughFinder_multi
for pt in 65 75 85 95 110 125 135
do
    for noise in 0 3 6 9 12 15 18 21
    do
        for multi in 2 3 4
        do
            ./bin/HoughFinder_multi $pt $noise all_multi $multi
        done
    done 
done