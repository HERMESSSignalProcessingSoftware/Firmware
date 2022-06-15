open_project -project {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root_fp/root.pro}
load_programming_data \
    -name {M2S010} \
    -fpga {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root.map} \
    -dca {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root.dca} \
    -header {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root.hdr} \
    -spm {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root.spm}  \
    -gen_bitstream
set_programming_file -name {M2S010} -no_file
save_project
close_project
