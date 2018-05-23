#ifndef _METEOR_DEMOD_H
#define _METEOR_DEMOD_H

#include <pthread.h>
#include "agc.h"
#include "pll.h"
#include "sample.h"

#define COSTAS_BW 100
#define COSTAS_DAMP 1/M_SQRT2
#define COSTAS_INIT_FREQ -0.005

#define AGC_WINSIZE 1024*32
#define AGC_TARGET 180

#define RRC_ALPHA 0.6
#define RRC_FIR_ORDER 32

#define CHUNKSIZE 32768
#define SYM_CHUNKSIZE 1024

typedef struct {
	Agc *agc;
	Sample *interp, *src;
	Costas *cst;
	float sym_period;
	unsigned sym_rate;
	pthread_t t;

	pthread_mutex_t mutex;
	unsigned bytes_out_count;
	int thr_is_running;
	char out_buf[SYM_CHUNKSIZE];
} Demod;

Demod*   demod_init(Sample *src, unsigned interp_factor, float pll_bw, unsigned sym_rate);
void     demod_start(Demod *self, char *fname);
void     demod_join(Demod *self);

int      demod_status(Demod *self);
int      demod_is_pll_locked(Demod *self);
unsigned demod_get_bytes(Demod *self);
float    demod_get_perc(Demod *self);
float    demod_get_freq(Demod *self);
char*    demod_get_buf(Demod *self);

#endif