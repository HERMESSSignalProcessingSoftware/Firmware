add_prb \
         -net    {STAMP1_SCLK_c} \
         -port   {Probe_Insert1} \
         -pkgpin {47} \
         -fam    {PA4M}
add_prb \
         -net    {mosi_cl} \
         -port   {Probe_Insert2} \
         -pkgpin {46} \
         -fam    {PA4M}
prbi \
         -desdir {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root} \
         -design {root} \
         -fam {PA4M} \
         -die {PA4M1000_N} \
         -pkg {tq144} \
         -use_debug {0} \
         -init_lock_file {}
