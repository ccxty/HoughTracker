i=0
for pt in 50 55 60 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135
do
    for noise in 0 6 12 18 24 30 39 48 57 66
    do
        for multi in 1 3 5
        do
            i=$(expr $i + 1)
            cp run.sh runem$i.sh
            cp submit.cmd submitem$i.cmd
            sed -i '2c ./bin/HoughFinder_multi all -pt '"$pt"' -noise '"$noise"' -particle e- -multi '"$multi" runem$i.sh
            sed -i '6c Output               = out/outem'"$i"'_test' submitem$i.cmd
            sed -i '7c Error                = err/errem'"$i"'_test' submitem$i.cmd
            sed -i '8c Log                  = log/logem'"$i"'_test' submitem$i.cmd

            sed -i '5c Executable           = runem'"$i"'.sh' submitem$i.cmd
            condor_submit submitem$i.cmd
        done
    done 
done
i=0
for pt in 50 55 60 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135
do
    for noise in 0 6 12 18 24 30 39 48 57 66
    do
        for multi in 1 3 5
        do
            i=$(expr $i + 1)
            cp run.sh runep$i.sh
            cp submit.cmd submitep$i.cmd
            sed -i '2c ./bin/HoughFinder_multi all -pt '"$pt"' -noise '"$noise"' -particle e+ -multi '"$multi" runep$i.sh
            sed -i '6c Output               = out/outep'"$i"'_test' submitep$i.cmd
            sed -i '7c Error                = err/errep'"$i"'_test' submitep$i.cmd
            sed -i '8c Log                  = log/logep'"$i"'_test' submitep$i.cmd

            sed -i '5c Executable           = runep'"$i"'.sh' submitep$i.cmd
            condor_submit submitep$i.cmd
        done
    done 
done
