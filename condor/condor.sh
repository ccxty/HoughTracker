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
            cp run.sh runmm$i.sh
            cp submit.cmd submitmm$i.cmd
            sed -i '2c ./bin/HoughFinder all -pt '"$pt"' -noise '"$noise"' -particle mu- -multi '"$multi" runmm$i.sh
            sed -i '6c Output               = out/outmm'"$i"'_test' submitmm$i.cmd
            sed -i '7c Error                = err/errmm'"$i"'_test' submitmm$i.cmd
            sed -i '8c Log                  = log/logmm'"$i"'_test' submitmm$i.cmd

            sed -i '5c Executable           = runmm'"$i"'.sh' submitmm$i.cmd
            condor_submit submitmm$i.cmd
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
            cp run.sh runpm$i.sh
            cp submit.cmd submitpm$i.cmd
            sed -i '2c ./bin/HoughFinder all -pt '"$pt"' -noise '"$noise"' -particle pi- -multi '"$multi" runpm$i.sh
            sed -i '6c Output               = out/outpm'"$i"'_test' submitpm$i.cmd
            sed -i '7c Error                = err/errpm'"$i"'_test' submitpm$i.cmd
            sed -i '8c Log                  = log/logpm'"$i"'_test' submitpm$i.cmd

            sed -i '5c Executable           = runpm'"$i"'.sh' submitpm$i.cmd
            condor_submit submitpm$i.cmd
        done
    done 
done
