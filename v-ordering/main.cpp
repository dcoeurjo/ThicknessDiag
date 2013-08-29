#include <vector>
#include <iterator>
#include <algorithm>
#include <CGAL/Exact_spherical_kernel_3.h>
#include <Sphere_intersecter.h>
#include <Event_queue.h>
#include <Event_queue_builder.h>

typedef CGAL::Exact_spherical_kernel_3 SK;

typedef SK::Point_3 Point_3;
typedef SK::Plane_3 Plane_3;
typedef SK::Circle_3 Circle_3;
typedef SK::Sphere_3 Sphere_3;
typedef SK::Direction_3 Direction_3;
typedef SK::Vector_3 Vector_3;
typedef SK::Line_3 Line_3;
typedef SK::Assign_3 Assign_3;
typedef SK::Object_3 Object_3;
typedef SK::Intersect_3 Intersect_3;
typedef SK::Circular_arc_3 Circular_arc_3;
typedef SK::Circular_arc_point_3 Circular_arc_point_3;

typedef Sphere_intersecter<SK>::Circle_handle Circle_handle;
typedef Sphere_intersecter<SK>::Sphere_handle Sphere_handle;

// Main sphere to work with
static const Sphere_3 test_sphere = Sphere_3(Point_3( 0, 0, 0), 3);

// Macro for squaring a number
#define SPHERE(x, y, z, r) Sphere_3(Point_3(x, y, z), r*r)

