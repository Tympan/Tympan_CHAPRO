
#ifndef _test_gha_h
#define _test_gha_h

// tst_gha.c - test IIR-filterbank + AGC + NFC
//              with WAV file input & ARSC output


#include <chapro.h>
#define MAX_MSG 256

typedef struct
{
    char *ifn, *ofn, *dfn, mat, nrep;
    double rate;
    float *iwav, *owav;
    int32_t cs;
    int32_t *siz;
    int32_t iod, nwav, nsmp, mseg, nseg, oseg, pseg;
    void **out;
} I_O;

/***********************************************************/

//static char msg[MAX_MSG] = {0};
static double srate = 24000; // sampling rate (Hz)
static int chunk = 8;        // chunk size   (WEA: This was 32.  I switched to 8 to lower the system's latency.)
static int prepared = 0;

// ////////////// Old method
//static CHA_AFC afc = {0};
//static CHA_DSL dsl = {0};
//static CHA_WDRC agc = {0};

// ///////////// New method
#include "GHA_Constants.h" //put all prescription info in this header file
CHA_WDRC agc = gha; //GHA_Constants defines this as "gha", but we actually want "agc"

/***********************************************************/

static void
process_chunk(CHA_PTR cp, float *x, float *y, int cs)
{
    if (prepared)
    {
        // next line switches to compiled data
        //cp = (CHA_PTR) cha_data;
        float *z = CHA_CB;
        //cha_afc_filters(cp, &afc); Steve said to dissble, email 10/6/2021
        // process IIR+AGC+AFC
        cha_afc_input(cp, x, x, cs); 
        cha_agc_input(cp, x, x, cs);
        cha_iirfb_analyze(cp, x, z, cs);
        cha_agc_channel(cp, z, z, cs);
        cha_iirfb_synthesize(cp, z, y, cs);
        cha_agc_output(cp, y, y, cs);
        cha_afc_output(cp, y, cs); 
    }
}

// prepare input/output

static int
prepare_io(I_O *io)
{
    // initialize waveform
    io->rate = srate;
    io->cs = chunk;
//    if (init_wav(io, msg))
//    {
//        return (1);
//    }
//    // prepare i/o
//    if (!io->ofn)
//    {
//        init_aud(io);
//    }
//    printf("%s", msg);
//    printf(" prepare_io: sr=%.0f cs=%d ns=%d\n", io->rate, io->cs, io->nsmp);
    return (0);
}

// prepare IIR filterbank
static void
prepare_filterbank(CHA_PTR cp)
{
    double td, sr, *cf;
    int cs, nc, nz;
    // zeros, poles, gains, & delays
    float z[64], p[64], g[8];
    int d[8];

    sr = srate;
    cs = chunk;
    // prepare IIRFB
    nc = dsl.nchannel;
    cf = dsl.cross_freq;
    nz = agc.nz;
    td = agc.td;
    printf("test_gha: prepare_filterbank: sr=%0.0f, cs=%d, nc=%d, nz=%d, td=%0.2f\n",sr,cs,nc,nz,td);  //added WEA
    cha_iirfb_design(z, p, g, d, cf, nc, nz, sr, td); //see iirfb_design.c
    cha_iirfb_prepare(cp, z, p, g, d, nc, nz, sr, cs);
    printf("test_gha: prepare_filterbank complete.\n");  // added WEA
}

// prepare AGC compressor

static void
prepare_compressor(CHA_PTR cp)
{
    // prepare AGC
    cha_agc_prepare(cp, &dsl, &agc);
}

// prepare feedback

static void
prepare_feedback(CHA_PTR cp)
{
    // prepare AFC
    cha_afc_prepare(cp, &afc);
}

// prepare signal processing

static void
prepare(I_O *io, CHA_PTR cp)
{
    prepare_io(io);
    srate = io->rate;
    chunk = io->cs;
    prepare_filterbank(cp);
    prepare_compressor(cp);
    if (afc.sqm)
        afc.nqm = io->nsmp * io->nrep;      
    prepare_feedback(cp);
    prepared++;
    // generate C code from prepared data
    //cha_data_gen(cp, DATA_HDR);

    //printf("test_gha.h: prepare:((int *)cp[_ivar])[_in1] = %i, ((int *)cp[_ivar])[_in2] = %i\n",((int *)cp[_ivar])[_in1],((int *)cp[_ivar])[_in2]);
}

/***********************************************************/

