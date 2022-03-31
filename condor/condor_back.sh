mkdir err log out
for ((i=0; i<162; ++i))
do
    
    cp run_particle_gun.py runep_particle_gun$i.py
    cp run.sh runep$i.sh 
    cp submit.cmd submitep$i.cmd
    sed -i '6c Output               = out/outep'"$i"'_test' submitep$i.cmd
    sed -i '7c Error                = err/errep'"$i"'_test' submitep$i.cmd
    sed -i '8c Log                  = log/logep'"$i"'_test' submitep$i.cmd
    sed -i '11c sSvc.property("RndmSeed").set('"$RANDOM"')' runep_particle_gun$i.py
    sed -i '35c oSvc.property("OutputStream").set({"/Event/StcfMCEvent" : "singleEplus'"$i"'.root"})' runep_particle_gun$i.py
    sed -i '55c g4svc.property("RunMac").set("runep'"$i"'.mac")' runep_particle_gun$i.py
    sed -i '73c factory.property("RandomSeed").set('"$RANDOM"')' runep_particle_gun$i.py
    
 
    sed -i '5c Executable           = runep'"$i"'.sh' submitep$i.cmd
    sed -i '4c python runep_particle_gun'"$i"'.py' runep$i.sh
    
    condor_submit submitep$i.cmd
done

for ((i=0; i<162; ++i))
do
    
    cp run_particle_gun.py runem_particle_gun$i.py
    cp run.sh runem$i.sh 
    cp submit.cmd submitem$i.cmd
    sed -i '6c Output               = out/outem'"$i"'_test' submitem$i.cmd
    sed -i '7c Error                = err/errem'"$i"'_test' submitem$i.cmd
    sed -i '8c Log                  = log/logem'"$i"'_test' submitem$i.cmd
    sed -i '11c sSvc.property("RndmSeed").set('"$RANDOM"')' runem_particle_gun$i.py
    sed -i '35c oSvc.property("OutputStream").set({"/Event/StcfMCEvent" : "singleEminus'"$i"'.root"})' runem_particle_gun$i.py
    sed -i '55c g4svc.property("RunMac").set("runem'"$i"'.mac")' runem_particle_gun$i.py
    sed -i '73c factory.property("RandomSeed").set('"$RANDOM"')' runem_particle_gun$i.py
    
 
    sed -i '5c Executable           = runem'"$i"'.sh' submitem$i.cmd
    sed -i '4c python runem_particle_gun'"$i"'.py' runem$i.sh
    
    condor_submit submitem$i.cmd
done



