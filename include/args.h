#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "clipp.h"
#include "json.hpp"
#ifndef __ARGS_PARSE_CXX_INCLUDE__
#define __ARGS_PARSE_CXX_INCLUDE__

enum ExecMode { all, single, help, json };

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

struct CArgs {
    char *data_file;
    char *output_file;
    double pt;
    int n_noise;
    int n_track;
    char *particle;
    ExecMode mode;
    char *deg;
};

CArgs export_c_args(Args &args);

void args_out_json(Args &args);

void args_parse_json(Args &args, std::string &json_file);
void args_parse(int argc, char **argv, const std::string &program_name,
                Args &args);
#endif