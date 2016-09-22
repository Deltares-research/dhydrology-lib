#ifndef WFLOW_SBM_H
#define WFLOW_SBM_H
/// move to model init
struct sbm_par {
	double CanopyGapFraction;
	double Cmax;
	double thetaS;
	double thetaR;
	double FirstZoneThickness;
	double FirstZoneCapacity;
	double PathFrac;
	double InfiltCapSoil;
	double InfiltCapPath;
	double RootingDepth;
	double FirstZoneKsatVer;
	double f;
	double CapScale;
	double timestepsecs;
	double basetimestep;
	double MaxLeakage;
	double MaxPercolation;
	double M;
	double GWScale;
	double RunoffGeneratingGWPerc;
	double DrainageBase;
	double DemMax;
	double Altitude;
	double CC;
	double Slope;
	double FirstZoneKsatHorFrac;
};

struct sbm_state {
	double CanopyStorage;
	double FirstZoneDepth;
	double UStoreDepth;
};

struct sbm_out
{
	int retval;
	double Inwater;
	double ActEvap;
	double Interception;
	double TotEvap;
	double StemFlow;
	double ThroughFall;
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
void rainfall_interception_modrut(double Precipitation, double PotEvap, double CanopyGapFraction, double Cmax,
	double  *NetInterception, double *ThroughFall, double *StemFlow, double *LeftOver, double *Interception,
	double *CanopyStorage);
#endif
