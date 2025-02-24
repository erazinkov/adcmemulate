# ADCMEmulate
Program for emulation and handling ADCM data.

Options:<br/>
  -b <begin>     Begin spill index (default = 0)<br/>
  -s <size>      Size of chunk (-ge 1, default = 1)<br/>
  -d <delay>     Turn on adcm emulation mode with delay in msecs (-ge 1000). In
                 emulation mode <size> -e 1.<br/>
  -o <overlap>   Size of chunk overlap (-ge 0, default = 0)<br/>
  -n <number>    Number of chunks (unused, -ge 1, default = 1)<br/>
  -e <end>       End spill index (unused)<br/>
  -h, --help     Displays help on commandline options.<br/>
  --help-all     Displays help including Qt specific options.<br/>
  -v, --version  Displays version information.<br/>

Arguments:<br/>
  input          Input file name.<br/>
  output         Output file name.<br/>

## How To<br/>
### Handler<br/>
`./adcmemulate inputFile outputFile -b 2 -s 10 -o 5`<br/>
* produces as many files (chunks) as can be get from inputFile
* the first spill from inputFile has index 2
* every chunk has name "outputFile_f_t" f-the first spill index in chunk, t-the last spill index in chunk
* overlapping is 5 spills, e.g. serial files has 5 identical spills

### Emulation<br/>
`./adcmemulate inputFile outputFile -b 3 -d 2000`<br/>
* produces one file till inputFile ends (chunk with size = 1 spill, options "s", "o" are ignored) "outputFile"
* the first spill from inputFile has index 3
* overwrites "outputFile" every 2000 ms.
