// SampleCppLibrary.cpp : Defines the exported functions for the DLL application.

#include <string>
#include <iostream>

#include "dhydrology.h"
#include "wflow_sbm.h"

// placeholder function, all dll's need a main.. in windows only
#if defined _WIN32
void main()
{
}
#endif

extern "C" {
    EXPORT_API int HYDRO_INITIALIZE(char *configfile)
    {
		int ret = wfhydro_sbm_initial(&par_sbm);


        return ret;
    }

	EXPORT_API int HYDRO_FINALIZE()
    {
		std::cout << "Hello world!" << std::endl;

		return 0;    
    }


}



extern "C" {
	EXPORT_API int HYDRO_UPDATE_CELL(double Precipitation, double PotEvap, double WaterFrac, double WaterLevel,
		double *CanopyStorage, double *UStoreDepth, double *FirstZoneDepth,
		double EvapoTranspiration, double ToRunoff, sbm_out *theoutput)
	{
		state_sbm.CanopyStorage = *CanopyStorage;
		state_sbm.FirstZoneDepth = *FirstZoneDepth;
		state_sbm.UStoreDepth = *UStoreDepth;
		int ret = wfhydro_sbm_update(Precipitation, PotEvap, WaterFrac, WaterLevel, par_sbm, &state_sbm);

		*theoutput = out_sbm;

		EvapoTranspiration = theoutput->TotEvap;
		ToRunoff = theoutput->Inwater;




		*FirstZoneDepth = state_sbm.FirstZoneDepth;
		*UStoreDepth = state_sbm.UStoreDepth;
		*CanopyStorage = state_sbm.CanopyStorage;
		

		return (ret);
	}
}
