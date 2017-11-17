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
    else if (insText == "R1")
    {
        return(" DEFRET_777_1 ");  
    }
    else if (insText == "R2")
    {
        return(" DEFRET_777_2 ");  
    }
    else if (insText == "R3")
    {
        return(" DEFRET_777_3 ");
    }
    return ("");

}


void writeRplsmtDefines(std::fstream& fs)
{
    fs << "#ifndef INSTRUMENT_DEFINES_777" << std::endl;
    fs << "#define INSTRUMENT_DEFINES_777" << std::endl;
    //fs << "#include <stdio.h>" << std::endl;
    fs << "#define DEFBEG_777 printf (\"start %s \\n\",__func__); " << std::endl;
    fs << "#define DEFEND_777 printf (\"end %s \\n\",__func__); " << std::endl;
    fs << "#define DEFRET_777_1 printf (\"ret %s \\n\",__func__), " << std::endl;
    fs << "#define DEFRET_777_2 {printf (\"ret %s \\n\",__func__); " << std::endl;
    fs << "#define DEFRET_777_3 } " << std::endl;
    fs << "#endif" << std::endl;
}
