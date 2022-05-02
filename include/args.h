#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "clipp.h"
#include "json.hpp"
#ifndef __ARGS_PARSE_CXX_INCLUDE__
#define __ARGS_PARSE_CXX_INCLUDE__

enum class ExecMode { all, single, help, json };

NLOHMANN_JSON_SERIALIZE_ENUM(ExecMode, {
                                           {ExecMode::all, "all"},
                                           {ExecMode::single, "single"},
                                           {ExecMode::help, "help"},
                                           {ExecMode::json, "json"},
                                       });

struct Args {
    std::string data_file;
    std::string output_file;
    double pt;
    int n_noise;
    int n_track;
    std::string particle;
    ExecMode mode = ExecMode::help;
    std::string deg = "90";
};

void args_out_json(Args &args) {
    nlohmann::json args_json;
    args_json["pt"] = args.pt;
    args_json["n_noise"] = args.n_noise;
    args_json["n_track"] = args.n_track;
    args_json["particle"] = args.particle;
    args_json["mode"] = args.mode;
    args_json["data_file"] = args.data_file;
    args_json["output_file"] = args.output_file;
    args_json["deg"] = args.deg;
    std::ofstream out("args_exec.json");
    out << std::setw(4) << args_json << std::endl;
}

void args_parse_json(Args &args, std::string &json_file) {
    if (json_file.empty()) {
        json_file = "args.json";
    }
    std::ifstream in(json_file);
    nlohmann::json args_json;
    in >> args_json;
    args.pt = args_json["pt"];
    args.n_noise = args_json["n_noise"];
    args.n_track = args_json["n_track"];
    args.particle = args_json["particle"];
    args.data_file = args_json["data_file"];
    args.output_file = args_json["output_file"];
    args.mode = args_json["mode"];
    args.deg = args_json["deg"];
}

void args_parse(int argc, char **argv, const std::string &program_name,
                Args &args) {
    using clipp::command;
    using clipp::option;
    using clipp::required;
    using clipp::usage_lines;
    using clipp::value;
    using std::string;
    string file_json;
    string pt_str;
    string n_noise_str;
    string n_track_str;
    auto pt_arg = required("-pt") & value("pt", pt_str);
    auto noise_arg = required("-noise") & value("noise", n_noise_str);
    auto particle_arg =
        required("-particle") & value("particle", args.particle);
    auto n_track_arg = required("-multi") & value("num-tracks", n_track_str);
    auto deg_arg = option("-deg") & value("deg", args.deg);
    auto all_mode =
        (command("all").set(args.mode, ExecMode::all),
         pt_arg % "Pt of the data file", noise_arg % "number of noise points",
         particle_arg % "the particle stored in the data file",
         n_track_arg % "number of tracks in single event",
         deg_arg % "degree of the momentum, default is 90");
    auto single_mode =
        (command("single").set(args.mode, ExecMode::single),
         pt_arg % "Pt of the data file", noise_arg % "number of noise points",
         particle_arg % "the particle stored in the data file",
         n_track_arg % "number of tracks in single event",
         deg_arg % "degree of the momentum, default is 90");
    auto json_mode =
        (command("json").set(args.mode, ExecMode::json),
         option("-c", "--config") &
             value("config_file", file_json) % "json file with arguments");
    auto help_mode = (command("help").set(args.mode, ExecMode::help));
    auto show_version = option("-v", "--version")
                            .call([] { std::cout << "version 1.0\n\n"; })
                            .doc("show version");
    auto set_file_path =
        (option("-f", "--file") &
         value("file", args.data_file) % "set the path to the data file");
    auto set_out_path =
        (option("-o", "--output") &
         value("output", args.output_file) % "set the path to the output file");
    auto cli = ((all_mode & set_file_path & set_out_path) |
                (single_mode & set_file_path & set_out_path) | json_mode |
                help_mode | show_version);
    if (parse(argc, argv, cli)) {
        switch (args.mode) {
            case ExecMode::all:
                std::cout << "all mode" << std::endl;
                args.pt = atof(pt_str.c_str());
                args.n_noise = atoi(n_noise_str.c_str());
                args.n_track = atoi(n_track_str.c_str());
                break;
            case ExecMode::single:
                std::cout << "single mode" << std::endl;
                args.pt = atof(pt_str.c_str());
                args.n_noise = atoi(n_noise_str.c_str());
                args.n_track = atoi(n_track_str.c_str());
                break;
            case ExecMode::json:
                std::cout << "json mode" << std::endl;
                args_parse_json(args, file_json);
                break;
            case ExecMode::help:
                std::cout << make_man_page(cli, program_name);
                exit(0);
        }
        if (args.data_file.empty()) {
            args.data_file = "./root_data_source/";
            args.data_file += args.particle;
            args.data_file += "/posPt";
            args.data_file += std::to_string(args.pt);
            args.data_file += "_deg";
            args.data_file += args.deg;
            args.data_file += ".root";
        }
        if (args.output_file.empty()) {
            args.output_file = "./data/" + args.particle + "/trackdata_Pt" +
                               pt_str + "_noise" + n_noise_str + "_multi" +
                               n_track_str + "_deg" + args.deg + ".root";
        }
    } else {
        std::cout << usage_lines(cli, program_name) << '\n';
        exit(1);
    }
}

#endif