/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2011
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 or 3 of the License
 *  (at your option).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License is available at
 *  http://www.r-project.org/Licenses/
 *
 */

#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c3photo.h"
#include "AuxBioCro.h"
#include "Century.h"
#include "BioCro.h"
#include "AuxwillowGro.h"
#include "AuxcaneGro.h"
#include "crocent.h"
#include "dailywillow.h"


SEXP willowCent(SEXP LAT,                 /* Latitude                  1 */ 
	    SEXP DOY,                 /* Day of the year           2 */
	    SEXP HR,                  /* Hour of the day           3 */
	    SEXP SOLAR,               /* Solar Radiation           4 */
	    SEXP TEMP,                /* Temperature               5 */
	    SEXP RH,                  /* Relative humidity         6 */ 
	    SEXP WINDSPEED,           /* Wind Speed                7 */ 
	    SEXP PRECIP,              /* Precipitation             8 */
	    SEXP KD,                  /* K D (ext coeff diff)      9 */
	    SEXP CHILHF,              /* Chi and Height factor    10 */
	    SEXP NLAYERS,             /* # Lay canop              11 */
	    SEXP IPLANT,             /* Ini PLANT                12 */
            SEXP IRTL,                /* i rhiz to leaf           13 */
            SEXP SENCOEFS,            /* sene coefs               14 */
            SEXP TIMESTEP,            /* time step                15 */
            SEXP VECSIZE,             /* vector size              16 */
	    SEXP SPLEAF,              /* Spec Leaf Area           17 */
	    SEXP SPD,                 /* Spec Lefa Area Dec       18 */
	    SEXP DBPCOEFS,            /* Dry Bio Coefs            19 */
	    SEXP THERMALP,            /* Themal Periods           20 */
	    SEXP TBASE,               /* Base Temp for calculaing thermal time */
	    SEXP VMAX,                /* Vmax of photo            21 */
	    SEXP ALPHA,               /* Quantum yield            22 */
	    SEXP THETA,               /* theta param (photo)      24 */
	    SEXP BETA,                /* beta param  (photo)      25 */
	    SEXP RD,                  /* Dark Resp   (photo)      26 */
	    SEXP CATM,                /* CO2 atmosph              27 */
	    SEXP B0,                  /* Int (Ball-Berry)         28 */
	    SEXP B1,                  /* Slope (Ball-Berry)       29 */
	    SEXP WS,                  /* Water stress flag        30 */
	    SEXP SOILCOEFS,           /* Soil Coefficients        31 */
	    SEXP ILEAFN,              /* Ini Leaf Nitrogen        32 */
	    SEXP KLN,                 /* Decline in Leaf Nitr     33 */
	    SEXP VMAXB1,              /* Effect of N on Vmax      34 */
	    SEXP ALPHAB1,             /* Effect of N on alpha     35 */
	    SEXP MRESP,               /* Maintenance resp         36 */
	    SEXP SOILTYPE,            /* Soil type                37 */
	    SEXP WSFUN,               /* Water Stress Func        38 */
	    SEXP CENTCOEFS,           /* Century coefficients     39 */
            SEXP CENTTIMESTEP,        /* Century timestep         40 */ 
	    SEXP CENTKS,              /* Century decomp rates     41 */
	    SEXP SOILLAYERS,          /* # soil layers            42 */
	    SEXP SOILDEPTHS,          /* Soil Depths              43 */
	    SEXP CWS,                 /* Current water status     44 */
	    SEXP HYDRDIST,            /* Hydraulic dist flag      45 */
	    SEXP SECS,                /* Soil empirical coefs     46 */
	    SEXP KPLN,                /* Leaf N decay             47 */
	    SEXP LNB0,                /* Leaf N Int               48 */
	    SEXP LNB1,                /* Leaf N slope             49 */
            SEXP LNFUN,               /* Leaf N func flag         50 */
            SEXP UPPERTEMP,           /* Temperature Limitations photoParms */
	    SEXP LOWERTEMP,
      SEXP JMAX,
      SEXP JMAXB1,
      SEXP O2,
      SEXP GROWTHRESP,
       SEXP STOMATAWS)           /*temperature Limitation photoParms */
{
  int vecsize = INTEGER(VECSIZE)[0];


	/* This creates vectors which will collect the senesced plant
	   material. This is needed to calculate litter and therefore carbon
	   in the soil and then N in the soil. */

	 
   double jmax1=REAL(JMAX)[0];
   double jmaxb1=REAL(JMAXB1)[0];
    
        double iSp, Sp , propLeaf;
	int i, i2, i3;
	double vmax1;
	double alpha1;
	double theta;
	double beta;
	double Rd1, Ca;
	double b01, b11;


	double Leaf, Stem, Root, LAI, Grain = 0.0;
	double TTc = 0.0;
	double kLeaf = 0.0, kStem = 0.0, kRoot = 0.0, kRhizome = 0.0, kGrain = 0.0;
	double newLeaf, newStem = 0.0, newRoot, newRhizome, newGrain = 0.0;
 





	/* Variables needed for collecting litter */
	double LeafLitter = REAL(CENTCOEFS)[20], StemLitter = REAL(CENTCOEFS)[21];
	double RootLitter = REAL(CENTCOEFS)[22], RhizomeLitter = REAL(CENTCOEFS)[23];
	double LeafLitter_d = 0.0, StemLitter_d = 0.0;
	double RootLitter_d = 0.0, RhizomeLitter_d = 0.0;
	double ALitter = 0.0, BLitter = 0.0;
	/* Maintenance respiration */

	double mrc1 = REAL(MRESP)[0];
	double mrc2 = REAL(MRESP)[1]; 

  double GrowthRespFraction = REAL(GROWTHRESP)[0];
  
	double waterCont;
	double LeafWS;
  double StomWS = REAL(STOMATAWS)[0];
	int timestep;
  int A, B;
	double CanopyA, CanopyT;

	double Rhizome;
  double leafdeathrate1;

	/* Soil Parameters*/
	double FieldC, WiltP, phi1, phi2, soilDepth;
	int soilType, wsFun;
	double LeafN, LeafN_0, kLN;
	double soilEvap, TotEvap;
	int soillayers = INTEGER(SOILLAYERS)[0];
	double cwsVec[soillayers];
	for(i2=0;i2<soillayers;i2++){
		cwsVec[i2] = REAL(CWS)[i2];
	}
	double cwsVecSum = 0.0;
	/* Some soil related empirical coefficients */
	double rfl = REAL(SECS)[0];  /* root factor lambda */
	double rsec = REAL(SECS)[1]; /* radiation soil evaporation coefficient */
	double rsdf = REAL(SECS)[2]; /* root soil depth factor */
	double scsf = REAL(SOILCOEFS)[6]; /* stomatal conductance sensitivity factor */ /* Rprintf("scsf %.2f",scsf); */
	double transpRes = REAL(SOILCOEFS)[7]; /* Resistance to transpiration from soil to leaf */
	double leafPotTh = REAL(SOILCOEFS)[8]; /* Leaf water potential threshold */

        /* Parameters for calculating leaf water potential */
	double LeafPsim = 0.0;

        /* Effect of Nitrogen */
	double kpLN = REAL(KPLN)[0];
	double lnb0 = REAL(LNB0)[0]; 
	double lnb1 = REAL(LNB1)[0];
	int lnfun = INTEGER(LNFUN)[0];

	/* Century */
	double MinNitro = REAL(CENTCOEFS)[19];
	int doyNfert = REAL(CENTCOEFS)[18];
	double Nfert;
	double SCCs[9];
	double Resp = 0.0;
	int centTimestep = INTEGER(CENTTIMESTEP)[0];

	double SeneLeaf, SeneStem, SeneRoot = 0.0, SeneRhizome = 0.0, Tfrosthigh, Tfrostlow, leafdeathrate, Deadleaf;
	double Remob;
	int k = 0, q = 0, m = 0, n = 0;
	int ri = 0;

	struct Can_Str Canopy = {0.0,0.0,0.0};
	struct ws_str WaterS;
	struct dbp_str dbpS;
	struct cenT_str centS; 
	struct soilML_str soilMLS;
	struct soilText_str soTexS; /* , *soTexSp = &soTexS; */
	soTexS = soilTchoose(INTEGER(SOILTYPE)[0]);

	centS.SCs[0] = 0.0;
	centS.SCs[1] = 0.0;
	centS.SCs[2] = 0.0;
	centS.SCs[3] = 0.0;
	centS.SCs[4] = 0.0;
	centS.SCs[5] = 0.0;
	centS.SCs[6] = 0.0;
	centS.SCs[7] = 0.0;
	centS.SCs[8] = 0.0;
	centS.Resp = 0.0;

	SEXP lists, names;

	SEXP DayofYear;
	SEXP Hour;
	SEXP CanopyAssim;
	SEXP CanopyTrans;
	SEXP Leafy;
	SEXP Stemy;
	SEXP Rooty;
	SEXP Rhizomey;
	SEXP Grainy;
	SEXP LAIc;
	SEXP TTTc;
	SEXP SoilWatCont;
	SEXP StomatalCondCoefs;
	SEXP LeafReductionCoefs;
	SEXP LeafNitrogen;
	SEXP AboveLitter;
	SEXP BelowLitter;
	SEXP VmaxVec;
	SEXP AlphaVec;
	SEXP SpVec;
	SEXP MinNitroVec;
	SEXP RespVec;
	SEXP SoilEvaporation;
	SEXP cwsMat;
	SEXP psimMat; /* Holds the soil water potential */
	SEXP rdMat;
	SEXP SCpools;
	SEXP SNpools;
	SEXP LeafPsimVec;
   SEXP DayafterPlanting;
  SEXP GDD; // daily vector of growing degree day
  SEXP GPP; // Gross Primary Productivity
  SEXP NPP; // Net Primary Productivity
  SEXP autoRESP; // Autotrophic Respiration
  SEXP hetRESP; // Heterotrophic Respiration
  SEXP NER; // Net Ecosystem Respiration
  SEXP StemMResp;
  SEXP RootMResp;
  SEXP RhizomeMResp;
  SEXP LeafDarkResp;
  SEXP Stemd;
  SEXP Leafd;
  SEXP Rootd;
  SEXP Rhizomed;
  SEXP Stemlitterd;
  SEXP Leaflitterd;
  SEXP Rootlitterd;
  SEXP Rhizomelitterd;
  SEXP LAId;

  int dailyvecsize = vecsize/24;
   
	PROTECT(lists = allocVector(VECSXP,49));
	PROTECT(names = allocVector(STRSXP,49));

	PROTECT(DayofYear = allocVector(REALSXP,vecsize));
	PROTECT(Hour = allocVector(REALSXP,vecsize));
	PROTECT(CanopyAssim = allocVector(REALSXP,vecsize));
	PROTECT(CanopyTrans = allocVector(REALSXP,vecsize));
	PROTECT(Leafy = allocVector(REALSXP,vecsize));
	PROTECT(Stemy = allocVector(REALSXP,vecsize));
	PROTECT(Rooty = allocVector(REALSXP,vecsize));
	PROTECT(Rhizomey = allocVector(REALSXP,vecsize));
	PROTECT(Grainy = allocVector(REALSXP,vecsize));
	PROTECT(LAIc = allocVector(REALSXP,vecsize));
	PROTECT(TTTc = allocVector(REALSXP,vecsize));
	PROTECT(SoilWatCont = allocVector(REALSXP,vecsize));
	PROTECT(StomatalCondCoefs = allocVector(REALSXP,vecsize));
	PROTECT(LeafReductionCoefs = allocVector(REALSXP,vecsize));
	PROTECT(LeafNitrogen = allocVector(REALSXP,vecsize));
	PROTECT(AboveLitter = allocVector(REALSXP,vecsize));
	PROTECT(BelowLitter = allocVector(REALSXP,vecsize));
	PROTECT(VmaxVec = allocVector(REALSXP,vecsize));
	PROTECT(AlphaVec = allocVector(REALSXP,vecsize));
	PROTECT(SpVec = allocVector(REALSXP,vecsize));
	PROTECT(MinNitroVec = allocVector(REALSXP,vecsize));
	PROTECT(RespVec = allocVector(REALSXP,vecsize));
	PROTECT(SoilEvaporation = allocVector(REALSXP,vecsize));
	PROTECT(cwsMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(psimMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(rdMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(SCpools = allocVector(REALSXP,9));
	PROTECT(SNpools = allocVector(REALSXP,9));
	PROTECT(LeafPsimVec = allocVector(REALSXP,vecsize));
  PROTECT(DayafterPlanting = allocVector(REALSXP,dailyvecsize));
  PROTECT(GDD = allocVector(REALSXP,dailyvecsize));
  PROTECT(GPP = allocVector(REALSXP,dailyvecsize));
  PROTECT(NPP = allocVector(REALSXP,dailyvecsize));
  PROTECT(autoRESP = allocVector(REALSXP,dailyvecsize));
  PROTECT(hetRESP = allocVector(REALSXP,dailyvecsize));
  PROTECT(NER = allocVector(REALSXP,dailyvecsize));
  PROTECT(StemMResp= allocVector(REALSXP,dailyvecsize));
  PROTECT(RootMResp = allocVector(REALSXP,dailyvecsize));
  PROTECT(RhizomeMResp = allocVector(REALSXP,dailyvecsize));
  PROTECT(LeafDarkResp = allocVector(REALSXP,dailyvecsize));
  PROTECT(Stemd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Leafd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Rootd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Rhizomed = allocVector(REALSXP,dailyvecsize));
  PROTECT(Stemlitterd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Leaflitterd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Rootlitterd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Rhizomelitterd = allocVector(REALSXP,dailyvecsize));
  PROTECT(LAId = allocVector(REALSXP,dailyvecsize));

	/* Picking vmax, alpha  */
	vmax1 = REAL(VMAX)[0];
	alpha1 = REAL(ALPHA)[0];
	theta = REAL(THETA)[0];
	beta = REAL(BETA)[0];
	Rd1 = REAL(RD)[0];
	Ca = REAL(CATM)[0];
	b01 = REAL(B0)[0];
	b11 = REAL(B1)[0];
  
  

	LeafN_0 = REAL(ILEAFN)[0];
	LeafN = LeafN_0; /* Initial value of N in the leaf */
	kLN = REAL(KLN)[0];
	timestep = INTEGER(TIMESTEP)[0];

  double iRRHIZOME = REAL(IPLANT)[0];
  double iSSTEM = REAL(IPLANT)[1];
  double iLLEAF=REAL(IPLANT)[2];
  double iRROOT=REAL(IPLANT)[3];
  double ifrRRHIZOME = REAL(IPLANT)[4];
  double ifrSSTEM = REAL(IPLANT)[5];
  double ifrLLEAF=REAL(IPLANT)[6];
  double ifrRROOT=REAL(IPLANT)[7];
  
  
	Sp = REAL(SPLEAF)[0]; 
	SeneLeaf = REAL(SENCOEFS)[0];
	SeneStem = REAL(SENCOEFS)[1];
	SeneRoot = REAL(SENCOEFS)[2];
	SeneRhizome = REAL(SENCOEFS)[3];
  Tfrosthigh = REAL(SENCOEFS)[4];
  Tfrostlow = REAL(SENCOEFS)[5];
	leafdeathrate = REAL(SENCOEFS)[6];


	/* Soil Parameters */
	FieldC = REAL(SOILCOEFS)[0];
	WiltP = REAL(SOILCOEFS)[1];
	phi1 = REAL(SOILCOEFS)[2];
	phi2 = REAL(SOILCOEFS)[3];
	soilDepth = REAL(SOILCOEFS)[4];
	waterCont = REAL(SOILCOEFS)[5];
	wsFun = INTEGER(WSFUN)[0];
	soilType = INTEGER(SOILTYPE)[0];

	SCCs[0] = REAL(CENTCOEFS)[0];
	SCCs[1] = REAL(CENTCOEFS)[1];
	SCCs[2] = REAL(CENTCOEFS)[2];
	SCCs[3] = REAL(CENTCOEFS)[3];
	SCCs[4] = REAL(CENTCOEFS)[4];
	SCCs[5] = REAL(CENTCOEFS)[5];
	SCCs[6] = REAL(CENTCOEFS)[6];
	SCCs[7] = REAL(CENTCOEFS)[7];
	SCCs[8] = REAL(CENTCOEFS)[8];

	propLeaf = REAL(IRTL)[0]; 
	/* It is useful to assume that there is a small amount of
	   leaf area at the begining of the growing season. */
	iLLEAF = iRRHIZOME * ifrRRHIZOME + iSSTEM*ifrSSTEM; 	
	LAI = iLLEAF * Sp;
	iSp = Sp;

/* Creating pointers to avoid calling functions REAL and INTEGER so much */
	int *pt_doy = INTEGER(DOY);
	int *pt_hr = INTEGER(HR);
	double *pt_solar = REAL(SOLAR);
	double *pt_temp = REAL(TEMP);
	double *pt_rh = REAL(RH);
	double *pt_windspeed = REAL(WINDSPEED);
	double *pt_precip = REAL(PRECIP);
	double lat = REAL(LAT)[0];
	int nlayers = INTEGER(NLAYERS)[0];
	int ws = INTEGER(WS)[0];
	double kd = REAL(KD)[0];
	double chil = REAL(CHILHF)[0];
	double hf = REAL(CHILHF)[1];
  double o2=210;
  double Tbase=REAL(TBASE)[0];  /* base temperature */

	
  
  // initialization based on iPlant 
   Stem=iSSTEM*(1-ifrSSTEM); 
   Leaf=iLLEAF;
   Root=iRROOT;
   Rhizome=iRRHIZOME*(1-ifrRRHIZOME);
  /**********NEW Variables DECLARATION FOR WILLOWCENT*************/
  double delTT;
  REAL(TTTc)[0]=0.0;
  int emergence,dap;
  struct c3tree willow;
  double LeafResp,StemResp,RootResp,RhizResp; 
  double dailynetassim, CanopyAGross, dailyGPP;
  double dailydelTT,accumulatedGDD;
  struct dailyclimate dailyclimate;
  struct respirationParms RESP;
  struct senthermaltemp senthermaltemp;
  struct canopyparms canopyparms;
  struct frostParms frostparms; 
   struct management management;

  /***************************************************************/
  
  
  /*******INITIALIZATION OF VARIABLES FOR WILLOWCENT *************/
  LeafResp=0.0;
  StemResp=0.0;
  RootResp=0.0;
  RhizResp=0.0;
  dailyGPP=0.0;
  
  emergence=1;
  dap=0;
  accumulatedGDD=0.0;
  
 RESP.growth.stem=0.3;
 RESP.growth.root=0.3;
 RESP.growth.rhizome=0.3;
 
 RESP.maint.Qstem=2.0;
 RESP.maint.mstem=0.004;
 
 RESP.maint.Qroot=2.0;
 RESP.maint.mroot=0.002;
 
 RESP.maint.Qrhizome=2.0;
 RESP.maint.mrhizome=0.002;
 
 canopyparms.remobFac=0.5;
 
// Rprintf("vecsize=%i\n",vecsize); 
createNULLwillow(&willow,vecsize);

// Initialize based on planting Rate

 willow.rhizome.biomass=REAL(IPLANT)[0];
 willow.stem.biomass=REAL(IPLANT)[1];
 willow.leaf.biomass=REAL(IPLANT)[2];
 willow.root.biomass=REAL(IPLANT)[3];
 
assignManagement(&management);
management.emergenceparms.StoragetoLeaffraction=REAL(IPLANT)[4];
management.emergenceparms.StemtoLeaffraction=REAL(IPLANT)[5];
// Rprintf("Leaf=%f, Rhizome=%f, Stem=%f, RhisFrac=%f, Stemfrac=%f\n",willow.leaf.biomass,willow.rhizome.biomass,willow.stem.biomass,management.emergenceparms.StoragetoLeaffraction,management.emergenceparms.StemtoLeaffraction);
 UpdateC3treeAfterEmergence(&willow,&management);
// Rprintf("LAI old beginning= %f\n",LAI);
   LAI = willow.leaf.biomass * Sp;
// Rprintf("LAI new beginning= %f\n",LAI);
// Rprintf("After Calling Emergence Function");
// Rprintf("Leaf=%f, Rhizome=%f, Stem=%f, RhisFrac=%f, Stemfrac=%f\n",willow.leaf.biomass,willow.rhizome.biomass,willow.stem.biomass,management.emergenceparms.StoragetoLeaffraction,management.emergenceparms.StemtoLeaffraction);
 management.harvestparms.harvestdoy=365;
 
 
  /****************************************************************/

	for(i=0;i<vecsize;i++)
	{
   
   // Calculation of Thermal Time
    delTT=getThermaltime(*(pt_temp+i), Tbase);
    delTT=delTT/24;
    TTc+=delTT;
    REAL(TTTc)[i]=REAL(TTTc)[i-1]+delTT;
    
    if(emergence==0)
    {
            CanopyA = 0.0;
            CanopyAGross =0.0;
          	CanopyT = 0.0;
            willow.autoresp.leafdarkresp=0;
    }
   else
   {
		Canopy = c3CanAC(LAI, *(pt_doy+i), *(pt_hr+i),
			       *(pt_solar+i), *(pt_temp+i),
			       *(pt_rh+i), *(pt_windspeed+i),
			       lat, nlayers,
			       vmax1,jmax1,Rd1,Ca,o2,b01,b11,theta,kd,hf,LeafN, kpLN, lnb0, lnb1, lnfun, StomWS, ws);
             
   /*Rprintf("%f,%f,%f,%f\n",StomWS,LeafWS,kLeaf,newLeaf);              

		/* Collecting the results */
		CanopyA = Canopy.Assim * timestep;
    CanopyA=(1.0-GrowthRespFraction)*CanopyA;
   
		CanopyT = Canopy.Trans * timestep;
    CanopyAGross= Canopy.GrossAssim*timestep;
//    Rprintf("Gross Assimilation = %f, timestep = %i\n", Canopy.GrossAssim, timestep);
   }
           
   /*Rprintf("%f,%f,%f\n",Canopy,CanopyA,GPP);*/      
		/* Inserting the multilayer model */
		if(soillayers > 1){
			soilMLS = soilML(*(pt_precip+i), CanopyT, &cwsVec[0], soilDepth, REAL(SOILDEPTHS), FieldC, WiltP,
					 phi1, phi2, soTexS, wsFun, INTEGER(SOILLAYERS)[0], willow.root.biomass, 
					 LAI, 0.68, *(pt_temp+i), *(pt_solar), *(pt_windspeed+i), *(pt_rh+i), 
					 INTEGER(HYDRDIST)[0], rfl, rsec, rsdf);

			StomWS = soilMLS.rcoefPhoto;
			LeafWS = soilMLS.rcoefSpleaf;
			soilEvap = soilMLS.SoilEvapo;
			for(i3=0;i3<soillayers;i3++){
				cwsVec[i3] = soilMLS.cws[i3];
				cwsVecSum += cwsVec[i3];
				REAL(cwsMat)[i3 + i*soillayers] = soilMLS.cws[i3];
				REAL(rdMat)[i3 + i*soillayers] = soilMLS.rootDist[i3];
			}

			waterCont = cwsVecSum / soillayers;
			cwsVecSum = 0.0;

		}else{

			soilEvap = SoilEvapo(LAI, 0.68, *(pt_temp+i), *(pt_solar+i), waterCont, FieldC, WiltP, 
                                             *(pt_windspeed+i), *(pt_rh+i), rsec);
			TotEvap = soilEvap + CanopyT;
			WaterS = watstr(*(pt_precip+i),TotEvap,waterCont,soilDepth,FieldC,WiltP,phi1,phi2,soilType, wsFun);   
			waterCont = WaterS.awc;
			StomWS = WaterS.rcoefPhoto ; 
			LeafWS = WaterS.rcoefSpleaf;
			REAL(cwsMat)[i] = waterCont;
			REAL(psimMat)[i] = WaterS.psim;
		}

/* An alternative way of computing water stress is by doing the leaf
 * water potential. This is done if the wsFun is equal to 4 */

                if(wsFun == 4){
			/* Calculating the leaf water potential */
			/* From Campbell E = (Psim_s - Psim_l)/R or
			 * evaporation is equal to the soil water potential
			 * minus the leaf water potential divided by the resistance.
			 * This can be rearranged to Psim_l = Psim_s - E x R   */
			/* It is assumed that total resistance is 5e6 m^4 s^-1
			 * kg^-1 
			 * Transpiration is in Mg ha-2 hr-1
			 * Multiply by 1e3 to go from Mg to kg
			 * Multiply by 1e-4 to go from ha to m^2 
			 * This needs to go from hours to seconds that's
			 * why the conversion factor is (1/3600).*/
			LeafPsim = WaterS.psim - (CanopyT * 1e3 * 1e-4 * 1.0/3600.0) * transpRes;

			/* From WIMOVAVC the proposed equation to simulate the effect of water
			 * stress on stomatal conductance */
			if(LeafPsim < leafPotTh){
				/* StomWS = 1 - ((LeafPsim - leafPotTh)/1000 *
				 * scsf); In WIMOVAC this equation is used but
				 * the absolute values are taken from the
				 * potentials. Since they both should be
				 * negative and leafPotTh is greater than
				 * LeafPsim this can be rearranged to*/ 
				StomWS = 1 - ((leafPotTh - LeafPsim)/1000 * scsf);
				/* StomWS = 1; */
				if(StomWS < 0.1) StomWS = 0.1;
			}else{
				StomWS = 1;
			}
		}else{
			LeafPsim = 0;
		}

/************************************************************************************************************************
 ******************* This is Where I am inserting Material for new Function
 * **********************************************************************************************************************/

  StemResp=MRespiration(willow.stem.biomass, RESP.maint.Qstem, RESP.maint.mstem, *(pt_temp+i), timestep);
  willow.autoresp.stemmaint+=StemResp;
  RootResp=MRespiration(willow.root.biomass, RESP.maint.Qroot, RESP.maint.mroot, *(pt_temp+i), timestep);
  willow.autoresp.rootmaint+=RootResp;
  RhizResp=MRespiration(willow.rhizome.biomass, RESP.maint.Qrhizome, RESP.maint.mrhizome, *(pt_temp+i), timestep);
  willow.autoresp.rhizomemaint+=RhizResp;
  willow.autoresp.leafdarkresp+=(CanopyAGross-CanopyA/(1.0-GrowthRespFraction));
  dailynetassim+=CanopyA/(1.0-GrowthRespFraction);//Net Canopy Assimilation
  willow.GPP+=CanopyAGross;
 /*********************************************************************************************************************/
//  Rprintf("%f,%f,%f,%f,%f\n",*(pt_solar+i),*(pt_temp+i),*(pt_precip+i),*(pt_rh+i),*(pt_windspeed+i));

   if(i % 24== 0)
   {
  // update daily climate structure
   getdailyclimate(&dailyclimate, pt_doy,pt_solar,pt_temp, pt_rh, pt_windspeed,pt_precip,i,vecsize);  
  // calculate GDDD for today only if crop is growing, otherwise set it to zero
   dailydelTT = (emergence ==1) ? getThermaltime(dailyclimate.temp, Tbase):0.0;
   // Update day after planting today
   REAL(DayafterPlanting)[dap]=dap;  
   // update GDD (accumulated thermal time) upto today
   accumulatedGDD+=((dap==0)? 0:dailydelTT);
//   REAL(GDD)[dap]=((dap==0)? 0:REAL(GDD)[dap-1])+dailydelTT;
     REAL(GDD)[dap]=accumulatedGDD;
     dailywillow(&willow, REAL(DBPCOEFS),REAL(THERMALP),accumulatedGDD, *(pt_temp+i), dailynetassim,&senthermaltemp, &canopyparms,&frostparms,i,dailydelTT,&RESP,emergence);
     LAI=willow.leaf.biomass*Sp;
    REAL(GPP)[dap]=willow.GPP;
   REAL(LeafDarkResp)[dap]=willow.autoresp.leafdarkresp;
   REAL(StemMResp)[dap]=willow.autoresp.stemmaint;
   REAL(RootMResp)[dap]=willow.autoresp.rootmaint;
   REAL(RhizomeMResp)[dap]=willow.autoresp.rhizomemaint;
   REAL(autoRESP)[dap]= willow.autoresp.total;
   willow.NPP=willow.GPP-willow.autoresp.total;
   REAL(NPP)[dap]=willow.NPP;
   REAL(Stemd)[dap]=willow.stem.biomass;
   REAL(Leafd)[dap]=willow.leaf.biomass;
   REAL(Rootd)[dap]=willow.root.biomass;
   REAL(Rhizomed)[dap]=willow.rhizome.biomass;
   REAL(Stemlitterd)[dap]=willow.stem.litter;
   REAL(Leaflitterd)[dap]=willow.leaf.litter;
   REAL(Rootlitterd)[dap]=willow.root.litter;
   REAL(Rhizomelitterd)[dap]=willow.rhizome.litter;
   REAL(LAId)[dap]=LAI;
   
    willow.autoresp.leafdarkresp=0.0;
    willow.autoresp.stemmaint=0.0;
    willow.autoresp.rootmaint=0.0;
    willow.autoresp.rhizomemaint=0.0;  
    willow.GPP=0;
    willow.NPP=0;
    dailynetassim=0.0;
    dailyGPP=0.0;
    dap+=1;
   }
 
 /*****************************************************************************************************************************
  * ****************************************************************************************************************************/
 




	

        

 
	

		REAL(DayofYear)[i] =  INTEGER(DOY)[i];
		REAL(Hour)[i] =  INTEGER(HR)[i];
		REAL(CanopyAssim)[i] =  CanopyA;
		REAL(CanopyTrans)[i] =  CanopyT; 
		REAL(Leafy)[i] = willow.leaf.biomass;// Leaf;
		REAL(Stemy)[i] = willow.stem.biomass;//Stem;
		REAL(Rooty)[i] =  willow.root.biomass;//Root;
		REAL(Rhizomey)[i] = willow.rhizome.biomass;//Rhizome;
		REAL(Grainy)[i] = Grain;
		REAL(LAIc)[i] = LAI;
		REAL(SoilWatCont)[i] = waterCont;
		REAL(StomatalCondCoefs)[i] = StomWS;
		REAL(LeafReductionCoefs)[i] = LeafWS;
		REAL(LeafNitrogen)[i] = LeafN;
		REAL(AboveLitter)[i] = ALitter;
		REAL(BelowLitter)[i] = BLitter;
		REAL(VmaxVec)[i] = vmax1;
		REAL(AlphaVec)[i] = alpha1;
		REAL(SpVec)[i] = Sp;
		REAL(MinNitroVec)[i] = MinNitro/ (24*centTimestep);
		REAL(RespVec)[i] = Resp / (24*centTimestep);
		REAL(SoilEvaporation)[i] = soilEvap;
		REAL(LeafPsimVec)[i] = LeafPsim;

	}

/* Populating the results of the Century model */

		REAL(SCpools)[0] = centS.SCs[0];
		REAL(SCpools)[1] = centS.SCs[1];
		REAL(SCpools)[2] = centS.SCs[2];
		REAL(SCpools)[3] = centS.SCs[3];
		REAL(SCpools)[4] = centS.SCs[4];
		REAL(SCpools)[5] = centS.SCs[5];
		REAL(SCpools)[6] = centS.SCs[6];
		REAL(SCpools)[7] = centS.SCs[7];
		REAL(SCpools)[8] = centS.SCs[8];

		REAL(SNpools)[0] = centS.SNs[0];
		REAL(SNpools)[1] = centS.SNs[1];
		REAL(SNpools)[2] = centS.SNs[2];
		REAL(SNpools)[3] = centS.SNs[3];
		REAL(SNpools)[4] = centS.SNs[4];
		REAL(SNpools)[5] = centS.SNs[5];
		REAL(SNpools)[6] = centS.SNs[6];
		REAL(SNpools)[7] = centS.SNs[7];
		REAL(SNpools)[8] = centS.SNs[8];

	SET_VECTOR_ELT(lists,0,DayofYear);
	SET_VECTOR_ELT(lists,1,Hour);
	SET_VECTOR_ELT(lists,2,CanopyAssim);
	SET_VECTOR_ELT(lists,3,CanopyTrans);
	SET_VECTOR_ELT(lists,4,Leafy);
	SET_VECTOR_ELT(lists,5,Stemy);
	SET_VECTOR_ELT(lists,6,Rooty);
	SET_VECTOR_ELT(lists,7,Rhizomey);
	SET_VECTOR_ELT(lists,8,Grainy);
	SET_VECTOR_ELT(lists,9,LAIc);
	SET_VECTOR_ELT(lists,10,TTTc);
	SET_VECTOR_ELT(lists,11,SoilWatCont);
	SET_VECTOR_ELT(lists,12,StomatalCondCoefs);
	SET_VECTOR_ELT(lists,13,LeafReductionCoefs);
	SET_VECTOR_ELT(lists,14,LeafNitrogen);
	SET_VECTOR_ELT(lists,15,AboveLitter);
	SET_VECTOR_ELT(lists,16,BelowLitter);
	SET_VECTOR_ELT(lists,17,VmaxVec);
	SET_VECTOR_ELT(lists,18,AlphaVec);
	SET_VECTOR_ELT(lists,19,SpVec);
	SET_VECTOR_ELT(lists,20,MinNitroVec);
	SET_VECTOR_ELT(lists,21,RespVec);
	SET_VECTOR_ELT(lists,22,SoilEvaporation);
	SET_VECTOR_ELT(lists,23,cwsMat);
	SET_VECTOR_ELT(lists,24,psimMat);
	SET_VECTOR_ELT(lists,25,rdMat);
	SET_VECTOR_ELT(lists,26,SCpools);
	SET_VECTOR_ELT(lists,27,SNpools);
	SET_VECTOR_ELT(lists,28,LeafPsimVec);
  SET_VECTOR_ELT(lists,29,DayafterPlanting);
  SET_VECTOR_ELT(lists,30,GDD);
  SET_VECTOR_ELT(lists,31,GPP);
  SET_VECTOR_ELT(lists,32,NPP);
  SET_VECTOR_ELT(lists,33,autoRESP);  
  SET_VECTOR_ELT(lists,34,hetRESP);
  SET_VECTOR_ELT(lists,35,NER);
  SET_VECTOR_ELT(lists,36,StemMResp);
  SET_VECTOR_ELT(lists,37,RootMResp);
  SET_VECTOR_ELT(lists,38,RhizomeMResp);
  SET_VECTOR_ELT(lists,39,LeafDarkResp);
  SET_VECTOR_ELT(lists,40,Stemd);
  SET_VECTOR_ELT(lists,41,Leafd);
  SET_VECTOR_ELT(lists,42,Rootd);
  SET_VECTOR_ELT(lists,43,Rhizomed);
  SET_VECTOR_ELT(lists,44,Stemlitterd);
  SET_VECTOR_ELT(lists,45,Leaflitterd);
  SET_VECTOR_ELT(lists,46,Rootlitterd);
  SET_VECTOR_ELT(lists,47,Rhizomelitterd);
   SET_VECTOR_ELT(lists,48,LAId);

	SET_STRING_ELT(names,0,mkChar("DayofYear"));
	SET_STRING_ELT(names,1,mkChar("Hour"));
	SET_STRING_ELT(names,2,mkChar("CanopyAssim"));
	SET_STRING_ELT(names,3,mkChar("CanopyTrans"));
	SET_STRING_ELT(names,4,mkChar("Leaf"));
	SET_STRING_ELT(names,5,mkChar("Stem"));
	SET_STRING_ELT(names,6,mkChar("Root"));
	SET_STRING_ELT(names,7,mkChar("Rhizome"));
	SET_STRING_ELT(names,8,mkChar("Grain"));
	SET_STRING_ELT(names,9,mkChar("LAI"));
	SET_STRING_ELT(names,10,mkChar("ThermalT"));
	SET_STRING_ELT(names,11,mkChar("SoilWatCont"));
	SET_STRING_ELT(names,12,mkChar("StomatalCondCoefs"));
	SET_STRING_ELT(names,13,mkChar("LeafReductionCoefs"));
	SET_STRING_ELT(names,14,mkChar("LeafNitrogen"));
	SET_STRING_ELT(names,15,mkChar("AboveLitter"));
	SET_STRING_ELT(names,16,mkChar("BelowLitter"));
	SET_STRING_ELT(names,17,mkChar("VmaxVec"));
	SET_STRING_ELT(names,18,mkChar("AlphaVec"));
	SET_STRING_ELT(names,19,mkChar("SpVec"));
	SET_STRING_ELT(names,20,mkChar("MinNitroVec"));
	SET_STRING_ELT(names,21,mkChar("RespVec"));
	SET_STRING_ELT(names,22,mkChar("SoilEvaporation"));
	SET_STRING_ELT(names,23,mkChar("cwsMat"));
	SET_STRING_ELT(names,24,mkChar("psimMat"));
	SET_STRING_ELT(names,25,mkChar("rdMat"));
	SET_STRING_ELT(names,26,mkChar("SCpools"));
	SET_STRING_ELT(names,27,mkChar("SNpools"));
	SET_STRING_ELT(names,28,mkChar("LeafPsimVec"));
  SET_STRING_ELT(names,29,mkChar("DayafterPlanting"));
  SET_STRING_ELT(names,30,mkChar("GDD"));
  SET_STRING_ELT(names,31,mkChar("GPP"));
  SET_STRING_ELT(names,32,mkChar("NPP"));
  SET_STRING_ELT(names,33,mkChar("autoRESP"));
  SET_STRING_ELT(names,34,mkChar("hetRESP"));
  SET_STRING_ELT(names,35,mkChar("NER"));
  SET_STRING_ELT(names,36,mkChar("StemMResp"));
  SET_STRING_ELT(names,37,mkChar("RootMResp"));
  SET_STRING_ELT(names,38,mkChar("RhizomeMResp"));
  SET_STRING_ELT(names,39,mkChar("LeafDarkResp"));
  SET_STRING_ELT(names,40,mkChar("Stemd"));
  SET_STRING_ELT(names,41,mkChar("Leafd"));
  SET_STRING_ELT(names,42,mkChar("Rootd"));
  SET_STRING_ELT(names,43,mkChar("Rhizomed"));
  SET_STRING_ELT(names,44,mkChar("Stemlitterd"));
  SET_STRING_ELT(names,45,mkChar("Leaflitterd"));
  SET_STRING_ELT(names,46,mkChar("Rootlitterd"));
  SET_STRING_ELT(names,47,mkChar("Rhizomelitterd"));
  SET_STRING_ELT(names,48,mkChar("LAId"));
	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(51);
	return(lists);
}

