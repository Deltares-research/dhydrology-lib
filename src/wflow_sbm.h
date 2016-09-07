#ifndef WFLOW_SBM_H
#define WFLOW_SBM_H
/// move to model init
struct sbm_par {
	double CanopyGapFraction = 0.6;
	double Cmax = 1.0;
	double thetaS = 0.6;
	double thetaR = 0.01;
	double FirstZoneThickness = 4000;
	double FirstZoneCapacity;
	double PathFrac  = 0.0;
	double InfiltCapSoil = 2000;
	double InfiltCapPath = 10;
	double RootingDepth = 500;
	double FirstZoneKsatVer = 400.;
	double f = 0.1;
	double CapScale = 5000;
	double timestepsecs =3600;
	double basetimestep = 86400;
	double MaxLeakage = 0.0;
	double MaxPercolation = 0.0;
};

struct sbm_state {
	double CanopyStorage = 0.0;
	double FirstZoneDepth = 0.0;
	double UStoreDepth = 0.0;
};
#endif