// Test circles
static const Sphere_3 test_spheres[] = {
  // polar circles
  SPHERE(3,                0,                 3,                3),
  // bipolar circles
  // TODO
  // normal circles
  SPHERE(2.70740620397,    0.0776660083161,   2.07546263732,    2.11372481705),
  SPHERE(3.70429426026,    0.869174586472,    1.26770523628,    1.81458985627),
  SPHERE(3.0927696761,     1.67927760515,     1.12789048956,    4.99157930731),
  SPHERE(4.04254050964,    0.567769003444,    4.69745254094,    4.20572801776),
  SPHERE(3.54695442437,    3.46294173367,     1.3289043281,     4.32310733506),
  SPHERE(4.57388628418,    3.31623761929,     3.92052272073,    3.82295225544),
  SPHERE(3.18647045421,    4.00560110596,     3.19868566467,    2.70184568541),
  SPHERE(2.44014694899,    2.79009871729,     3.05586321198,    2.13962598446),
  SPHERE(3.24711167484,    3.07995614486,     3.40328436888,    4.42671388723),
  SPHERE(2.25663658021,    1.28718097747,     1.77289934813,    3.17779466657),
  SPHERE(2.08058846114,    2.54107697706,     0.423323883018,   3.69803677234),
  SPHERE(1.22175037521,    1.86980967443,     4.15668870002,    0.847851253761),
  SPHERE(0.32567097039,    2.9215141386,      1.69707940618,    4.53967759657),
  SPHERE(0.153719577763,   4.93477975028,     3.24717223911,    1.50073511532),
  SPHERE(0.927757827229,   4.36627288472,     4.03561436284,    4.46429541065),
  SPHERE(1.42883697341,    4.49689533749,     4.04980157881,    1.87299735105),
  SPHERE(1.10631431489,    1.93397037157,     3.14113308886,    3.98165969747),
  SPHERE(3.61204472483,    4.17055449007,     0.662211181626,   4.24023709663),
  SPHERE(3.54377864794,    1.69617988637,     1.53629711196,    0.0914508917091),
  SPHERE(2.06047074945,    3.71570767089,     0.367639069735,   4.87723568975),
  SPHERE(0.16163892799,    2.26941912647,     1.78878531021,    1.32660354691),
  SPHERE(3.38405290111,    4.88644346742,     4.47588087275,    2.43355341201),
  SPHERE(0.775747964239,   0.206005958851,    2.2461651332,     3.87185963171),
  SPHERE(3.91529806126,    3.7308697953,      3.72937446357,    1.54107793844),
  SPHERE(4.45835497295,    4.53223216653,     0.65393433768,    0.574895114453),
  SPHERE(2.00177970157,    2.69339961071,     2.13845811128,    0.600608988799),
  SPHERE(2.86387364967,    3.33458345408,     2.73619381116,    2.38251274738),
  SPHERE(4.52795485314,    0.72016173832,     0.554668901778,   2.09923422313),
  SPHERE(2.85269197562,    1.40651470788,     3.75281915623,    3.03170637831),
  SPHERE(4.94133521058,    2.64930437479,     4.2063474592,     4.16699791204),
  SPHERE(2.79294442033,    2.69563716767,     0.308393909182,   2.25956335132),
  SPHERE(0.906768241784,   0.320814867724,    1.93252459079,    4.97276934056),
  SPHERE(1.73169585523,    3.35511028537,     4.89794581417,    0.420223810246),
  SPHERE(3.58720570672,    2.68881199665,     4.64247134631,    1.80852106777),
  SPHERE(1.61322802923,    2.14063003343,     4.1665774102,     1.60731405722),
  SPHERE(3.9674164131,     3.51763082581,     4.13713070285,    3.51811909856),
  SPHERE(2.82755582803,    3.99478176392,     0.166201321468,   1.67663577636),
  SPHERE(3.94912206452,    2.13643907879,     1.81262045897,    0.394678363693),
  SPHERE(4.36758376942,    2.49102746372,     1.87138894949,    0.46838329725),
  SPHERE(3.69782074453,    4.3185283272,      1.56475316262,    0.30924801685),
  SPHERE(2.09524344181,    2.2710435083,      2.65950624249,    3.18314530481),
  SPHERE(2.27397223907,    0.950016434039,    0.839104469613,   1.17803653822),
  SPHERE(4.38590718348,    0.561922295288,    1.07439144639,    0.52144464081),
  SPHERE(3.16491838702,    1.94927979293,     3.04616409423,    4.15997444541),
  SPHERE(2.10611742721,    0.764792402173,    1.23201303782,    3.11327176352),
  SPHERE(1.72160731471,    0.934755251219,    3.00868100873,    3.44198075786),
  SPHERE(0.242239250955,   2.26864475298,     2.87239714701,    2.54676505513),
  SPHERE(2.5476405569,     2.71744008331,     4.29081867031,    3.67127371496),
  SPHERE(3.43861948929,    1.64291531864,     0.0392226235385,  1.49794274449),
  SPHERE(0.116660518876,   4.79823180815,     3.19058533161,    1.83495988656),
  SPHERE(2.55356448044,    0.642657492447,    1.59620566479,    2.17840966706),
  SPHERE(1.64467316037,    3.68975322353,     3.95396940564,    3.74242521397),
  SPHERE(0.209746462415,   3.5737808433,      1.05367327955,    4.81330410446),
  SPHERE(0.212929721625,   4.22154987183,     1.563958284,      3.97546317185),
  SPHERE(4.10896649012,    1.4373842103,      1.61846903261,    1.80176976615),
  SPHERE(0.215792574043,   1.60920719705,     1.32207747013,    1.34569797866),
  SPHERE(0.0950913006398,  3.32257926035,     2.64343853658,    1.0359277756),
  SPHERE(0.0563124418083,  1.07125808976,     0.603466171395,   0.00262111415058),
  SPHERE(2.85835404318,    2.1909729393,      0.391127091075,   4.49896434227),
  SPHERE(3.37815330299,    3.20859738782,     2.34999565098,    4.63878454165),
  SPHERE(1.23865313833,    2.99949209251,     2.49649425911,    0.197126640925),
  SPHERE(1.1588461758,     1.51323780706,     3.3322579069,     4.05010599507),
  SPHERE(1.2953847522,     3.35234915409,     2.91908561174,    4.49019190103),
  SPHERE(0.755878300743,   2.95298143294,     0.841743742746,   3.4334617592),
  SPHERE(0.207619981204,   4.48701005277,     4.9945213524,     3.10640605274),
  SPHERE(2.77884193927,    1.47935943697,     1.27228983887,    1.66634760748),
  SPHERE(3.75376109812,    0.637522809572,    1.13374590368,    2.01304305836),
  SPHERE(2.43840437352,    1.15404607887,     2.91069547783,    4.81956210135),
  SPHERE(3.02356787018,    3.93984184077,     3.93330400126,    0.146154905775),
  SPHERE(2.98586604072,    1.65485330896,     0.6299683576,     2.18644679782),
  SPHERE(3.86765770847,    4.20893165664,     4.07061611037,    0.183348644464),
  SPHERE(1.27143783084,    2.88188822856,     2.03872043638,    0.982772804923),
  SPHERE(3.28562609073,    4.74384800265,     0.347892345951,   0.118076215739),
  SPHERE(4.09989240536,    4.44257699652,     4.93615543975,    1.79307238278),
  SPHERE(2.8643020976,     3.96446991541,     1.83876291986,    0.505401908222),
  SPHERE(4.90398132965,    1.73095537671,     3.55701893846,    1.9409024826),
  SPHERE(3.25856932277,    2.13961148697,     1.58561633242,    1.65358306785),
  SPHERE(0.988110776,      1.26691136558,     2.31376453136,    2.44126748424),
  SPHERE(1.6389436907,     2.62977677684,     4.97402882175,    2.94402707615),
  SPHERE(0.199515494082,   3.27622662258,     4.20503613339,    4.25238961586),
  SPHERE(0.40769142855,    4.55237015011,     4.73519217911,    4.14029199691),
  SPHERE(3.5207433136,     0.778826260958,    2.88323454794,    4.79693139527),
  SPHERE(0.289444025967,   4.67517801586,     2.96272931101,    4.8567464348),
  SPHERE(3.24851125459,    0.743036374853,    1.95836375694,    1.64681505919),
  SPHERE(3.70376096347,    0.166122224165,    0.351478491175,   0.109380636848),
  SPHERE(3.77664340665,    0.0861255526479,   0.695756590055,   2.85011449865),
  SPHERE(1.38627411542,    4.21585230755,     3.20838381266,    1.53290237406),
  SPHERE(0.816708648819,   1.94155520168,     2.62240084723,    4.56435401382),
  SPHERE(1.62843733021,    2.90621426867,     1.14855061723,    1.03057602027),
  SPHERE(2.26355039406,    2.78826880148,     4.66516455173,    4.75798597132),
  SPHERE(1.67405038418,    0.723712410878,    4.10462949146,    2.37832425087),
  SPHERE(2.73180907147,    0.824631069319,    3.62081019088,    0.874966131236),
  SPHERE(0.44614836184,    3.87399668625,     3.88418224404,    3.0297239407),
  SPHERE(3.45860321972,    1.35393033039,     1.75201944439,    0.0915922989148),
  SPHERE(0.0769102513585,  0.413881180742,    3.75327962445,    1.18394285593),
  SPHERE(1.81943984422,    1.18063334377,     1.34600531493,    2.19925427847),
  SPHERE(4.32725693584,    0.00225341036987,  3.0552622835,     3.69910539004),
  SPHERE(1.68047168841,    2.37883317863,     0.832193181343,   1.29005911739),
  SPHERE(4.24813460012,    4.31396752148,     1.57568689939,    0.491121364259),
  SPHERE(4.37983082218,    2.68357003199,     2.2709203493,     1.743232948),
};
// and the size of this array
static const std::size_t test_spheres_size = sizeof(test_spheres) / sizeof(double[4]);

