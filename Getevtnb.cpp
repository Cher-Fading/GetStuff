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
void Getevtnb(const char *dataType = "", bool PbPb = true, bool pnfs = true, bool inclusive = true)
{
	std::string chain_name = "bTag_AntiKt4HIJets";
	//TTree *myChain = (TTree*)f->Get(chain_name.c_str());
	int cent_N = PbPb ? cet_N : 1;
	int gridsize = inclusive ? grid_size : s50k_size;
	TTree *myChain;
	TFile *f;
	std::ofstream fileo(Form("../GetStuff/%s_evtnb%s.txt", dataType, suffix[pnfs]));

	int JZ_ID[gridsize][2];
	int JZ_wt[gridsize];
	int JZ = -1;
	int tag = -1;
	int NUM = -1;
	for (int j = 0; j < gridsize; j++)
	{
		JZ_wt[j] = 0;
		for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
		{
			JZ_ID[j][jj] = 0;
		}
	}

	if (inclusive)
	{
		std::ofstream fileo2(Form("../GetStuff/%s_filenum%s.txt", dataType, suffix[pnfs]));
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
					//cout << itemj << endl;
					int j = 0;
					while (JZ_ID[itemj[k + 2] - 48][j] > 1000000)
					{
						j++;
					}

					JZ_ID[itemj[k + 2] - 48][j] = std::stoi(id);
					cout << itemj[k + 2] - 48 << ": " << j << ": " << id << endl;
					//JZ_wt[itemj[k+2]-48] = 0;
				}
				++linePosj;
			}
		}
		if (pnfs)
		{
			std::ifstream fnames(Form("../GetStuff/%s_root%s.txt", dataType, suffix[pnfs]));
			std::string filename;
			int counter = 0;
			while (std::getline(fnames, filename))
			{
				int k = filename.rfind("Akt4HIJets");
				if (k == std::string::npos)
				{
					cout << "Wrong name in reading: " << filename << endl;
					return;
				}
				counter++;

				bool found = false;

				for (int j = 0; j < grid_size; j++)
				{
					for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
					{
						if (std::stoi(filename.substr(k - 9, 8)) == JZ_ID[j][jj])
						{
							found = true;
							JZ = j;
							tag = jj;
						}
					}
				}

				if (!found)
				{
					cout << filename << endl;
					cout << "filename JZ info not found for pnfs" << endl;
					return;
				}
				//if (JZ != 0) continue;
				f = TFile::Open(filename.c_str(), "READ");
				myChain = (TTree *)f->Get(chain_name.c_str());
				int newct = myChain->GetEntries();
				if (newct < 0 || newct > 30000)
				{
					cout << "file: " << filename << "; ct: " << newct << endl;
					return;
				}
				//cout << counter << ": " << line << ": " << newct << endl;
				//cout << JZ_wt[JZ] << endl;
				JZ_wt[JZ] = JZ_wt[JZ] + newct;
				NUM = std::stoi(filename.substr(filename.length() - 11, 6));
				if (NUM < 0)
				{
					cout << "wrong num for " << filename << endl;
					return;
				}
				//cout << JZ_wt[JZ] << endl;
				//cout << endl;
				fileo2 << JZ << "_" << tag << "_" << NUM << ": " << newct << endl;
				f->Close();
				f = 0;
				myChain = 0;
				if (counter % 100 == 0)
				{
					cout << "counted " << counter << " files" << endl;
				}
			}
		}
		else
		{
			ofstream outf(Form("../GetStuff/%s_root.txt", dataType), std::ofstream::trunc);
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
				JZ = std::stoi(foldName.substr(k + 2, 1));

				cout << "Success:" << pdir->d_name << endl;
				DIR *dir2;
				dirent *pdir2;
				dir2 = opendir((input + "/" + foldName).c_str());
				while ((pdir2 = readdir(dir2)))
				{
					std::string fName = pdir2->d_name;
					if (fName.find(".root") == std::string::npos)
						continue;
					int k2 = fName.rfind("Akt4HIJets");
					if (k2 == std::string::npos)
					{
						cout << "Wrong name in reading: " << input << "/" << foldName << "/" << fName << endl;
						return;
					}
					f = TFile::Open((input + "/" + foldName + "/" + fName).c_str(), "READ");
					outf << input << "/" << foldName << "/" << fName << endl;
					myChain = (TTree *)f->Get(chain_name.c_str());
					bool found = false;
					for (int j = 0; j < grid_size; j++)
					{
						for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
						{
							if (std::stoi(fName.substr(k2 - 9, 8)) == JZ_ID[j][jj])
							{
								found = true;
								if (JZ != j)
								{
									cout << "JZ doesn't match up: " << std::stoi(fName.substr(k2 - 9, 8)) << fName << endl;
									return;
								}
								tag = jj;
							}
						}
					}

					if (!found)
					{
						cout << fName << endl;
						cout << "filename JZ info not found for non pnfs" << endl;
						return;
					}

					int newct = myChain->GetEntries();
					if (newct < 0 || newct > 30000)
					{
						cout << "file: " << input << "/" << foldName << "/" << fName << "; ct: " << newct << endl;
						return;
					}
					JZ_wt[JZ] = JZ_wt[JZ] + newct;
					int NUM = std::stoi(fName.substr(fName.length() - 11, 6));
					fileo2 << JZ << "_" << tag << "_" << NUM << ": " << newct << endl;
					f->Close();
					f = 0;
					myChain = 0;
					//goto here;
				}
			}
			outf.close();
		}
		fileo2.close();
	}
	else
	{
		Float_t mcwg;
		TBranch *b_mcwg; //!
		for (int i = 0; i < gridsize; i++)
		{
			TFile *f = TFile::Open(Form("/atlasgpfs01/usatlas/data/cher97/flav_%s_Akt4HIJets/%smc16/%smc16JZ%d.root", dataType, Type[PbPb], Type[PbPb], i + 1), "READ");
			TTree *tree = (TTree *)f->Get(chain_name.c_str());
			cout << tree->GetEntries() << endl;
			tree->SetBranchAddress("mcwg", &mcwg, &b_mcwg);

			for (int jentry = 0; jentry < JZ_wt[i]; jentry++)
			{
				b_mcwg->GetEntry(jentry);
				JZ_wt[i] = JZ_wt[i] + mcwg;
			}
			f->Close();
			//delete f;
			//delete tree;
		}
	}

	for (int jz = 0; jz < gridsize; jz++)
	{
		cout << "JZ" << jz << ": " << JZ_wt[jz] << endl;
		fileo << jz << ": " << JZ_wt[jz] << endl;
	}
	fileo.close();
}

