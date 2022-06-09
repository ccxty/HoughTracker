#include "args.h"
#include "run_hough.h"

int main(int argc, char **argv) {
    Args args;
    args_parse(argc, argv, "HoughTracker", args);
    args_out_json(args);
    auto c_args = export_c_args(args);
    run_hough(c_args);
    return 0;
}