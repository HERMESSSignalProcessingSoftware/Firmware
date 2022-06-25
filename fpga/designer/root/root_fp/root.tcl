open_project -project {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root\root_fp\root.pro}\
         -connect_programmers {FALSE}
load_programming_data \
    -name {M2S010} \
    -fpga {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root\root.map} \
    -header {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root\root.hdr} \
    -envm {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root\root.efc} \
    -spm {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root\root.spm} \
    -dca {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root\root.dca}
export_single_ppd \
    -name {M2S010} \
    -file {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root\export/tempExport\root.ppd}

save_project
close_project
