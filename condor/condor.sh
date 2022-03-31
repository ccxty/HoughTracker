i=0
for pt in 50 55 60 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135
do
    for noise in 0 6 12 18 24 30 39 48 57 66
    do
        for multi in 1 3 5
        do
            i=$(expr $i + 1)
            cp run.sh run$i.sh
            cp submit.cmd submit$i.cmd
            sed -i '2c ./bin/HoughFinder_multi -pt '"$pt"' -particle e- -noise '"$noise"' -multi '"$multi" run$i.sh
            sed -i '6c Output               = out/outep'"$i"'_test' submit$i.cmd
            sed -i '7c Error                = err/errep'"$i"'_test' submit$i.cmd
            sed -i '8c Log                  = log/logep'"$i"'_test' submit$i.cmd

            sed -i '5c Executable           = run'"$i"'.sh' submit$i.cmd
            condor_submit submit$i.cmd
        done
    done 
done