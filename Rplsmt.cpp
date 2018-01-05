#include "Rplsmt.h"

Rplsmt::Rplsmt(){

	pos = 0;
	insText = "Empty";

}

Rplsmt::Rplsmt (long int pos, std::string insText){
	this->pos = pos;
	this->insText = insText;
}


const char* getRplsmtText(std::string& insText)
{
    if (insText == "B")
    {
        return (" DEFBEG_777 ");  
    }
    else if (insText == "E")
    {
         return(" DEFEND_777 ");  
    }
    else if (insText == "R")
    {
        return(" DEFRET_777 ");
    }
    return ("");

}


void writeRplsmtDefines(std::fstream& fs)
{
    fs << "#ifndef INSTRUMENT_DEFINES_777" << std::endl;
    fs << "#define INSTRUMENT_DEFINES_777" << std::endl;
    fs << "#include <stdio.h>" << std::endl;

	fs << "#include <sys/types.h>" <<std::endl;
	fs << "#include <sys/syscall.h>" <<std::endl;
	fs << "#include <unistd.h>" << std::endl;

	fs << "#ifdef SYS_gettid" << std::endl;
	fs << "#define PRINTTID syscall(SYS_gettid)" << std::endl;
	fs << "#else" << std::endl;
	fs << "#warning \"SYS_gettid unavailable on this system\"" << std::endl;
	fs << "#define PRINTTID 0" << std::endl;
	fs << "#endif" << std::endl;

    fs << "#define DEFBEG_777 printf (\"\\nstart %s %ld %s %d\\n\",__func__,PRINTTID,__FILE__,__LINE__); " << std::endl;
    fs << "#define DEFEND_777 printf (\"\\nend %s %ld %s %d\\n\",__func__,PRINTTID,__FILE__,__LINE__); " << std::endl;
    fs << "#define DEFRET_777 if (0 & printf (\"\\nret %s %ld %s %d\\n\",__func__,PRINTTID,__FILE__,__LINE__)); else " << std::endl;



    fs << "#endif" << std::endl;
}
