make \
CFLAGS+="-DDEFBEG_777='printf(\"start %s\n\",__func__);' -DDEFEND_777='printf(\"end %s\n\",__func__);'" \
CXXFLAGS+="-DDEFBEG_777='printf(\"start %s\n\",__func__);' -DDEFEND_777='printf(\"end %s\n\",__func__);'"
