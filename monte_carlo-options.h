#define EPS 0.0000001

typedef struct params
{
    int FinalIndex;
    int mode;                       // 0 for horizontal, 1 for vertical
    double separation;              // (shortest) separation between the scintillators. In cm
    long long unsigned int seed;    // seed for rng
    int StoreInter;                 // index for storing the data
    
    // Derived parameters  
    double width;                   // scintillator width
    double depth;                   // scintillator depth
    double height;                  // scintillator height
    double max_theta;               // maximum angle for a coincident hit. Depends on geometry

} params;

typedef struct ray
{

} ray;