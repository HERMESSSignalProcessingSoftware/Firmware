<<<<<<< HEAD
open_project -project {C:\Users\RG\Documents\Firmware\fpga\designer\root\root_fp\root.pro}
enable_device -name {M2S010} -enable 1
set_programming_file -name {M2S010} -file {C:\Users\RG\Documents\Firmware\fpga\designer\root\root.ppd}
set_programming_action -action {PROGRAM} -name {M2S010} 
run_selected_actions
=======
open_project -project {B:\HERMESS_SPSoftware\Firmware\fpga\designer\root\root_fp\root.pro}\
         -connect_programmers {FALSE}
load_programming_data \
    -name {M2S010} \
    -fpga {B:\HERMESS_SPSoftware\Firmware\fpga\designer\root\root.map} \
    -header {B:\HERMESS_SPSoftware\Firmware\fpga\designer\root\root.hdr} \
    -envm {B:\HERMESS_SPSoftware\Firmware\fpga\designer\root\root.efc} \
    -spm {B:\HERMESS_SPSoftware\Firmware\fpga\designer\root\root.spm} \
    -dca {B:\HERMESS_SPSoftware\Firmware\fpga\designer\root\root.dca}
export_single_ppd \
    -name {M2S010} \
    -file {B:\HERMESS_SPSoftware\Firmware\fpga\designer\root\root.ppd}

>>>>>>> e34d31ef438c63e3bfd3979803f72878cb389376
save_project
close_project
