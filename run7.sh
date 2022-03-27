make bin/HoughFinder_multi
for pt in 50 55 60 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135
do
    for noise in 0 6 12 18 24 30 39 48 57 66
    do
        for multi in 1 3 5 7
        do
            time ./bin/HoughFinder_multi e+ $pt $noise all_multi $multi
        done
    done 
done
