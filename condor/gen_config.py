import json
import sys
pt_array = ["50", "55", "60", "65", "70", "75", "80", "85", "90", "95",
            "100", "105", "110", "115", "120", "125", "130", "135"]
n_noise_array = ["0", "6", "12", "18", "24", "30",
                 "39", "48", "57", "66", "75", "85", "100"]
n_track_array = ["1", "3", "5"]
deg_str_array = ["90", "84.2608", "78.463", "72.5424",
                 "66.4218", "60", "53.1301", "45.573", "36.8699"]
particle = sys.argv[1]

config_index = 0
for pt in pt_array:
    for n_noise in n_noise_array:
        for n_track in n_track_array:
            for deg_str in deg_str_array:
                config_index += 1
                data_file = "./root_data_source/{}/posPt{}_deg{}.root".format(
                    particle, pt, deg_str)
                output_file = "./data/{}/trackdata_Pt{}_noise{}_multi{}_deg{}.root".format(
                    particle, pt, n_noise, n_track, deg_str)
                config = {
                    "data_file": data_file,
                    "mode": "all",
                    "n_noise": int(n_noise),
                    "n_track": int(n_track),
                    "output_file": output_file,
                    "particle": particle,
                    "pt": float(pt),
                    "deg": deg_str,
                }
                with open("config_{}_{}.json".format(particle, config_index), "w") as f:
                    json.dump(config, f, indent=4)
