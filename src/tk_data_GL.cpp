#include "tk_data_GL.hpp"

void GLDataClass::dataInput(DataClass *data_in)
{
	data_in->GoToHomeFlag = data.GoToHomeFlag;
	data_in->nazo.expflag = data.nazo.expflag;
	data_in->pioneer_activate = data.pioneer_activate;
	data_in->human_num = data.human_num;
	data_in->human = data.human;
	data_in->pa_human[0] = data.pa_human[0];
	data_in->pa_human[1] = data.pa_human[1];
	data_in->pa_human[2] = data.pa_human[2];
	data_in->sensitivity_of_anxiety = data.sensitivity_of_anxiety;
}
