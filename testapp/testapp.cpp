// testapp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "dhydrology.h"
#include "wflow_sbm.h"


sbm_out myout;

int _tmain(int argc, _TCHAR* argv[])
{

	double P = 1.0, PET = 2.0, WF = 0.1, WL = 0.0, C = 1.0, US = 20, FZ = 3000, AET = 0.0, Q = 0.0;
	int a;

	HYDRO_INITIALIZE("testing 1 2 3");

	std::cout << "FZ,US,C,P,AET,Q" << std::endl;

	std::cout << FZ << "," << US << "," << C << "," << P << "," << myout.TotEvap << "," << myout.Inwater << std::endl;
	for (a = 1; a < 200; a = a + 1){
		if (a < 16)  {
			P = P + a;
		}
		else
		{
			P = 0.0;
		}
		HYDRO_UPDATE_CELL(&P, &PET, &WF, &WL,
			&C, &US, &FZ,
			&AET, &Q, &myout);

		std::cout << FZ << "," << US << "," << C << "," << P << "," << myout.TotEvap << "," << myout.Inwater << std::endl;
	}

}

