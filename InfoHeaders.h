#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <dirent.h>
#include "TFile.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TSystem.h"
#include <algorithm>
#include "TStyle.h"
#include "TColor.h"
#include "TProfile.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include <math.h>

#include <TH2.h>
#include <TStyle.h>

const int myColor[] = {kBlue, kViolet, kMagenta, kPink, kOrange, kYellow, kSpring, kTeal, kCyan, kAzure, kGray, kGray + 1, kGray + 3};
const float eta_cut = 2.1;
const float eta_cut2 = 2.4;
const float JZs = 5;
const float s50kWeight[] = {6.7890E+07 * 6.1692E-06, 6.3996E+05 * 5.8420E-05, 4.7192E+03 * 1.1270E-04, 9.2038E-05 * 2.6602E+01};
const int s50k_size = sizeof(s50kWeight) / sizeof(float);
const float pt_max = 2000.;
const float Weight[] = {6.7890E+07, 6.789E+07, 6.3996E+05, 4.7195E+03, 2.6602E+01, 2.2476E-01};
const float Filter[] = {9.9713E-01, 2.8748E-03, 4.2952E-03, 5.2994E-03, 4.5901E-03, 2.1846E-03};
const int grid_size = sizeof(Weight) / sizeof(float);
char legend1[3][50] = {"b-Jets", "c-jet", "light jet"};
char leg[3][10] = {"B", "C", "U"};
const float FCal_range[] = {0, 0.063719, 0.14414, 0.289595, 0.525092, 0.87541, 1.36875, 2.04651, 2.98931, 5}; // fcal_cuts options

const int cet[] = {0, 2, 2, 5, 5, 8}; //selected centrality sections
const int cet_N = (sizeof(cet) / sizeof(int)) / 2;

const int llr_bins = 200;
const int llrlim = 40;

char suffix[2][10] = {"", "_pnfs"};
char Type[2][10] = {"pp", "PbPb"};

const int nString = 2;
char inclusive_str[nString][30] = {"/usatlas/scratch", "/pnfs"};
const int nNonInc = 1;
char ninclusive_str[nNonInc][30] = {"/atlasgpfs01/"};
const int nPNFS = 1;
char pnfs_str[nPNFS][30] = {"/usatlas/scratch"};
const int nSmall = 1;
char small_str[nSmall][30] = {"flav_"};