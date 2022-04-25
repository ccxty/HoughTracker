import os
import subprocess
particles = ["e-", "mu-", "pi-"]
#excutable = "./bin/HoughTracker"
excutable = "./bin/LocalTracker"

for particle in particles:
    os.system("python gen_config.py {}".format(particle))
    ls = subprocess.Popen(('ls', '-l'), stdout=subprocess.PIPE)
    grep = subprocess.Popen(('grep', "config_{}".format(
        particle)), stdin=ls.stdout, stdout=subprocess.PIPE)
    wc = subprocess.Popen(('wc', '-l'), stdin=grep.stdout,
                          stdout=subprocess.PIPE)
    n = int(wc.communicate()[0])

    for index in range(1, n + 1):
        run = "run_{}_{}.sh".format(particle, index)
        submit = "submit_{}_{}.cmd".format(particle, index)
        config = "./condor/config_{}_{}.json".format(particle, index)
        log = "log/log_{}_{}_test".format(particle, index)
        err = "err/err_{}_{}_test".format(particle, index)
        out = "out/out_{}_{}_test".format(particle, index)
        os.system("cp run.sh {}".format(run))
        os.system("cp submit.cmd {}".format(submit))
        os.system("chmod +x {}".format(run))
        os.system("sed -i '2c {} json -c {}' {}".format(excutable, config, run))
        os.system("sed -i '5c Executable           = {}' {}".format(run, submit))
        os.system("sed -i '6c Output               = {}' {}".format(out, submit))
        os.system("sed -i '7c Error                = {}' {}".format(err, submit))
        os.system("sed -i '8c Log                  = {}' {}".format(log, submit))
        os.system("condor_submit {}".format(submit))
