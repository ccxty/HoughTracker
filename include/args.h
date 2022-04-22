#include <iostream>
#include <string>

#include "clipp.h"
#ifndef __ARGS_PARSE_CXX_INCLUDE__
#define __ARGS_PARSE_CXX_INCLUDE__ 1

enum class ExecMode { all, single, help };
struct Args {
    std::string pt_str;
    std::string n_noise_str;
    std::string n_track_str;
    std::string particle;
    ExecMode mode = ExecMode::help;
};

void arg_parse_cmd(int argc, char **argv, Args &args) {
    using clipp::command;
    using clipp::option;
    using clipp::required;
    using clipp::usage_lines;
    using clipp::value;
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
    auto cli = ((all_mode | single_mode |
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
            case ExecMode::help:
                std::cout << make_man_page(cli, "HoughTracker");
                exit(0);
        }
    } else {
        std::cout << usage_lines(cli, "HoughTracker") << '\n';
        exit(1);
    }
}

#endif