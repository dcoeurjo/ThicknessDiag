#include <CGAL/Exact_spherical_kernel_3.h>
typedef CGAL::Exact_spherical_kernel_3 SK;

typedef typename SK::Point_3 Point_3;
typedef typename SK::Circle_3 Circle_3;
typedef typename SK::Sphere_3 Sphere_3;

#include <Sphere_intersecter.h>

// Test circles (ugly, but more reliable than a txt file)
static const double test_circles[][4] = {
  //  x-coordinate      y-coordinate      z-coordinate     radius
  {   9.31402785704,    -5.23914784193,   -3.99334762245,  1       },
  {   -6.51705032215,   -14.3935130723,   11.5965540335,   1       },
  {   11.9534488395,    -6.28729712218,   14.524808526,    1       },
  {   3.64744007587,    0.117365047336,   -16.3536215015,  1       },
  {   3.59117632732,    -14.7543815337,   -2.07664811984,  1       },
  {   -0.865544378757,  5.32359777018,    15.3772516735,   1       },
  {   -12.3223357461,   13.7477606535,    -5.82575645298,  1       },
  {   16.6228936613,    -9.92548415437,   -15.3824300133,  1       },
  {   -5.00400919467,   -10.0830822624,   7.92740387842,   1       },
  {   9.68020629138,    5.09431162849,    -11.3215409406,  1       },
  {   9.99815519899,    11.6347509809,    19.2416760139,   1       },
  {   -5.60883382335,   -17.6884189807,   17.5146878697,   1       },
  {   -16.5371762589,   10.8126061969,    8.97811995819,   1       },
  {   -4.65932559222,   -11.2244570814,   10.4076547921,   1       },
  {   -17.7942736074,   -10.9651816823,   -10.4932396486,  1       },
  {   10.3575928137,    4.64502023533,    -9.2214003019,   1       },
  {   -10.8870326355,   -18.4167507477,   -14.0205265395,  1       },
  {   14.3406458013,    -16.6309211031,   -5.28324091807,  1       },
  {   18.0565761589,    -0.065388455987,  18.7732614204,   1       },
  {   13.1514988281,    -5.43896608055,   -11.4662565663,  1       },
  {   8.29068737105,    -12.9200565815,   1.80972436443,   1       },
  {   17.9741837271,    -13.7996560894,   -10.8680215478,  1       },
  {   11.7158008367,    -1.97658745572,   14.3081033044,   1       },
  {   18.7041032687,    12.6797280461,    -1.79838513955,  1       },
  {   3.93763545901,    -10.4245608859,   -14.8691705242,  1       },
  {   -8.15609045327,   18.847652059,     17.2368730418,   1       },
  {   -3.66384696215,   9.06294062734,    9.23207817599,   1       },
  {   4.04761420563,    7.57914211601,    5.08935237303,   1       },
  {   1.39939080924,    10.8373140916,    14.8213875666,   1       },
  {   6.86834981665,    9.54069811851,    13.2523965463,   1       },
  {   7.36580371857,    2.46858267114,    -2.29403451085,  1       },
  {   1.42926525325,    -1.30210952833,   6.29716956988,   1       },
  {   10.7578803413,    9.27194783464,    15.8768698946,   1       },
  {   3.65109981969,    -4.25296330824,   5.85765894502,   1       },
  {   16.678858269,     6.32079903036,    -7.24418895319,  1       },
  {   -18.6659564637,   -5.96483308822,   -13.711614497,   1       },
  {   -13.2165719382,   -18.6419534124,   -8.37911430746,  1       },
  {   -16.194446478,    -11.3347002864,   -1.40483831987,  1       },
  {   -6.37762349099,   -19.974544663,    -3.41888342053,  1       },
  {   3.00985902548,    9.36686245725,    -13.5341975465,  1       },
  {   7.60018318892,    -13.4115307406,   7.07123285159,   1       },
  {   11.8289498053,    11.3439021818,    4.20425571501,   1       },
  {   -0.926353558898,  8.84182997048,    -16.8103108928,  1       },
  {   1.08890539035,    5.69654658437,    11.0981815495,   1       },
  {   -16.8413227983,   5.13910405338,    -11.6955160536,  1       },
  {   1.11948529258,    -7.85601384938,   -19.7734279558,  1       },
  {   -11.3985378109,   -5.50295267254,   3.20290083066,   1       },
  {   -13.1010378338,   11.660061609,     1.94056591019,   1       },
  {   14.6092598513,    -12.516454123,    12.6628570817,   1       },
  {   -12.2697565705,   5.26535313576,    -16.7728250101,  1       },
  {   10.8251555078,    -13.8943130709,   10.6751088053,   1       },
  {   13.0583882518,    -12.3975196294,   3.89220146462,   1       },
  {   15.670164004,     -14.8276754282,   14.6471222676,   1       },
  {   5.45369006693,    0.0541345961392,  -11.0248118825,  1       },
  {   17.0651314594,    -14.3866817653,   -18.2290148735,  1       },
  {   19.9485570565,    -1.9538227655,    14.0358054824,   1       },
  {   17.0839494467,    -18.0045660585,   2.33929261565,   1       },
  {   5.83048539236,    10.81095092,      -4.07235713676,  1       },
  {   -11.6213420779,   -8.01728868857,   -14.8554930091,  1       },
  {   12.6574828103,    -10.7312376052,   16.4019326679,   1       },
  {   -14.7439243831,   14.746911414,     9.19885087758,   1       },
  {   -3.32363773137,   8.81830848753,    -0.6397004053,   1       },
  {   16.0746589117,    18.3414732106,    -6.13664291799,  1       },
  {   -13.0116116442,   -1.29037067294,   -5.19781094044,  1       },
  {   -3.83064689115,   -17.1897812374,   16.076261159,    1       },
  {   -17.2791789658,   8.30598514527,    -6.51080826297,  1       },
  {   -18.1987380609,   -13.6901398003,   -1.27751596272,  1       },
  {   -6.87303554267,   -7.07613427192,   6.07504339889,   1       },
  {   15.6680242717,    -4.21682672575,   8.28751571476,   1       },
  {   -18.0486093834,   8.07315394282,    8.92664207146,   1       }
};
// and the size of this array
static const std::size_t test_circles_size = sizeof(test_circles) /sizeof(double[4]);

int main(int argc, const char * argv[])
{
  Sphere_intersecter<SK> si;
  for (std::size_t i = 0; i < test_circles_size; i++)
  {
    Point_3 coord(test_circles[i][0],
        test_circles[i][1], test_circles[i][2]);
    double radius = test_circles[i][3];
    // TODO
  }
  return 0;
}

// vim: sw=2 et ts=2 sts=2