import json
import sys
pt_array = ["50", "55", "60", "65", "70", "75", "80", "85", "90", "95",
            "100", "105", "110", "115", "120", "125", "130", "135"]
n_noise_array = ["0", "6", "12", "18", "24", "30", "39", "48", "57", "66"]
n_track_array = ["1", "3", "5"]
particle = sys.argv[1]

config_index = 0
for pt in pt_array:
    for n_noise in n_noise_array:
        for n_track in n_track_array:
            config_index += 1
            data_file = "./root_data_source/{}/pos{}.root".format(particle, pt)
            output_file = "./data/{}/trackdata_Pt{}_noise{}_multi{}.root".format(
                particle, pt, n_noise, n_track)
            config = {
                "data_file": data_file,
                "mode": "all",
                "n_noise": int(n_noise),
                "n_track": int(n_track),
                "output_file": output_file,
                "particle": particle,
                "pt": float(pt),
            }
            with open("config_" + particle+str(config_index) + ".json", "w") as f:
                json.dump(config, f)