float get_weight(const char *dataType, std::string filename, bool pnfs, bool inclusive)
{
	//int cent_N = PbPb ? cet_N : 1;
	std::ifstream fevtnb(Form("../GetStuff/%s_evtnb%s.txt", dataType, suffix[pnfs]));
	int gridsize = inclusive ? grid_size : s50k_size;

	std::string eline;
	int JZ_evtnb[gridsize];
	while (std::getline(fevtnb, eline))
	{
		JZ_evtnb[std::stoi(eline.substr(0, 1))] = std::stoi(eline.substr(3, eline.length() - 3));
	}
	std::ifstream fJZ_ID("../GetStuff/JZ_ID.txt");

	int JZ = -1;
	int tag = -1;
	int JZ_ID[gridsize][2];
	int JZ_ct[gridsize][2];
	for (int j = 0; j < gridsize; j++)
	{
		for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
		{

			JZ_ct[j][jj] = 0;
			JZ_ID[j][jj] = 0;
		}
	}

	std::string linej;
	while (std::getline(fJZ_ID, linej))
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
					return -1;
				}
				while (JZ_ID[itemj[k + 2] - 48][j] > 1000000)
				{
					j++;
				}
				JZ_ID[itemj[k + 2] - 48][j] = std::stoi(id);
				//cout << itemj[k + 2] - 48 << "; " << j << ": " << id << endl;
			}
			++linePosj;
		}
	}
	int k = filename.rfind("Akt4HIJets");
	if (k == std::string::npos)
	{
		cout << "Wrong name: " << filename << endl;
		return -1;
	}
	bool found = false;

	for (int j = 0; j < gridsize; j++)
	{
		for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
		{
			if (std::stoi(filename.substr(k - 9, 8)) == JZ_ID[j][jj])
			{
				found = true;
				JZ = j;
				tag = jj;
			}
		}
	}

	if (!found)
	{
		cout << filename << endl;
		cout << "not found" << endl;
		return -1;
	}

	float weight = -1;
	if (inclusive)
	{
		weight = 1;
	}
	return -1;
}
