from pyHough import *

particle = "e-"
deg = "90"
pt = 50
n_noise = 66
n_track = 1
mode = 0
# mode == 0: all events
# mode == 1: single event
# deg_str_array = ["90", "84.2608", "78.463", "72.5424",
#                  "66.4218", "60", "53.1301", "45.573", "36.8699"]

data_file = "./root_data_source/{}/posPt{}_deg{}.root".format(
    particle, pt, deg)

output_file = "./data/{}/trackdata_Pt{}_noise{}_multi{}_deg{}.root".format(
    particle, pt, n_noise, n_track, deg)


c_args = Args(
    data_file=data_file.encode(),
    output_file=output_file.encode(),
    pt=pt,
    n_noise=n_noise,
    n_track=n_track,
    particle=particle.encode(),
    mode=mode,
    deg=deg.encode(),
)

run(c_args)
