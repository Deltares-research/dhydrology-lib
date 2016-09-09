/*
State vars:
FirstZoneDepth
UStoreDepth
CanopyStorage


Forcing:
Precipitation
PotEvap


Parameters:
CanopyGapFraction
Cmax
PathFrac
FirstZoneThickness
*/

#include <iostream>
#include <math.h> 
#include "dhydrology.h"
#include "wflow_sbm.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


extern "C" {
	EXPORT_API sbm_state state_sbm;
	EXPORT_API sbm_par par_sbm;
	EXPORT_API sbm_out out_sbm;
}

double sCurve(double X, double a, double b, double c) {
	/*
	double a= 0.0, b = 1.0, c = 1.0) :
	sCurve function :

	Input :
	-X input map
	- C determines the steepness or "stepwiseness" of the curve.
	The higher C the sharper the function.A negative C reverses the function.
	- b determines the amplitude of the curve
	- a determines the centre level(default = 0)

	Output :
	-result
	*/

	double s;

	s = 1.0 / (b + exp(-c * (X - a)));
	
	return (s);
}

void rainfall_interception_modrut(double Precipitation, double PotEvap, double CanopyGapFraction, double Cmax,
	double  *NetInterception, double *ThroughFall, double *StemFlow, double *LeftOver, double *Interception, 
		double *CanopyStorage) {
	/*
	Interception according to a modified Rutter model.The model is solved
	explicitly and there is no drainage below Cmax.

	Returns :
	-NetInterception : P - TF - SF(may be different from the actual wet canopy evaporation)
	- ThroughFall :
	-StemFlow :
	-LeftOver : Amount of potential eveporation not used
	- Interception : Actual wet canopy evaporation in this thimestep
	- CanopyStorage : Canopy storage at the end of the timestep
	*/
	double p, pt, Pfrac, DD, dC, D;

	p = CanopyGapFraction;
	pt = 0.1 * p;

	// Amount of P that falls on the canopy
	Pfrac = (1 - p - pt) * Precipitation;

	// S cannot be larger than Cmax, no gravity drainage below that
	DD = *CanopyStorage > Cmax ?  *CanopyStorage - Cmax: 0.0;
	*CanopyStorage = *CanopyStorage - DD;

	// Add the precipitation that falls on the canopy to the store
	*CanopyStorage = *CanopyStorage + Pfrac;

	// Now do the Evap, make sure the store does not get negative
	dC = -1 * (*CanopyStorage > PotEvap ? PotEvap : *CanopyStorage);
	*CanopyStorage += dC;

	*LeftOver = PotEvap + dC; // Amount of evap not used

	// Now drain the canopy storage again if needed.. .
	D = *CanopyStorage > Cmax ? *CanopyStorage - Cmax : 0.0;
	*CanopyStorage = *CanopyStorage - D;

	// Calculate throughfall
	*ThroughFall = DD + D + p * Precipitation;
	*StemFlow = Precipitation * pt;

	// Calculate interception, this is NET Interception
	*NetInterception = Precipitation - *ThroughFall - *StemFlow;
	*Interception = -dC;
}



double actEvap_SBM(double RootingDepth, double WTable, double *UStoreDepth, double *FirstZoneDepth, 
	double *ActEvapUStore, double PotTrans, double smoothpar) {
	/*
	
	Actual evaporation function :

	-first try to get demand from the saturated zone, using the rootingdepth as a limiting factor
		- secondly try to get the remaining water from the unsaturated store
		- it uses an S - Curve the make sure roots het wet / dry gradually(basically)
		representing a root - depth distribution

		if ust is True, all ustore is deemed to be avaiable fro the roots a

		Input :

	-RootingDepth, WTable, UStoreDepth, FirstZoneDepth, PotTrans, smoothpar

	Output :

	-ActEvap, FirstZoneDepth, UStoreDepth ActEvapUStore
	*/

	// Step 1 from saturated zone, use rootingDepth as a limiting factor
	/*	#rootsinWater = WTable < RootingDepth
		#ActEvapSat = ifthenelse(rootsinWater, min(PotTrans, FirstZoneDepth), 0.0)
		# new method:
		# use sCurve to determine if the roots are wet.At the moment this ise set
		# to be a 0 - 1 curve
	*/
	int ust = 0;
	double wetroots, ActEvapSat, RestPotEvap, AvailCap, MaxExtr, ActEvap;

	
	wetroots = sCurve(WTable, RootingDepth, 1.0, smoothpar);
	ActEvapSat = MIN(PotTrans * wetroots, *FirstZoneDepth);
	*FirstZoneDepth = (*FirstZoneDepth) - ActEvapSat;
	RestPotEvap = PotTrans - ActEvapSat;

	// now try unsat store
	// #AvailCap = min(1.0, max(0.0, (WTable - RootingDepth) / (RootingDepth + 1.0)))

	if (ust >= 1) {
		AvailCap = *UStoreDepth * 0.99;
	}else{
		AvailCap = MAX(0.0, WTable < RootingDepth ? 1.0 : RootingDepth / (WTable + 1.0));
	}
	MaxExtr = AvailCap * *UStoreDepth;
	*ActEvapUStore = MIN(MaxExtr, MIN(RestPotEvap, *UStoreDepth));
	*UStoreDepth = *UStoreDepth - *ActEvapUStore;
	ActEvap = ActEvapSat + *ActEvapUStore;

	return (ActEvap);
}


