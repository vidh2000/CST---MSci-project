#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
#include <numeric>
#include <stdexcept>
#include <stdio.h>
#include <stack>
#include <string>
#include <vector>

#include "shapes.h"
#include "spacetimes.h"
#include "functions.h"
#include "vecfunctions.h"

using std::vector;

/*============================================================================
* ============================================================================
* GENERAL SPACETIME METHODS
* ============================================================================
============================================================================*/

Spacetime::Spacetime(){}

/**
 * @brief Internal function for the time sampling array for a cone from 
 * "origin point"(hence take t0=origin[0]) to time "t".
 */
vector<double> Spacetime::_T_slice_sampling(double t, 
                                            vector<double>origin,
                                            int samplingsize)
{
    vector<double> ts (samplingsize);
    double t_i = (t-origin[0])/samplingsize;
    for (int i = 0; i<samplingsize; i++)
        {ts[i] += origin[0] + t_i*i;}
    return ts;
}     


/**
 * @brief Simplest function of two events in 1D
 * 
 * @param xvec : vector<double>(1), time-coordinate of x
 * @param yvec : vector<double>(1), time-coordinate of y
 * @param period : needed for consistency with Causality, not used
 * @return vector<bool> : {1, x<=y, x>y}
 */
vector<bool> Spacetime::causal1d(vector<double> xvec, vector<double> yvec,
                                 vector<double> period)
{
    double t_delta = yvec[0] - xvec[0];
    return {1, t_delta >= 0.0, t_delta < 0.0};
}

/**
 * @brief Return most simple causal function: t_2>t_1, useful in 1D cases. 
 */
typedef vector<bool> (*func)
(vector<double> xvec, vector<double> yvec, vector<double> period);
func Spacetime::Causality()
    {return &Spacetime::causal1d;}


Spacetime::~Spacetime(){}




/*============================================================================
* ============================================================================
* FLAT SPACETIME
* ============================================================================
============================================================================*/
/**
 * @brief Construct a new Flat Spacetime:: Flat Spacetime object
 * 
 * @param dim int:   dimension of flat spacetime. Default 4.
 * @param period vector<double>:   periodicity vector: ith entry is period 
 * along ith SPATIAL diemnsion (0->x, 1->y, 2->z). Default is no periodicty.
 * No periodicity along ith direction requires 0. Usually, but not necessarily,
 * with cuboid might want period[i]=shape_object.Edges()[i].
 */
FlatSpacetime::FlatSpacetime(int dim, vector<double> period) : Spacetime()
{
    if (dim < 1)
        {throw std::invalid_argument("Dimension has to be at least 1.");}

    _dim = dim;
    _name = "flat";
    _metricname = "Minkowski";
    if (period.size()) 
    {
        _isPeriodic = true;
        _period = period;
        for (int i=0; i<period.size(); i++)
        {
            if (period[i]<0) {_period[i]=-period[i];}
        }
    }
}


/**
 * @brief Spacetime Interval Squared t squared - x_i*x^i
 */
double FlatSpacetime::ds2(vector<double> xvec, vector<double> yvec)
{
    
    double time_delta2  = (yvec[0] - xvec[0])*(yvec[0] - xvec[0]);
    double space_delta2 = 0;
    for (int i = 1; i<_dim; i++)
        {space_delta2 += (yvec[i]-xvec[i])*(yvec[i]-xvec[i]);}
    return time_delta2 - space_delta2;
}


/**
 * @brief Spacetime Interval sqrt(t squared - x_i*x^i)
 */
double FlatSpacetime::ds(vector<double> xvec, vector<double> yvec)
{
    
    double time_delta2  = (yvec[0] - xvec[0])*(yvec[0] - xvec[0]);
    double space_delta2 = 0;
    for (int i = 1; i<_dim; i++)
        {space_delta2 += (yvec[i]-xvec[i])*(yvec[i]-xvec[i]);}
    return std::sqrt(time_delta2 - space_delta2);
}


/**
 * @brief Returns causality function based on spacetime parameters
 * 
 * @return "{bool1, bool2, bool3} &callable (vector<double> xvec, yvec)" :
 * i.e. a callable that takes coordinates of 2 points and returns vector<bool>
 *        {x-y timelike, x<=y, x>y}
 */    
