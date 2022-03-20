for pt in 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135
do
    for noise in 3 6 9 12 15 18 21
    do
        ./bin/HoughFinder_noise $pt $noise all
    done 
done