int main(int argc, const char * argv[])
{
  // 1) Setup initial spheres
  //std::cout << "Setting up sphere intersecter" << std::endl;
  Sphere_intersecter<SK> si;

  // Add spheres to SI
  std::cout << "Adding spheres" << std::endl;
  std::copy(test_spheres, test_spheres + test_spheres_size,
      si.insert_iterator());
  // add the sphere to work with
  Sphere_handle sh = si.add_sphere(test_sphere);
  Sphere_3 s = *sh;

  // Event queue
  std::cout << "Building event queue" << std::endl;
  Event_queue<SK> E = Event_queue_builder<SK>()(si, sh);

  if (E.empty())
  {
    std::cerr << "Empty event queue" << std::endl;
    return 1;
  }

  // V-ordering
  std::list<Circular_arc_3> V;

  // Initialize V-ordering
  Vector_3 meridian(0, 1, 0);
  Line_3 pole_axis(s.center(), Direction_3(0, 0, 1));
  Circular_arc_3 meridian_arc;
  typedef std::vector<Object_3> Intersection_list;
  typedef std::pair<Circular_arc_point_3, unsigned int> CAP;
  Intersection_list poles;
  Intersect_3()(pole_axis, s, std::back_inserter(poles));
  CGAL_assertion(poles.size() == 2);
  CAP north_cap, south_cap;
  Assign_3()(north_cap, poles[0]);
  Assign_3()(south_cap, poles[1]);
  meridian_arc = Circular_arc_3(Circle_3(s, Plane_3(s.center(), meridian)), north_cap.first, south_cap.first);
  std::vector<Circle_handle> circles;
  si.circles_on_sphere(sh, std::back_inserter(circles));
  for (std::vector<Circle_handle>::const_iterator it = circles.begin();
      it != circles.end(); it++)
  {
    const Circle_3 & c = **it;
    Intersection_list ini_intersected_arcs;
    Intersect_3()(meridian_arc, c, std::back_inserter(ini_intersected_arcs));
    if (ini_intersected_arcs.empty())
    { continue; }
    else if (ini_intersected_arcs.size() == 2)
    {
      CAP cap1, cap2;
      Assign_3()(cap1, ini_intersected_arcs[0]);
      Assign_3()(cap2, ini_intersected_arcs[1]);
      V.push_back(Circular_arc_3(c, cap1.first, cap2.first));
      V.push_back(Circular_arc_3(c, cap2.first, cap1.first));
    }
    else
    {
      Intersection_list pole_inters;
      Intersect_3()(pole_axis, c, std::back_inserter(pole_inters));

      // If the intersection does not reduce to a pole,
      // but a point (end point).
      if (pole_inters.empty())
      {
        CAP cap;
        Assign_3()(cap, ini_intersected_arcs[0]);
        V.push_back(Circular_arc_3(c, cap.first));
      }
    }
  }

  // Sort arcs at initial meridian
  std::sort(V.begin(), V.end(), Compare_z_at_theta(meridian))

  // Initialize arrangement
  // TODO

  // Iterate over the event queue and get corresponding arcs
  std::cout << "Handling events" << std::endl;
  for (Event_queue<SK>::Event_site_type ev_type = E.next_event();
      ev_type != Event_queue<SK>::None; ev_type = E.next_event())
  {
    if (ev_type == Event_queue<SK>::Polar)
    {
      Event_queue<SK>::Polar_event_site pes = E.pop_polar();
      //break_adjacencies(pes);
      //handle_polar_event_site(E, V, pes);
    }
    else if (ev_type == Event_queue<SK>::Bipolar)
    {
      Event_queue<SK>::Bipolar_event_site bpes = E.pop_bipolar();
      //break_adjacencies(bpes);
      //handle_bipolar_event_site(E, V, bpes);
    }
    else
    {
      CGAL_assertion(ev_type == Event_queue<SK>::Normal);
      //break_adjacencies(nes);
      //handle_event_site(E, V, E.pop_normal());
    }
  }

  // Merge virtual faces
  // TODO

  return 0;
}

// vim: sw=2 et ts=2 sts=2
