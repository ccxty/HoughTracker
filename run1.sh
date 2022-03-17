make HoughFinder
for pt in 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135
do
    for eventID in {0..999}
    do
        ./HoughFinder $pt given $eventID
    done
done