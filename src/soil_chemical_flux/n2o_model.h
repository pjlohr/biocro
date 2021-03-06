/*****************************************************************************
**
**  FILE:    n2o_model.h
**
**  AUTHOR:  Melannie D. Hartman
**           Natural Resource Ecology Laboratory
**           Colorado State University
**           11/10/95, 2/13/97
**
*****************************************************************************/

#define COARSE 1           /* texture type for sandy soil */
#define MEDIUM 2           /* texture type for medium (loamy) soil */
#define FINE 3             /* texture type for fine soil */
#define VERYFINE 4         /* texture type for volcanic soil */

#define MIN_NH4_CONC 0.05  /* minimum NH4 concentration (ppm) */
#define MIN_NO3_CONC 0.05  /* minimum NO3 concentration (ppm) */



double nox_pulse(double *ppt, double *snow);

void getsoilprop(double *asand, double *asilt, double *aclay, double *bulkden,
                 double *fieldcap, int *texture,int *soilclassification);

double diffusiv(double *A, double *bulkden, double *wfps);

void leachdly(double tfluxout[], int numlyrs, double nitrate[], double critflow,
              double frlechd[], double stream[], double basef, double stormf,
              double *inorglch);



double f_allometric(double x, double A[]);

double f_arctangent(double x, double A[]);

double f_exponential(double x, double A[]);

double f_gen_gompertz(double x, double A[]);

double f_logistic(double x, double A[]);

double f_gen_poisson_density(double x, double A[]);
