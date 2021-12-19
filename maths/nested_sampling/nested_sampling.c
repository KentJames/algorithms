// This is adapted from D.S.Sivia's book:
//  Data Analysis: A Bayesian Tutorial
// Chapter 9 - Nested Sampling

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define VERBOSE_DEBUG_MESSAGES ( 1 )

#define N_DIM ( 2 ) // Dimensionality of the likelihood space
#define N ( 100 ) // Number of atoms
#define MAX ( 1000 ) // Max number of iterations

#define UNIFORM ( ( rand() + 0.5 ) / ( RAND_MAX + 1.0 ) ) // Defined on domain (0,1) 
#define PLUS(x,y) ( x > y ? x + log( 1 + exp( y - x ) ) : y + log( 1 + exp( x - y ) ) )

// Object within an n-dimensional space
typedef struct nestedObject {
    double x[N_DIM]; // Pointer to an array with num_dim elements.
    double log_likelihood;
    double logwidth;
} nestedObject;

// Functions encapsulating knowledge of our model, prior and results
#define MODEL_MEAN ( 20.0 ) // Mean the same in all dimensions
#define MODEL_SIGMA ( 8.0 )  // Standard deviation the same in all dimensions
double _model(nestedObject *object)
{
    double normalisation_constant = 1.0 / ( sqrt( 2 * M_PI ) * MODEL_SIGMA );
    double exponent = 0.0;
    for( int i = 0; i < N_DIM; ++i )
    {
        double coord = object->x[i];
	exponent -= ( pow( ( coord - MODEL_MEAN ) / MODEL_SIGMA , 2 ) );
    }
    exponent *= (double)( N_DIM / 2 );
    return log( exp( exponent ) );
}

#define PRIOR_BOX ( 10.0 )
#define PRIOR_MIN ( MODEL_MEAN - PRIOR_BOX )
#define PRIOR_MAX ( MODEL_MEAN + PRIOR_BOX )
void Prior(nestedObject *object)
{
    for(int i = 0; i < N_DIM; ++i)
    {
	double randv = UNIFORM;
	fprintf( stderr, "UNIFORM:%g\n", randv );
	object->x[i] = ( randv - 0.5 ) * ( 2 * PRIOR_BOX ) + MODEL_MEAN;// - ( 2 * PRIOR_BOX - MODEL_MEAN );
    }
    object->log_likelihood = _model( object );
}

// For re-nesting an object after it has been killed.
// We use a very crude genetic algorithm to do this by mixing co-ords from two random objects.
void Explore(nestedObject *object,  // Object we want to replace 
	     nestedObject *objects, // Objects 
	     double logLstar)
{
    double evolvedLogL = 0.0;
    nestedObject dummy;
    do
    {
	int parents[2] = { 0 };
	parents[0] = (int)( N * UNIFORM ) % N;
	parents[1] = (int)( N * UNIFORM ) % N;

	if( parents[0] == parents[1] ) continue; // We can not reproduce asexually.

    	for( int i = 0; i < N_DIM; ++i )
	{
	    int gene_to_use = (int)( 2 * UNIFORM ) % 2;
	    dummy.x[i] = objects[parents[gene_to_use]].x[i];
	}
        evolvedLogL = _model( &dummy );
	
    }
    while( evolvedLogL < logLstar );
    dummy.log_likelihood = evolvedLogL;
    //    fprintf( stderr, "New logL:%g\n", evolvedLogL );
    dummy.logwidth = 0.0;
    *object = dummy;
}

int main(void)
{
    nestedObject objects[N];
    nestedObject samples[MAX];
    double logwidth; // ln(width in prior mass)
    double logLstar; // ln(Likelihood constraint)
    double H = 0.0; // Information (sometimes known as the negative log entropy)
    double logZ = -DBL_MAX; // ln(Evidence Z), initially 0
    double logZnew; // Updated logZ
    int i; // Object counter
    int copy; // Duplicated object
    int worst; // Worst object
    int nest; // Nested sampling iteration count
    fprintf( stderr, "Initialising priors on objects...");
    // Set prior objects
    for( i = 0; i < N; ++i )
    {
	Prior(&objects[i]);	    
    }
    fprintf( stderr, "done\n");

#if VERBOSE_DEBUG_MESSAGES
    for( i = 0; i < N; ++i )
    {
	fprintf( stderr, "Prior Object %d: %g %g logL:%g\n", i, objects[i].x[0], objects[i].x[1], objects[i].log_likelihood );
    }
#endif

    // Outermost interval of prior mass
    logwidth = log( 1.0 - exp( - 1.0 / N ) );

    // START - Nested Sampling Loop
    for( nest = 0; nest < MAX; ++nest )
    {
	fprintf( stderr, "nest:%d logwidth:%g\n", nest, logwidth );
	worst = 0;
	for( i = 1; i < N; ++i )
        {
	    if( objects[i].log_likelihood < objects[worst].log_likelihood ) {
		worst = i;
	    }
	}
	objects[worst].logwidth = logwidth + objects[worst].log_likelihood;
	// Update Evidence Z and Information H
	logZnew = PLUS( logZ, objects[worst].logwidth );
	H = exp( objects[worst].logwidth - logZnew ) * objects[worst].log_likelihood
	    + exp( logZ - logZnew ) * ( H + logZ ) - logZnew;
	logZ = logZnew;

	// Posterior samples (optional, not needed for optimising over the parameter space)
	samples[nest] = objects[worst];

	// Kill the worst object in favour of the copy of different survivor
	do {
	    copy = (int)( N * UNIFORM ) % N;
	}
	while( copy == worst && N > 1 );
	logLstar = objects[worst].log_likelihood;
	objects[worst] = objects[copy];

	// Evolve the copied object within our new constraint.
	Explore( &objects[worst], objects, logLstar );

	// Shrink the interval
	logwidth -= 1.0 / N;
    }
    // END - Nested Sampling Loop
#if VERBOSE_DEBUG_MESSAGES
    for( i = 0; i < N; ++i )
    {
	fprintf( stderr, "Post Nested Sampling Object %d: %g %g logL:%g\n", i, objects[i].x[0], objects[i].x[1], objects[i].log_likelihood );
    }
#endif

    printf( "# iterations = %d\n", nest );
    printf( "Evidence: ln(Z) = %g +- %g\n", logZ, sqrt( H / N ) );
    printf( "Information: H = %g nats = %g bits\n", H, H / log( 2. ) );
    return 0;
}
