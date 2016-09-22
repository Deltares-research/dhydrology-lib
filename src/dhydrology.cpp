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
		double timestep,
		double *CanopyStorage, double *UStoreDepth, double *FirstZoneDepth,
		double *EvapoTranspiration, double *ToRunoff)
		/**
		 *  @brief Updates the hydrological model
		 *  
		 *  @param[in] precipitation Precip in this timestep [mm]
		 *  @param[in] PotEvap Potential Evapotranspiration this timestep [mm]
		 *  @param[in] WaterFrac Fraction of Open water between 0-1 [-]
		 *  @param[in] WaterLevel Average level of open water [m]
		 *  @param[in] timestep length of this timestep [s]
		 *  
		 *  State variables:
		 *  @param[in,out] *CanopyStorage Actual Iterception storage [mm]
		 *  @param[in,out] *UStoreDepth Storage in the unsaturated zone (mm)
		 *  @param[in,out] *FirstZoneDepth Storage in the saturated zone [mm]
		 *  
		 *  Output
		 *  @param[out] *EvapoTranspiration Actual evapotranspiration [mm]
		 *  @param[out] *ToRunoff Net runoff to open water (can also be -) [mm]
		 */
	{
		state_sbm.CanopyStorage = *CanopyStorage;
		state_sbm.FirstZoneDepth = *FirstZoneDepth;
		state_sbm.UStoreDepth = *UStoreDepth;
		par_sbm.timestepsecs = timestep;
		int ret = wfhydro_sbm_update(Precipitation, PotEvap, WaterFrac, WaterLevel, par_sbm, &state_sbm);

		

		*EvapoTranspiration = out_sbm.TotEvap;
		*ToRunoff = out_sbm.Inwater;




		*FirstZoneDepth = state_sbm.FirstZoneDepth;
		*UStoreDepth = state_sbm.UStoreDepth;
		*CanopyStorage = state_sbm.CanopyStorage;
		

		return (ret);
	}
}