static void
configure_compressor()
{
    // DSL prescription example
    /*
    static CHA_DSL dsl_ex = {5, 50, 119, 0, 8, 
        {317.1666, 502.9734, 797.6319, 1264.9, 2005.9, 3181.1, 5044.7}, 
        {-13.5942, -16.5909, -3.7978, 6.6176, 11.3050, 23.7183, 25.0, 25.0},   // last two values were 35.8586, 37.3885
        {0.7, 0.9, 1, 1.1, 1.2, 1.4, 1.6, 1.7}, 
        {32.2, 26.5, 26.7, 26.7, 29.8, 33.6, 34.3, 32.7}, 
        {78.7667, 88.2, 90.7, 92.8333, 98.2, 103.3, 101.9, 99.8}
        };
    static CHA_WDRC agc_ex = {1, 50, 24000, 119, 0, 105, 10, 105};
    */
    static int nz = 4;
    static double td = 2.5;

    //memcpy(&dsl, &dsl_ex, sizeof(CHA_DSL));
    //memcpy(&agc, &agc_ex, sizeof(CHA_WDRC));
    agc.nz = nz;
    agc.td = td;
}

static void
configure_feedback()
{
//  switch (2) {
//    case 1:
//      // AFC parameters...original plus update on 10/9/2021...As of 11/17, WEA thinks that this sounds better than the settings on 11/9, below
//      afc.afl = 45;           // adaptive filter length...was 45
//      afc.wfl = 5;            // whiten-filter length...originally 9, but Steve suggested 5 along with pfl of 36 in email 10/9/2021
//      afc.pfl = 36;           // band-limit-filter length...originally 0, but Steve suggested 36 with wfl of 9 in email 10/9/2021
//      afc.rho = 0.002577405;  // forgetting factor   (WEA: optimized for pfl=23??)
//      afc.eps = 0.000008689;  // power threshold   (WEA: optimized for pfl=23??)
//      afc.mu = 0.000050519;   // step size   (WEA: optimized for pfl=23??)
//      afc.alf = 0.000001825;  // band-limit update   (WEA: optimized for pfl=23??)
//      afc.pup = 1;            // band-limit update period
//      break;    
//    case 2:
//      // AFC parameters...per email from Steve on 11/9/2021
//      afc.afl  = 42;          // adaptive filter length
//      afc.wfl  = 9;           // whiten filter length
//      afc.pfl  = 20;          // band-limit filter length
//      afc.rho  = 0.007218985; // forgetting factor 
//      afc.eps  = 0.000919300; // power threshold
//      afc.mu   = 0.004607254; // step size
//      afc.alf  = 0.000010658; // band-limit update
//      afc.pup  = 8;           // band-limit update period 
//      break;
//    default:
//      Serial.println("test_gha: configure_feedback: *** ERROR ***: AFC is not configured.");
//  }
  
  //afc.hdel = 0; // output/input hardware delay
//  afc.hdel = 38 + 2*chunk; // output/input hardware delay.  Tympan has a hardware delay of 17+21 = 38 samples plus the I2S buffering is 2 block sizes
  
  afc.sqm = 0;  // save quality metric ?
  //afc.fbg = 1;  // simulated-feedback gain
  afc.nqm = 0;  // initialize quality-metric length
  //if (!args.simfb)
      afc.fbg = 0;  //zero synthetic feedback
}

static void
configure(I_O *io)
{
//    static char *ifn = "test/carrots.wav";
//    static char *wfn = "test/tst_gha.wav";
//    static char *mfn = "test/tst_gha.mat";

    // initialize CHAPRO variables
    configure_compressor();
    configure_feedback();
    // initialize I/O
#ifdef ARSCLIB_H
    io->iod = ar_find_dev(ARSC_PREF_SYNC); // find preferred audio device
#endif                                     // ARSCLIB_H
    io->iwav = NULL;
    io->owav = NULL;
//    io->ifn = args.ifn ? args.ifn : ifn;
//    io->ofn = args.play ? args.ofn : wfn;
//    io->dfn = mfn;
//    io->mat = args.mat;
//    io->nrep = (args.nrep < 1) ? 1 : args.nrep;
}

//static void
//report()
//{
//    char *en, *fc;
//    int nc, nz;
//
//    // report
//    fc = args.afc ? "+AFC" : "";
//    en = args.simfb ? "en" : "dis";
//    nc = dsl.nchannel;
//    nz = agc.nz;
//    printf("CHA simulation: feedback simulation %sabled.\n", en);
//    printf("IIR+AGC%s: nc=%d nz=%d\n", fc, nc, nz);
//}

/***********************************************************/

//int main(int ac, char *av[])
//{
//   static void *cp[NPTR] = {0};  //create this as a global
//   static I_O io;                //create this as a global
//
//    parse_args(ac, av);
//    configure(&io);
//    report();
//    prepare(&io, cp);
//    process(&io, cp);
//    cleanup(&io, cp);
//    return (0);
//}

#endif
