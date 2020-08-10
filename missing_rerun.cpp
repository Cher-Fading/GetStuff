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

//const float trkLim = 2.0;
//const float ptLim = 50;
//const float ptLim2 = 60;

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

const int llrlim = 100;
const int limshift = 20;
const int llr_bins = (int)4 * llrlim;

char suffix[2][10] = {"", "_pnfs"};

void missing_rerun(const char *dataType, bool pnfs, float ptLim, float trkLim, const char *opt, bool PbPb = true)
{
    int cent_N = PbPb ? cet_N : 1;
    int JZ_ID[grid_size][2];
    int JZ_ct[grid_size][2];
    int JZ = -1;
    for (int j = 0; j < grid_size; j++)
    {
        for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
        {
            JZ_ID[j][jj] = 0;
            JZ_ct[j][jj] = 0;
        }
    }

    std::ifstream filej("../GetStuff/JZ_ID.txt");
    std::string linej;
    while (std::getline(filej, linej))
    {
        std::stringstream linestreamj(linej);
        std::string itemj;
        int linePosj = 0;
        std::string id;
        //cout << linej << endl;
        while (std::getline(linestreamj, itemj, ' '))
        {
            if (itemj == "")
                continue;

            if (linePosj == 0)
            {
                id = itemj;
            }
            if (linePosj == 4)
            {
                if (itemj.find(dataType) == std::string::npos)
                {
                    //cout << "Wrong file name" << itemj << endl;
                    continue;
                }
                int k = itemj.find("JZ");
                //cout << itemj << endl;
                int j = 0;
                if (k == std::string::npos)
                {
                    cout << "Wrong name" << itemj << endl;
                    return;
                }
                while (JZ_ID[itemj[k + 2] - 48][j] > 1000000)
                {
                    j++;
                }
                JZ_ID[itemj[k + 2] - 48][j] = std::stoi(id);
                cout << itemj[k + 2] - 48 << "; " << j << ": " << id << endl;
            }
            ++linePosj;
        }
    }

    std::ifstream filein(Form("../GetStuff/%s_root%s.txt", dataType, suffix[pnfs]));           //list of input files
    std::ifstream filesdone(Form("../GetStuff/%s_done%s%s.txt", dataType, opt, suffix[pnfs])); //list of finished files
    //cout << Form("../GetStuff/%s_50.00_0.5_done%s.txt", dataType, suffix[pnfs]) << endl;
    std::ofstream outF(Form("../GetStuff/%s_root%s_%srerun.txt", dataType, suffix[pnfs], opt)); //list of unfinished files
    int totf = 0;
    int donef[cent_N];
    int outf[cent_N];
    for (int c = 0; c < cent_N; c++)
    {
        donef[c] = 0;
        outf[c] = 0;
    }

    int done[6][3000][2][cent_N];
    int full[6][3000][2];
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3000; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                full[i][j][k] = 0;
                for (int c = 0; c < cent_N; c++)
                {
                    done[i][j][k][c] = 0;
                }
            }
        }
    }

    std::string filename;
    int NUM = -1;
    int tag = -1;
    int central = -1;

    //cout << 138 << endl;
    std::string filedone;
    while (std::getline(filesdone, filedone))
    {
        //cout << filedone << endl;
        int k = filedone.find("JZ");
        JZ = filedone[k + 2] - 48;
        //cout << filedone << endl;
        //cout << "JZ: " << JZ << endl;
        if (k == std::string::npos)
        {
            cout << "Wrong name: " << filedone << endl;
            continue;
        }
        bool found = false;

        int k2 = filedone.find("rapidity");
        tag = std::stoi(filedone.substr(k2 - 1, 1));
        //cout << "tag: " << tag << endl;
        NUM = std::stoi(filedone.substr(k + 4, k2 - k - 6));
        //cout << JZ << endl;
        //cout << "NUM: " << NUM << endl;
        int k3 = filedone.find("cent");
        central = std::stoi(filedone.substr(k3 + 5, 1));
        if (done[JZ][NUM][tag][central] == 1)
        {
            cout << "name repeated for done at JZ: " << JZ << "; NUM: " << NUM << "; tag: " << tag << "; centrality: " << central << endl;
            return;
        }
        done[JZ][NUM][tag][central] = 1;
        donef[central]++;
    }

    while (std::getline(filein, filename))
    {
        int k = filename.rfind("Akt4HIJets");
        if (k == std::string::npos)
        {
            cout << "Wrong name: " << filename << endl;
            return;
        }
        bool found = false;

        for (int j = 0; j < grid_size; j++)
        {
            for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
            {
                //cout << k << endl;
                //cout << filename << endl;
                //int k2 = filename.find("214");
                //cout << k2 << endl;
                //cout << std::stoi(filename.substr(k - 9, 8)) << endl;
                //cout << filename.substr(k - 9, 8) << endl;
                if (std::stoi(filename.substr(k - 9, 8)) == JZ_ID[j][jj])
                {
                    found = true;
                    JZ = j;
                    tag = jj;
                    //cout << "JZ: " << JZ << endl;
                    //return;
                }
            }
        }

        if (!found)
        {
            cout << filename << endl;
            cout << "not found" << endl;
            return;
        }

        NUM = std::stoi(filename.substr(filename.length() - 11, 6));
        if (full[JZ][NUM][tag] == 1)
        {
            cout << "name repeated for full at JZ: " << JZ << "; NUM: " << NUM << "; tag: " << tag << endl;
            return;
        }
        full[JZ][NUM][tag] = 1;
        totf++;
bool missing = false;
        for (int c = 0; c < cent_N; c++)
        {
            if (done[JZ][NUM][tag][c] != 1)
            {
		missing = true;
                cout << filename << " for centrality: " << c << endl;
                outf[c]++;
            }
        }
if (missing) outF << filename << endl;
    }

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3000; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                for (int c = 0; c < cent_N; c++)
                {
                    if (done[i][j][k][c] == 1 && full[i][j][k] != 1)
                    {
                        cout << "JZ: " << i << endl;
                        cout << "ID: " << j << endl;
                        cout << "tag: " << k << endl;
                        cout << "centrality: " << c << endl;
                    }
                }
            }
        }
    }

    cout << totf << endl;
    for (int c = 0; c < cent_N; c++)
    {
        cout << "done: centrality: " << c << ": " << donef[c] << endl;
        cout << "rerun: centrality: " << c << ": " << outf[c] << endl;
    }
}
