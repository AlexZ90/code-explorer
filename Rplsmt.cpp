#include "Rplsmt.h"

Rplsmt::Rplsmt(){

	pos = 0;
	insText = "Empty";

}

Rplsmt::Rplsmt (long int pos, std::string insText){
	this->pos = pos;
	this->insText = insText;
}
