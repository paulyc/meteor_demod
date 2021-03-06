#include <math.h>
#include <stdlib.h>
#include "filters.h"
#include "interpolator.h"
#include "utils.h"

static int interp_read(Sample *self, size_t count);
static int interp_free(Sample *self);

typedef struct {
	Sample *src;
	Filter *rrc;
	unsigned factor;
} InterpState;

/* Initialize the interpolator, which will use a RRC filter at its core */
Sample*
interp_init(Sample* src, float alpha, unsigned order, unsigned factor, int sym_rate)
{
	Sample *interp;
	InterpState *status;

	interp = safealloc(sizeof(*interp));

	interp->count = 0;
	interp->samplerate = src->samplerate * factor;
	interp->data = NULL;
	interp->bps = sizeof(*src->data);
	interp->read = interp_read;
	interp->close = interp_free;

	interp->_backend = safealloc(sizeof(InterpState));
	status = (InterpState*) interp->_backend;

	status->factor = factor;
	status->src = src;
	status->rrc = filter_rrc(order, factor, src->samplerate/(float)sym_rate, alpha);

	return interp;
}

/* Wrapper to interpolate the source data and provide a transparent translation
 * layer between the raw samples and the interpolated samples */
int
interp_read(Sample *const self, size_t count)
{
	InterpState *status;
	Filter *rrc;
	Sample *src;
	int i;
	int factor;
	size_t true_samp_count;

	/* Retrieve the backend info */
	status = (InterpState*)self->_backend;
	factor = status->factor;
	src = status->src;
	rrc = status->rrc;

	/* Prepare a buffer for the data that will go into the Sample struct */
	if (!self->data) {
		self->data = safealloc(sizeof(*self->data) * count);
	} else if (self->count < count) {
		free(self->data);
		self->data = malloc(sizeof(*self->data) * count);
	}

	self->count = count;
	true_samp_count = count / factor;

	/* Read the true samples from the associated source */
	true_samp_count = src->read(src, true_samp_count);
	if (!true_samp_count) {
		return 0;
	}

	/* Feed through the filter, with zero-order hold interpolation */
	for (i=0; i<count; i++) {
		self->data[i] = filter_fwd(rrc, src->data[i/factor]);
	}

	return count;
}

/* Free the memory related to the interpolator. Note that this function does not
 * try to close the underlying data source (aka self->_backend->src) */
int
interp_free(Sample *self)
{
	filter_free(((InterpState*)(self->_backend))->rrc);
	free(self->_backend);
	free(self->data);
	free(self);
	return 0;
}

