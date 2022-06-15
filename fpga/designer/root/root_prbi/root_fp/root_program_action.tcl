open_project -project {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root_fp/root.pro}
set_programming_file \
    -name {M2S010} \
    -file {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root.ipd} 
enable_device \
    -name {M2S010} \
    -enable {1} 
set_programming_action \
    -name {M2S010} \
    -action {PROGRAM} 
run_selected_actions
set_programming_file \
    -name {M2S010} \
    -no_file
save_project
close_project
