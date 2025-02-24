# adcmemulate
Program for emulation and handling ADCM data.

Options:
  -b <begin>     Begin spill index (default = 0)
  -s <size>      Size of chunk (-ge 1, default = 1)
  -d <delay>     Turn on adcm emulation mode with delay in msecs (-ge 1000). In
                 emulation mode <size> -e 1.
  -o <overlap>   Size of chunk overlap (-ge 0, default = 0)
  -n <number>    Number of chunks (unused, -ge 1, default = 1)
  -e <end>       End spill index (unused)
  -h, --help     Displays help on commandline options.
  --help-all     Displays help including Qt specific options.
  -v, --version  Displays version information.

Arguments:
  input          Input file name.
  output         Output file name.

Examples:
Handle mode
./adcmemulate inputFile outputFile -b 2 -s 10 -o 5
produces as many files (chunks) as can be get from inputFile, the first spill from inputFile is 3, every chunk has name "outputFile_f_t", f-the first spill index in chunk, t-the last spill index in chunk, overlapping is 5 spills, e.g. serial files has 5 identical spills.

Emulation mode
./adcmemulate inputFile outputFile -b 3 -d 2000
produces one file till inputFile ends (chunk with size = 1 spill, options "s", "o" are ignored) "outputFile", the first spill from inputFile is 3, overwrites it every 2000 ms.
