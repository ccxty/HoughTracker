#
#
#
noise=66
multi=1
n=500
for pt in 50 55 60 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135
do
    time ./bin/HoughFinder all -pt $pt -particle e- -noise $noise -multi $multi
    mv ./data/e-/trackdata_Pt${pt}_noise${noise}_multi${multi}.root ./data/e-/track${n}x${n}_Pt${pt}_noise${noise}_multi${multi}.root
done
