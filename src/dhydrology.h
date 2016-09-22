#ifndef DHYDROLOGY_H
#define DHYDROLOGY_H

#define DHYDROLOGY_VERSION_MAJOR 1
#define DHYDROLOGY_VERSION_MINOR 0

#if defined _WIN32
#define EXPORT_API __declspec(dllexport)

/* Calling convention, stdcall in windows, cdecl in the rest of the world */
#define CALLCONV __stdcall 
#else
#define EXPORT_API
#define CALLCONV
#endif
#include "wflow_sbm.h"

#ifdef __cplusplus
extern "C"
{
#endif

    EXPORT_API int HYDRO_INITIALIZE(char *configfile);

	/**
	*  @brief Updates the hydrological model
	*
	*  @param[in] Precipitation Precip in this timestep [mm]
	*  @param[in] PotEvap Potential Evapotranspiration this timestep [mm]
	*  @param[in] WaterFrac Fraction of Open water between 0-1 [-]
	*  @param[in] WaterLevel Average level of open water [m]
	*  @param[in] timestep length of this timestep [s]
	*
	*  State variables:
	*  @param[in,out] CanopyStorage Actual Iterception storage [mm]
	*  @param[in,out] UStoreDepth Storage in the unsaturated zone (mm)
	*  @param[in,out] FirstZoneDepth Storage in the saturated zone [mm]
	*
	*  Output
	*  @param[out] EvapoTranspiration Actual evapotranspiration [mm]
	*  @param[out] ToRunoff Net runoff to open water (can also be -) [mm]
	*/
	EXPORT_API int HYDRO_UPDATE_CELL(double Precipitation, double PotEvap, double WaterFrac, double WaterLevel,
		double timestep,
		double *CanopyStorage, double *UStoreDepth, double *FirstZoneDepth,
		double *EvapoTranspiration, double *ToRunoff);

	EXPORT_API int HYDRO_FINALIZE();

#ifdef __cplusplus
}
#endif

#endif
