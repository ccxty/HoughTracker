for particle in e-
do
    python gen_config.py $particle
    for index in {1..541}
    do
        cp run.sh run${particle}${index}.sh
        chmod 755 run${particle}${index}.sh
        cp submit.cmd submit${particle}${index}.cmd
        sed -i '2c ./bin/Houghtracker json config_'${particle}${index}'.json' run${particle}${index}.sh
        sed -i '6c Output               = out/out'${particle}${index}'_test' submit${particle}${index}.cmd
        sed -i '7c Error                = err/err'${particle}${index}'_test' submit${particle}${index}.cmd
        sed -i '8c Log                  = log/log'${particle}${index}'_test' submit${particle}${index}.cmd
        sed -i '5c Executable           = run'${particle}${index}'.sh' submit${particle}${index}.cmd
        condor_submit submit${particle}${index}.cmd
    done
done