/*
	WaterFrac - fraction of open water in this unit
*/
int wfhydro_sbm_update(double Precipitation, double PotEvap, double WaterFrac, double WaterLevel,  sbm_par	par, 
	sbm_state *states) {
	double MporeFrac = 0.0;
	double soilInfRedu = 1.0;
	double NetInterception, ThroughFall, StemFlow, LeftOver, Interception, ActEvapUStore;


	par.FirstZoneCapacity = par.FirstZoneThickness * (par.thetaS - par.thetaR);
	par.f = (par.thetaS - par.thetaR) / par.M;
	double ActRootingDepth = par.RootingDepth;
	// first run the rainfall interception function
	rainfall_interception_modrut(Precipitation, PotEvap, par.CanopyGapFraction, par.Cmax, &NetInterception, 
		&ThroughFall, &StemFlow, 
		&LeftOver,	&Interception, &states->CanopyStorage);

	double PotTransSoil = MAX(0.0, LeftOver); //  # now in mm
	Interception = NetInterception;
	double zi = MAX(0.0, par.FirstZoneThickness - states->FirstZoneDepth / (par.thetaS - par.thetaR));//  # Determine actual water depth
	// set FirstZoenDepth -> this extra only neede to be able to set zi from the outside
	states->FirstZoneDepth = (par.thetaS - par.thetaR) * (par.FirstZoneThickness - zi);
	double UStoreCapacity = par.FirstZoneCapacity - states->FirstZoneDepth - states->UStoreDepth;
	double AvailableForInfiltration = ThroughFall + StemFlow;

	// Runoff from water bodies and river network
	double RunoffOpenWater = WaterFrac * AvailableForInfiltration;
	AvailableForInfiltration -= RunoffOpenWater;

	// TODO: add runoff subgrid generation. We ignore subgrid runoff generation for now!

	// First determine if the soil infiltration capacity can deal with the
	// amount of water
	// split between infiltration in undisturbed soil and compacted areas(paths)
	double SoilInf = AvailableForInfiltration * (1 - par.PathFrac);
	double PathInf = AvailableForInfiltration * par.PathFrac;
	
	// TODO: add soil infiltration reduction and snow modelling
	double MaxInfiltSoil = MIN(SoilInf, par.InfiltCapSoil * soilInfRedu); 
	double InfiltSoil = MIN(MaxInfiltSoil, UStoreCapacity);
	states->UStoreDepth = states->UStoreDepth + InfiltSoil;
	UStoreCapacity = UStoreCapacity - InfiltSoil;
	AvailableForInfiltration = AvailableForInfiltration - InfiltSoil;
	double MaxInfiltPath = MIN(par.InfiltCapPath * soilInfRedu, PathInf);
	double InfiltPath = MIN(MaxInfiltPath, UStoreCapacity);
	states->UStoreDepth +=  InfiltPath;
	UStoreCapacity -= InfiltPath;
	AvailableForInfiltration = AvailableForInfiltration - InfiltPath;
	double ActInfilt = InfiltPath + InfiltSoil;
	double InfiltExcess = UStoreCapacity > 0.0 ? AvailableForInfiltration : 0.0;
	double ExcessWater = AvailableForInfiltration; // Saturation overland flow
	
	// Limit rootingdepth(if set externally)
	ActRootingDepth = MIN(par.FirstZoneThickness * 0.99, ActRootingDepth);
	// Determine transpiration
	// Split between bare soil / open water and vegetation
	double potsoilopenwaterevap = (1.0 - par.CanopyGapFraction) * PotTransSoil;
	double PotTrans = PotTransSoil - potsoilopenwaterevap;
	
	double SaturationDeficit = par.FirstZoneCapacity - states->FirstZoneDepth;
	// Linear reduction of soil moisture evaporation based on deficit
	// Determine transpiration
		
	// Determine Open Water EVAP.Later subtract this from water that
	// enters the Kinematic wave
	double RestEvap = potsoilopenwaterevap;
	double ActEvapOpenWater = MIN(WaterLevel * 1000.0 * WaterFrac, WaterFrac * RestEvap);
	RestEvap = RestEvap - ActEvapOpenWater;
	// Next the rest is used for soil evaporation
	double soilevap = RestEvap * MAX(0.0, MIN(1.0, SaturationDeficit / par.FirstZoneCapacity));
	soilevap = MIN(soilevap, states->UStoreDepth);
	states->UStoreDepth -= soilevap;
	// rest is used for transpiration
	PotTrans = PotTransSoil - soilevap - ActEvapOpenWater;
	
	double Transpiration = actEvap_SBM(ActRootingDepth, zi,  &states->UStoreDepth,  &states->FirstZoneDepth,  
		&ActEvapUStore,	PotTrans, 3000.0);


	double ActEvap = Transpiration + soilevap + ActEvapOpenWater;
	//##########################################################################
	//# Transfer of water from unsaturated to saturated store...################
	//##########################################################################
	//# Determine saturation deficit.NB, as noted by Vertessy and Elsenbeer 1997
	//# this deficit does NOT take into account the water in the unsaturated zone

	//# Optional Macrco - Pore transfer
	double MporeTransfer = ActInfilt * MporeFrac;
	states->FirstZoneDepth += MporeTransfer;
	states->UStoreDepth -= MporeTransfer;
	
	SaturationDeficit = par.FirstZoneCapacity - states->FirstZoneDepth;
	
	zi = MAX(0.0, par.FirstZoneThickness - states->FirstZoneDepth / (par.thetaS - par.thetaR));//  # Determine actual water depth
	double Ksat = par.FirstZoneKsatVer * exp(-par.f * zi);
	double DeepKsat = par.FirstZoneKsatVer * exp(-par.f * par.FirstZoneThickness);

	//# now the actual transfer to the saturated store..
	double Transfer = MIN(states->UStoreDepth, SaturationDeficit <= 0.00001 ? 0.0 : Ksat * states->UStoreDepth / (SaturationDeficit + 1));

	double MaxCapFlux = MAX(0.0, MIN(Ksat, MIN(ActEvapUStore, MIN(UStoreCapacity, states->FirstZoneDepth))));

	//# No capilary flux is roots are in water, max flux if very near to water, lower flux if distance is large
	double CapFluxScale = zi > ActRootingDepth ? par.CapScale / (par.CapScale + zi - ActRootingDepth) * par.timestepsecs / par.basetimestep : 0.0;
	double CapFlux = MaxCapFlux * CapFluxScale;
	// Determine Ksat at base
	double DeepTransfer = MIN(states->FirstZoneDepth, DeepKsat);
	//#ActLeakage = 0.0
	//# Now add leakage.to deeper groundwater
	double ActLeakage = MAX(0.0, MIN(par.MaxLeakage, DeepTransfer));
	double Percolation = MAX(0.0, MIN(par.MaxPercolation, DeepTransfer));
	
	// ActLeakage = Seepage > 0.0 ? -1.0 * Seepage : ActLeakage;
	states->FirstZoneDepth = states->FirstZoneDepth + Transfer - CapFlux - ActLeakage - Percolation;
	states->UStoreDepth = states->UStoreDepth - Transfer + CapFlux;
	//##########################################################################
	// # Horizontal(downstream) transport of water #############################
	// ##########################################################################
	zi = MAX(0.0, par.FirstZoneThickness - states->FirstZoneDepth / (par.thetaS - par.thetaR)); //  # Determine actual water depth

	//# Re - Determine saturation deficit.NB, as noted by Vertessy and Elsenbeer 1997
	//# this deficit does NOT take into account the water in the unsaturated zone
	SaturationDeficit = par.FirstZoneCapacity - states->FirstZoneDepth;

	// TODO: lateral transport
	//##########################################################################
	//# Determine returnflow from first zone          ##########################
	//##########################################################################
	double ExfiltWaterFrac = sCurve(states->FirstZoneDepth, par.FirstZoneCapacity, 1.0, 5.0);
	double ExfiltWater = ExfiltWaterFrac * (states->FirstZoneDepth - par.FirstZoneCapacity);
	//#self.ExfiltWater = ifthenelse(self.FirstZoneDepth - self.FirstZoneCapacity > 0, self.FirstZoneDepth - self.FirstZoneCapacity, 0.0)
	states->FirstZoneDepth -= ExfiltWater;

	//# Re - determine UStoreCapacity
	zi = MAX(0.0, par.FirstZoneThickness - states->FirstZoneDepth / (par.thetaS - par.thetaR));//  # Determine actual water depth

	double ExfiltFromUstore = zi == 0.0 ? (states->UStoreDepth > 0.0 ? states->UStoreDepth : 0.0) : 0.0;
	
	ExfiltWater += ExfiltFromUstore;
	states->UStoreDepth -= ExfiltFromUstore;
	UStoreCapacity = par.FirstZoneCapacity - states->FirstZoneDepth - states->UStoreDepth;
	Ksat = par.FirstZoneKsatVer * exp(-par.f * zi);

	double SubCellRunoff = 0.0, SubCellGWRunoff = 0.0, reinfiltwater = 0.0;
	double InwaterMM = MAX(0.0, ExfiltWater + ExcessWater + SubCellRunoff + SubCellGWRunoff + RunoffOpenWater -
		reinfiltwater - ActEvapOpenWater);

	out_sbm.retval = 0;
	out_sbm.TotEvap = ActEvap + Interception;
	out_sbm.Inwater = InwaterMM;

	return(0);

}