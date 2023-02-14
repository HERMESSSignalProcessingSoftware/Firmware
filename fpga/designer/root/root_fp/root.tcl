open_project -project {C:\Users\RG\Documents\Firmware\fpga\designer\root\root_fp\root.pro}\
         -connect_programmers {FALSE}
load_programming_data \
    -name {M2S010} \
    -fpga {C:\Users\RG\Documents\Firmware\fpga\designer\root\root.map} \
    -header {C:\Users\RG\Documents\Firmware\fpga\designer\root\root.hdr} \
    -envm {C:\Users\RG\Documents\Firmware\fpga\designer\root\root.efc} \
    -spm {C:\Users\RG\Documents\Firmware\fpga\designer\root\root.spm} \
    -dca {C:\Users\RG\Documents\Firmware\fpga\designer\root\root.dca}
export_single_ppd \
    -name {M2S010} \
    -file {C:\Users\RG\Documents\Firmware\fpga\designer\root\export/tempExport\root.ppd}

save_project
close_project
