/* 
 *  This file defines the hearing prescription for each user.  Put each
 *  user's prescription in the "dsl" (for multi-band parameters) and
 *  "gha" (for broadband limiter at the end) below.  
 */


/*  From chapro.h
typedef struct {
    double attack;               // attack time (ms)
    double release;              // release time (ms)
    double maxdB;                // maximum signal (dB SPL)
    int32_t ear;                 // 0=left, 1=right
    int32_t nchannel;            // number of channels
    double cross_freq[DSL_MXCH]; // cross frequencies (Hz)
    double tkgain[DSL_MXCH];     // compression-start gain
    double cr[DSL_MXCH];         // compression ratio
    double tk[DSL_MXCH];         // compression-start kneepoint
    double bolt[DSL_MXCH];       // broadband output limiting threshold
} CHA_DSL;
*/


// Here is the per-band prescription that is the default behavior of the multi-band WDRC...must be in same order as CHA_DSL definition in chapro.h
CHA_DSL dsl = { 5,  // attack (ms)
  50,  // release (ms)
  119,  //maxdB.  calibration.  dB SPL for input signal at 0 dBFS.  Needs to be tailored to mic, spkrs, and mic gain.
  0,    // 0=left, 1=right...ignored
  8,    //num channels...ignored.  8 is always assumed
  {317.1666, 502.9734, 797.6319, 1264.9, 2005.9, 3181.1, 5044.7},        // cross frequencies (Hz)...FOR IIR FILTERING, THESE VALUES ARE IGNORED!!!
  {-13.5942, -16.5909, -3.7978, 6.6176, 11.3050, 23.7183, 25.0, 25.0},   // compression-start gain
  {0.7, 0.9, 1, 1.1, 1.2, 1.4, 1.6, 1.7},   // compression ratio
  {32.2, 26.5, 26.7, 26.7, 29.8, 33.6, 34.3, 32.7},   // compression-start kneepoint (input dB SPL)
  {78.7667, 88.2, 90.7, 92.8333, 98.2, 103.3, 101.9, 99.8}    // output limiting threshold (comp ratio 10)
};



/* From chapro.h 
typedef struct {
    double attack;          // attack time (ms)
    double release;         // release time (ms)
    double fs;              // sampling rate (Hz)
    double maxdB;           // maximum signal (dB SPL)
    double tkgain;          // compression-start gain
    double tk;              // compression-start kneepoint
    double cr;              // compression ratio
    double bolt;            // broadband output limiting threshold
    // processing parameters
    double td;              // target delay
    int32_t nz;             // filter order
    int32_t nw;             // window size
    int32_t wt;             // window type: 0=Hamming, 1=Blackman
} CHA_WDRC;
*/

// Here are the settings for the broadband limiter at the end....must be in same order as CHA_WDRC definition in chapro.h
CHA_WDRC gha = {1.f, // attack time (ms)
  50.f,    // release time (ms)
  24000.f,  // sampling rate (Hz)...ignored?  Isn't this set globally in the main program?
  119.f,    // maxdB.  calibration.  dB SPL for signal at 0dBFS.  Needs to be tailored to mic, spkrs, and mic gain.
  0.f,      // compression-start gain....set to zero for pure limitter
  105.f,    // compression-start kneepoint...set to some high value to make it not relevant
  10.f,     // compression ratio...set to 1.0 to make linear (to defeat)
  105.0     // output limiting threshold...hardwired to compression ratio of 10.0
};

/* From chapro.h 
typedef struct
{
    // simulation parameters
    double fbg;                  // simulated-feedback gain
    // AFC parameters
    double   rho;                // forgetting factor
    double   eps;                // power threshold
    double    mu;                // step size
    double   alf;                // band-limit update
    int32_t  afl;                // adaptive-filter length
    int32_t  wfl;                // whiten-filter length
    int32_t  pfl;                // band-limit-filter length
    int32_t  fbl;                // simulated-feedback length
    int32_t  hdel;               // output/input hardware delay
    int32_t  pup;                // band-limit update period
    // feedback filter buffers
    float   *efbp;               // estimated-feedback buffer pointer
    float   *sfbp;               // simulated-feedback buffer pointer
    float   *wfrp;               // whiten-feedback buffer pointer
    float   *ffrp;               // persistent-feedback buffer pointer
    // quality metric buffers & parameters
    float *qm;                   // quality-metric buffer pointer
    int32_t *iqmp;               // quality-metric index pointer
    int32_t  nqm;                // quality-metric buffer size
    int32_t  iqm;                // quality-metric index
    int32_t  sqm;                // save quality metric ?
    CHA_PTR  pcp;                // previous CHA_PTR
} CHA_AFC;
*/

// Here are the settings for the adaptive feedback cancelation...must be in same order as CHA_AFC definition in chapro.h
CHA_AFC afc = {  
  0.0,  //simulated-feedback gain
  0.0072189585,     //rho, forgetting factor
  0.000919300,      //eps, power threshold
  0.004607254,      //mu, step size
  0.000010658,      //alf, band-limit update
  42,               //afl, adaptive filter length
  9,                //wfl, whiten filter length
  20,               //pfl, band-limit filter length
  0,                //fbl, simulated-feedback length  [IS ZERO CORRECT?]
  38 + 2*chunk,     //hdel, output/input hardware delay..."chunk' was defined earlier in the main code or else this wouldn't work
  8                 //pup, band-limit update period
};  //the rest of the parameters are assumed zero and will be set by the rest of the code
