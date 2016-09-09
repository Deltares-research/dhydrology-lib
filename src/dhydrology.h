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
	EXPORT_API int HYDRO_UPDATE_CELL(double *Precipitation, double *PotEvap, double *WaterFrac, double *WaterLevel,
		double *CanopyStorage, double *UStoreDepth, double *FirstZoneDepth,
		double *EvapoTranspiration, double *ToRunoff, sbm_out *theoutput);
	EXPORT_API int HYDRO_FINALIZE();

#ifdef __cplusplus
}
#endif

#endif
