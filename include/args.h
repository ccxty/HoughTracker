#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "clipp.h"
#include "json.hpp"
#ifndef __ARGS_PARSE_CXX_INCLUDE__
#define __ARGS_PARSE_CXX_INCLUDE__ 1

enum class ExecMode { all, single, help, json };

NLOHMANN_JSON_SERIALIZE_ENUM(ExecMode, {
                                           {ExecMode::all, "all"},
                                           {ExecMode::single, "single"},
                                           {ExecMode::help, "help"},
                                           {ExecMode::json, "json"},
                                       });

struct Args {
    std::string pt_str;
    std::string n_noise_str;
    std::string n_track_str;
    std::string particle;
    ExecMode mode = ExecMode::help;
};

void args_out_json(Args &args) {
    nlohmann::json args_json;
    args_json["pt"] = args.pt_str;
    args_json["n_noise"] = args.n_noise_str;
    args_json["n_track"] = args.n_track_str;
    args_json["particle"] = args.particle;
    args_json["mode"] = args.mode;
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
    args.pt_str = args_json["pt"];
    args.n_noise_str = args_json["n_noise"];
    args.n_track_str = args_json["n_track"];
    args.particle = args_json["particle"];
    args.mode = args_json.get<ExecMode>();
}

void args_parse(int argc, char **argv, const std::string &program_name,
                Args &args) {
    using clipp::command;
    using clipp::option;
    using clipp::required;
    using clipp::usage_lines;
    using clipp::value;
    std::string file_json;
    auto pt_arg = required("-pt") & value("pt", args.pt_str);
    auto noise_arg = required("-noise") & value("noise", args.n_noise_str);
    auto particle_arg =
        required("-particle") & value("particle", args.particle);
    auto n_track_arg =
        required("-multi") & value("num-tracks", args.n_track_str);
    auto all_mode =
        (command("all").set(args.mode, ExecMode::all),
         pt_arg % "Pt of the data file", noise_arg % "number of noise points",
         particle_arg % "the particle stored in the data file",
         n_track_arg % "number of tracks in single event");
    auto single_mode =
        (command("single").set(args.mode, ExecMode::single),
         pt_arg % "Pt of the data file", noise_arg % "number of noise points",
         particle_arg % "the particle stored in the data file",
         n_track_arg % "number of tracks in single event");
    auto json_mode =
        (command("json").set(args.mode, ExecMode::json),
         value("config_file", file_json) % "json file with arguments");
    auto cli = ((all_mode | single_mode | json_mode |
                 command("help").set(args.mode, ExecMode::help)),
                option("-v", "--version")
                    .call([] { std::cout << "version 1.0\n\n"; })
                    .doc("show version"));
    if (parse(argc, argv, cli)) {
        switch (args.mode) {
            case ExecMode::all:
                std::cout << "all mode" << std::endl;
                break;
            case ExecMode::single:
                std::cout << "single mode" << std::endl;
                break;
            case ExecMode::json:
                std::cout << "json mode" << std::endl;
                args_parse_json(args, file_json);
                break;
            case ExecMode::help:
                std::cout << make_man_page(cli, "HoughTracker");
                exit(0);
        }
    } else {
        std::cout << usage_lines(cli, program_name) << '\n';
        exit(1);
    }
}

#endif