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

save_project
close_project
