make HoughFinder_noise
for pt in 65 75 85 95 105 115 125 135
do 
    for noise in 3 6 9 12 15 18 21
    do
        for eventID in {0..999}
        do
            ./HoughFinder_noise $pt noise_given  $noise $eventID
        done
    done
done