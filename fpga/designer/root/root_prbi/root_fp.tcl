new_project \
         -name {root} \
         -location {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root_fp} \
         -mode {chain}
add_actel_device \
         -device {M2S010} \
         -name {M2S010}
enable_device \
         -name {M2S010} \
         -enable {TRUE}
save_project
