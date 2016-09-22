#ifndef WFLOW_SBM_H
#define WFLOW_SBM_H
/// move to model init
struct sbm_par {
	double CanopyGapFraction = 0.2;
	double Cmax = 1.0;
	double thetaS = 0.6;
	double thetaR = 0.01;
	double FirstZoneThickness = 8000;
	double FirstZoneCapacity = 4000;
	double PathFrac  = 0.0;
	double InfiltCapSoil = 2000;
	double InfiltCapPath = 10;
	double RootingDepth = 1200;
	double FirstZoneKsatVer = 400.;
	double f = 0.0;
	double CapScale = 5000;
	double timestepsecs = 86400;
	double basetimestep = 3600;
	double MaxLeakage = 0.0;
	double MaxPercolation = 0.0;
	double M = 500.0;
	double GWScale = 0.0;
	double RunoffGeneratingGWPerc=0.1;
	double DrainageBase=150.0;
	double DemMax=125.0;
	double Altitude = 112.0;
	double CC = 0.0;
	double Slope = 0.01;
	double FirstZoneKsatHorFrac = 1.0;
};

struct sbm_state {
	double CanopyStorage = 0.0;
	double FirstZoneDepth = 0.0;
	double UStoreDepth = 0.0;
};

struct sbm_out
{
	int retval = 0;
	double Inwater = 0.0;
	double ActEvap = 0.0;
	double Interception = 0.0;
	double TotEvap;
	double StemFlow = 0.0;
	double ThroughFall = 0.0;
	double ExcessWater;
	double RunoffOpenWater;
	double SubCellRunoff;
	double ActInfilt;
	double reinfiltwater;
	double Transpiration;
	double soilevap;
	double ExfiltWater;
	double SubCellGWRunoff;
	double FirstZoneFlux;
};

#ifdef __cplusplus
extern "C" {
#endif

	EXPORT_API extern sbm_state state_sbm;
	EXPORT_API extern sbm_par par_sbm;
	EXPORT_API extern sbm_out out_sbm;
#ifdef __cplusplus
}
#endif

int wfhydro_sbm_update(double Precipitation, double PotEvap, double WaterFrac, double WaterLevel, sbm_par	par, 
	sbm_state *state);
int wfhydro_sbm_initial(sbm_par *par);
#endif
