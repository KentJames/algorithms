// This is adapted from D.S.Sivia's book:
//  Data Analysis: A Bayesian Tutorial
// Chapter 9 - Nested Sampling

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifndef VERBOSE_DEBUG_MESSAGES
#define VERBOSE_DEBUG_MESSAGES ( 0 )
#endif

#define N_DIM ( 2 ) // Dimensionality of the likelihood space
#define N ( 200 ) // Number of atoms
#define MAX ( 2000 ) // Max number of iterations

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
#define MODE_SHIFT ( 17.6 ) // Used for a second mode in multimodal model.

double _model( nestedObject *object ); // Basic gaussian model
double _multimodal_model( nestedObject *object ); // Bi-model gaussian model
#define LIKELIHOOD( object ) ( _multimodal_model( object ) );

#define PRIOR_BOX ( 30.0 )
#define PRIOR_MIN ( MODEL_MEAN - PRIOR_BOX )
#define PRIOR_MAX ( MODEL_MEAN + PRIOR_BOX )
void Prior(nestedObject *object); // Sets up an object within our prior boundaries

// For re-nesting an object after it has been killed.
void Explore(nestedObject *object,  // Object we want to replace 
	     nestedObject *objects, // Objects 
	     double logLstar); // The new log likelihood limit.

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
	//fprintf( stderr, "nest:%d logwidth:%g\n", nest, logwidth );
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
    printf( "Final posterior sample: \n" );
    for(int i = 0; i < N_DIM; ++i) printf( "Coord %d:%g\n", i, samples[nest].x[i] );
    printf( "logL:%g +- %g\n", samples[nest].log_likelihood, sqrt( H / N ) );
    return 0;
}

// Basic gaussian model
double _model(nestedObject *object)
{
    double exponent = 0.0;
    for( int i = 0; i < N_DIM; ++i )
    {
        double coord = object->x[i];
	exponent -= ( pow( ( coord - MODEL_MEAN ) / MODEL_SIGMA , 2 ) );
    }
    exponent *= (double)( N_DIM / 2 );
    return 1 + log( exp( exponent ) );
}

// Slightly more elaborate multi-model gaussian model
double _multimodal_model(nestedObject *object)
{
    double exponent1 = 0.0;
    double exponent2 = 0.0;
    // Mode 1
    for( int i = 0; i < N_DIM; ++i )
    {
        double coord = object->x[i];
	exponent1 -= ( pow( ( coord - MODEL_MEAN ) / MODEL_SIGMA, 2 ) );
    }
    exponent1 *= (double)( N_DIM / 2 );

    // Mode 2
    for( int i = 0; i < N_DIM; ++i )
    {
	double coord = object->x[i];
	exponent2 -= ( pow( ( coord - MODEL_MEAN - MODE_SHIFT  ) / MODEL_SIGMA, 2 ) );
    }
    exponent2 *= (double)( N_DIM / 2 );

    return 1 + log( exp ( exponent1 ) + 1.1 * exp( exponent2 ) );
}

void Prior(nestedObject *object)
{
    for(int i = 0; i < N_DIM; ++i)
    {
	object->x[i] = ( UNIFORM - 0.5 ) * ( 2 * PRIOR_BOX ) + MODEL_MEAN;// - ( 2 * PRIOR_BOX - MODEL_MEAN );
    }
    object->log_likelihood = LIKELIHOOD( object );
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
            double mixing_factor = UNIFORM;
	    double new_gene = mixing_factor * objects[parents[0]].x[i] +
		             ( 1 - mixing_factor ) * objects[parents[1]].x[i];
	    dummy.x[i] = new_gene;
	}
        evolvedLogL = LIKELIHOOD( &dummy );	
    }
    while( evolvedLogL < logLstar );
    dummy.log_likelihood = evolvedLogL;
    //    fprintf( stderr, "New logL:%g\n", evolvedLogL );
    dummy.logwidth = 0.0;
    *object = dummy;
}
