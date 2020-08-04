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

void Getevtnb(const char *dataType = "", bool pnfs = true)
{
	std::string chain_name = "bTag_AntiKt4HIJets";
	//TTree *myChain = (TTree*)f->Get(chain_name.c_str());
	TTree *myChain;
	TFile *f;
	std::ofstream fileo(Form("../GetStuff/%s_evtnb%s.txt", dataType, suffix[pnfs]));
	int JZ_ID[grid_size];
	int JZ_wt[grid_size];

	if (pnfs)
	{
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
					//cout << "id: " << id << endl;
				}
				if (linePosj == 4)
				{
					if (itemj.find(dataType) == std::string::npos)
					{
						cout << "Wrong tag: " << itemj << endl;
						continue;
					}
					int k = itemj.find("JZ");
					//cout << itemj << endl;
					if (k == std::string::npos)
					{
						cout << "Wrong name format in JZ_ID: " << itemj << endl;
						return;
					}
					JZ_ID[itemj[k + 2] - 48] = std::stoi(id);
					cout << itemj[k + 2] - 48 << ": " << id << endl;
				}
				++linePosj;
			}
		}
	}
		int JZ = -1;
	if (pnfs)
	{
		std::ifstream fnames(Form("../GetStuff/%s_root_pnfs.txt", dataType));
		std::string line;

		while (std::getline(fnames, line))
		{
			int k = line.find("Akt4HIJets");
			if (k == std::string::npos)
			{
				cout << "Wrong name in reading: " << line << endl;
				return;
			}
			bool found = false;

			for (int j = 0; j < grid_size; j++)
			{
				if (std::stoi(line.substr(k - 9, 8)) == JZ_ID[j])
				{
					found = true;
					JZ = j;
				}
			}
			if (!found)
			{
				cout << line << endl;
				cout << "filename JZ info not found" << endl;
				return;
			}

			f = TFile::Open(line.c_str(), "READ");
			myChain = (TTree *)f->Get(chain_name.c_str());
			int newct = myChain->GetEntries();
			if (newct < 0 || newct > 3000)
			{
				cout << "file: " << line << "; ct: " << newct << endl;
				return;
			}
			JZ_wt[JZ] = JZ_wt[JZ] + newct;
			f->Close();
			f = 0;
			myChain = 0;
		}
	}
	else
	{
		ofstream outf(Form("../GetStuff/%s_root.txt", dataType),std::ofstream::trunc);
		std::string input = "/pnfs/usatlas.bnl.gov/users/cher97/rucio/user.xiaoning";
		DIR *dir1;
		dirent *pdir;
		dir1 = opendir(input.c_str());
		while ((pdir = readdir(dir1)))
		{
			std::string foldName = pdir->d_name;
			cout << pdir->d_name << endl;
			if (foldName.find(dataType) == std::string::npos)
				continue;
			if (!(foldName.find(".root") == std::string::npos))
				continue;
			int k = foldName.find("JZ");
				//cout << itemj << endl;
				if (k == std::string::npos)
				{
					cout << "Wrong folder name format: " << foldName << endl;
					return;
				}
				JZ = std::stoi(foldName.substr(k+2,1));
				JZ_wt[JZ] = 0;
			cout << "Success:" << pdir->d_name << endl;
			DIR *dir2;
			dirent *pdir2;
			dir2 = opendir((input + "/" + foldName).c_str());
			while ((pdir2 = readdir(dir2)))
			{
				std::string fName = pdir2->d_name;
				if (fName.find(".root") == std::string::npos)
					continue;
				f = TFile::Open((input + "/" + foldName + "/" + fName).c_str(), "READ");
				outf << input << "/" << foldName << "/" << fName << endl;
				myChain = (TTree *)f->Get(chain_name.c_str());
				
				int newct = myChain->GetEntries();
				if (newct < 0 || newct > 3000)
				{
					cout << "file: " << input << "/" << foldName << "/" << fName << "; ct: " << newct << endl;
					return;
				}
				JZ_wt[JZ] = JZ_wt[JZ] + newct;
				f->Close();
				f = 0;
				myChain = 0;
				//goto here;
			}
		}
	}
	for (int jz = 0; jz < grid_size; jz++)
	{
		cout << "JZ" << jz << ": " << JZ_wt[jz] << endl;
		fileo << jz << ": " << JZ_wt[jz] << endl;
	}
}