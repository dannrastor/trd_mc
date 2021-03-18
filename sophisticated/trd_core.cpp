#define PHOTON_ENERGY 60.0

//detector parameters
#define THR 4.2
#define PAIR_ENERGY 0.0042
#define NOISE 80 //electrons
#define FANO 0.14
#define PX_SIZE 55.0
#define THICKNESS 500.0

//absorption parameters
// ln(mu[cm^-1]) = A * ln(E[keV]) + B
//for E < 10 keV (see NIST)
#define A1 -2.658
#define B1 11.401
//for E > 10 keV
#define A2 -2.735
#define B2 13.570



//fluo parameters
#define FLUO_RATE 0.5
#define FLUO_BRANCH 0.5
#define FLUO_DISTANCE_GA 40.0
#define FLUO_DISTANCE_AS 15.0
#define FLUO_ENERGY 10.0


//sigma(z) fit parameters
#define p0 2.96
#define p1 0.02
#define p2 -0.000014


using namespace std;

random_device rd;
mt19937 gen(rd());






struct PhotonHit {
    double x;
    double y;
    double z;
    double energy;
};

using PhotonDistribution = vector<PhotonHit>;

struct Event {
    double deposit;
    int npixels;
    double z;
    ClusterType cluster_type;
};



class PhotonGenerator {
private:
    double yield_xy() {
        return PX_SIZE * unity(gen);
    }

    double abs_length(double energy) {
        double A = (energy < 10) ? A1 : A2;
        double B = (energy < 10) ? B1 : B2;
        double mu = exp(A * log(energy) + B);
        return 1 / mu * 10000;

    }

    double depth_distribution(double z) {
        return exp(-1.0 * (THICKNESS - z) / abs_length(PHOTON_ENERGY));
    };
    double yield_z() {
        uniform_real_distribution<double> urd(0, 1);

        while(1) {
            double z = THICKNESS * unity(gen);
            double p = unity(gen);
            if (p < depth_distribution(z)) {
                return z;
            }
        }
    };
public:
    PhotonGenerator() : unity(0, 1) {}

    PhotonDistribution Generate(double energy) {
        PhotonDistribution result;

        double x = yield_xy();
        double y = yield_xy();
        double z = yield_z();

        double if_fluo = unity(gen);
        if (if_fluo < FLUO_RATE && energy > FLUO_ENERGY) {
            double path = (unity(gen) > FLUO_BRANCH) ? FLUO_DISTANCE_AS : FLUO_DISTANCE_GA;
            double phi = 2 * TMath::Pi() * unity(gen);
            double cos_theta = 2 * unity(gen) - 1;
            double xf = x + path * cos(phi) * cos_theta;
            double yf = y + path * sin(phi) * cos_theta;
            double zf = z + path * cos_theta;
            result.push_back({x, y, z, energy - FLUO_ENERGY});
            result.push_back({xf, yf, zf, FLUO_ENERGY});
        } else {
            result.push_back({x, y, z, energy});
        }
        return result;
    }
private:
    uniform_real_distribution<double> unity;
};

class ResponseGenerator {
public:
    ResponseGenerator() : noise_distr(0, NOISE) {
    vfunc.push_back(
        [](double t, double s){return erf((PX_SIZE + t) / s / sqrt(2)) - erf(t / s / sqrt(2));}    );
    vfunc.push_back(
        [](double t, double s){return erf((PX_SIZE - t) / s / sqrt(2)) + erf(t / s / sqrt(2));} );
    vfunc.push_back(
        [](double t, double s){return erf((2*PX_SIZE - t) / s / sqrt(2)) - erf((PX_SIZE - t) / s / sqrt(2));}    );
    };
    Event Process(const PhotonDistribution& distr) {
        Event result;

        //3x3 array: energy deposit
        vector<vector<double>> pixels(3, vector<double>(3));

        for (const auto& photon : distr) {
            double e = smear_e(photon.energy);
            double sig = sigma(photon.z);
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    pixels[i][j] += e * 0.25 * vfunc[i](photon.x, sig) * vfunc[j](photon.y, sig);
                }
            }
        }
        int counter = 0;
        double deposit = 0;


        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                pixels[i][j] += yield_noise();
                if (pixels[i][j] < THR) {
                    pixels[i][j] = 0;
                } else {
                    counter++;
                    deposit += pixels[i][j];
                }
            }
        }

        return {deposit, counter, distr[0].z, define_type(pixels, counter)};
    };


private:
    double sigma(double z) {
        return p0 + p1 * z + p2 * z * z;
    }
    double smear_e(double energy) {
        double npairs = energy / PAIR_ENERGY;
        double sig = sqrt(FANO * npairs);
        normal_distribution<double> nd(npairs, sig);
        return PAIR_ENERGY * nd(gen);
    }
    double yield_noise() {
        return PAIR_ENERGY * noise_distr(gen);
    }


    normal_distribution<double> noise_distr;
    vector<function<double(double, double)>> vfunc;
};

map<ClusterType, int> clsstats(double e) {
    PhotonGenerator pg;
    ResponseGenerator rg;
    int nevents = 100000;
    map<ClusterType, int> cluster_stats;
    for (long int i = 0; i < nevents; ++i) {
        auto photon_distr = pg.Generate(e);
        auto p = rg.Process(photon_distr);
        cluster_stats[p.cluster_type]++;
    }
    return cluster_stats;
}