typedef vector<bool> (*func)
(vector<double> xvec, vector<double> yvec, vector<double> period);
func FlatSpacetime::Causality()
{
    if (_dim == 1)
        {return &Spacetime::causal1d;}
    else if (!_isPeriodic)
        {return &FlatSpacetime::causal;}
    else
        {return &FlatSpacetime::causal_periodic;}
}

/**
 * @brief Function of two events in any D returning {x-y timelike?, x<=y, x>y}.
 * 
 * @param xvec vector<double>:  coordinates of x
 * @param yvec vector<double>:  coordinates of y
 * @param period not used, needed for consistency with Causality
 * @return vector<bool> : {x-y timelike, x<=y, x>y}
 */
vector<bool> FlatSpacetime::causal(vector<double> xvec, vector<double> yvec,
                                    vector<double>period)
{
    double t_delta = (yvec[0] - xvec[0]);
    double t_delta2 = t_delta*t_delta;
    double space_delta2 = 0;
    for (int i = 1; i<yvec.size(); i++){
        double space_delta_i = (yvec[i]-xvec[i]);
        space_delta2 += space_delta_i*space_delta_i;}
    bool isCausal = t_delta2 >= space_delta2;
    return {isCausal,
            (t_delta >= 0.0) && isCausal,
            (t_delta < 0.0) && isCausal
           };
}

/**
 * @brief Function of two events in any D returning {x-y timelike?, x<=y, x>y}.
 * 
 * @param xvec : vector<double>, coordinates of x
 * @param yvec : vector<double>, coordinates of y
 * @param period : vector<double>, ith entry is periodicty along ith SPATIAL
 * dimension (0->x, 1->y, 2-->z)
 * @return vector<bool> : {x-y timelike, x<=y, x>y}
 */
vector<bool> FlatSpacetime::causal_periodic(vector<double> xvec, 
                                            vector<double> yvec,
                                            vector<double> period)
{
    double t_delta = (yvec[0] - xvec[0]);
    double t_delta2 = t_delta*t_delta;
    double space_delta2 = 0;
    for (int i = 1; i<yvec.size(); i++)
    {
        double space_delta_i = (yvec[i]-xvec[i]);
        double wrapped_space_delta_i = (space_delta_i>0)?
                                        period[i]-space_delta_i:
                                        period[i]+space_delta_i; 
        space_delta2 += (space_delta_i<wrapped_space_delta_i)?
                        space_delta_i*space_delta_i :
                        wrapped_space_delta_i*wrapped_space_delta_i;
    }
    bool isCausal = t_delta2 >= space_delta2;
    return {isCausal,
            (t_delta >= 0.0) && isCausal,
            (t_delta < 0.0) && isCausal
           };
}

FlatSpacetime::~FlatSpacetime(){}




/*============================================================================
* ============================================================================
* BLACK HOLES SPACETIME
* Notes: does not support anything actually at the moment
* ============================================================================
============================================================================*/
BlackHoleSpacetime::BlackHoleSpacetime(int dim,// = 2,
                            double r_S,// = 0.5,
                            const char* metric)// = "Eddington-Finkelstein")
                            : Spacetime()
/**
 * @brief Initialises a Black Hole Spacetime.
 *
 * @param dim: dimension of spacetime. Default 2.
 * @param r_S: Schwarzschild radius. Default 0.5
 * @param metric: Specify metric: either "Eddington-Finkelstein" or "EF"
 *                (default), or "Schwarzschild" or "S".
 */
{
    if (dim < 2 || dim > 4)
    {
        throw std::invalid_argument("Dimension has to be 2, 3 or 4.");
    }
    _dim = dim;
    _name = "black hole";

    if (metric == "Eddington-Finkelstein" || metric == "EF")
        {_metricname = "Eddington-Finkelstein";}
    else if (metric == "Schwarzschild" || metric == "S")
        {_metricname = "Schwarzschild";}
}


typedef std::vector<bool> (*func)
        (std::vector<double> xvec, std::vector<double> yvec, 
        std::vector<double> period);
        func BlackHoleSpacetime::Causality()
{

} 

BlackHoleSpacetime::~BlackHoleSpacetime(){}





int main(){
std::cout << "spacetime.cpp WORKS! :)";
return 0;

};
