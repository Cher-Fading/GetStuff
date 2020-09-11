#include "InfoHeaders.h"

void Getevtnb(const char *dataType = "", bool PbPb = true, bool pnfs = true, bool inclusive = true)
{
	std::string chain_name = "bTag_AntiKt4HIJets";
	//TTree *myChain = (TTree*)f->Get(chain_name.c_str());
	int cent_N = PbPb ? cet_N : 1;
	int gridsize = inclusive ? grid_size : s50k_size;
	TTree *myChain;
	TFile *f;
	std::string type_ = inclusive ? "" : Type[PbPb];
	std::ofstream fileo(Form("../GetStuff/%s%s_evtnb%s.txt", dataType, type_.c_str(), suffix[pnfs]));

	int JZ_ID[gridsize][2];
	float JZ_wt[gridsize];
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
			//cout << tree->GetEntries() << endl;
			tree->SetBranchAddress("mcwg", &mcwg, &b_mcwg);

			for (int jentry = 0; jentry < tree->GetEntries(); jentry++)
			{
				b_mcwg->GetEntry(jentry);
				JZ_wt[i] = JZ_wt[i] + mcwg;
			}
			f->Close();
			//delete f;
			//delete tree;
		}
	}

	int JZ_shift = inclusive ? 0 : 1;
	for (int jz = 0; jz < gridsize; jz++)
	{
		cout << "JZ" << jz + JZ_shift << ": " << JZ_wt[jz] << endl;
		fileo << jz + JZ_shift << ": " << JZ_wt[jz] << endl;
	}
	fileo.close();
}

bool parse_filename(std::string filename, int &JZ, int &tag, int &NUM, bool &inclusive, bool &PbPb, bool &pnfs, std::string &dataType)
{
	//initialize the values
	JZ = -1;
	tag = -1;
	NUM = -1;
	inclusive = false;
	PbPb = false;
	pnfs = false;
	dataType = "";
	int valid = 0;

	TString fileName = filename.data();

	for (int s = 0; s < nString; s++)
	{
		if (fileName.Contains(inclusive_str[s]))
		{
			inclusive = true;
			valid++; //1
			break;
		}
	}
	int gridsize = inclusive ? grid_size : s50k_size;

	if (inclusive) //use jobID to determine JZ tag and PbPb
	{
		for (int p = 0; p < nPNFS; p++)
		{
			if (fileName.Contains(pnfs_str[p]))
			{
				pnfs = true;

				break;
			}
		}				
		if (!pnfs)
		{
			for (int np = 0; np < nNpnfs; np++)
			{
				if (fileName.Contains(nnpnfs_str[np]))
				{
					pnfs = false;
					valid++; //2
					break;
				}
			}
		}
		if (valid != 2)
		{
			cout << "pnfs info wrong" << endl;
			cout << pnfs << valid << endl;
			return false;
		}
		int k = filename.rfind("Akt4HIJets");
		if (k == std::string::npos)
		{
			cout << "Wrong name: " << filename << endl;
			return false;
		}
		int jobID = std::stoi(filename.substr(k - 9, 8));
		std::ifstream fJZ_ID("../GetStuff/JZ_ID.txt");
		if (!fJZ_ID) {
		  cout << "JZ_ID file not found" << endl;
		  return false;
		}
		std::string linej;
		bool found = false;
		while (std::getline(fJZ_ID, linej))
		{
			std::stringstream linestreamj(linej);
			std::string itemj;
			int linePosj = 0;
			//std::string id;
			//cout << linej << endl;
			while (std::getline(linestreamj, itemj, ' '))
			{
				if (itemj == "")
					continue;

				if (linePosj == 0)
				{
					if (std::stoi(itemj) == jobID)
						found = true;
				}
				if (linePosj == 4)
				{
					if (!found)
					{
						cout << "Wrong file name" << itemj << endl;
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
					JZ = std::stoi(itemj.substr(k+2,1));
					if (JZ >= 0 && JZ < gridsize)
						valid++;
					else
					{
						cout << "JZ wrong" << endl;
						return false;
					}
					valid++; //3
					TString itemJ = itemj.data();
					if (itemJ.Contains("r11199"))
					{
						PbPb = false;
						valid++; //4
					}
					if (itemJ.Contains("r11217"))
					{
						PbPb = true;
						valid++; //4
					}
					if (valid != 4)
					{
						cout << "PbPb wrong" << endl;
						return false;
					}
					tag = itemJ.Contains("e6608") ? 0 : itemJ.Contains("e4108") ? 1 : -1;
					if (tag >= 0)
						valid++; //5
					else
					{
						cout << "tag wrong" << endl;
						return false;
					}

					dataType = itemj.substr(itemj.rfind(".")+1,itemj.length()-2-itemj.rfind("."));
					//cout << itemj[k + 2] - 48 << "; " << j << ": " << id << endl;
				}
				++linePosj;
			}
		}
		if (valid!=5){
		  cout << "jobID wrong" << endl;
		  return false;
		}

		NUM = std::stoi(filename.substr(filename.length() - 11, 6));
		if (NUM >= 0)
			valid++; //6
		else
		{
			cout << "NUM wrong" << endl;
			return false;
		}
	}
	else
	{
		bool nonInc = 0;
		for (int n = 0; n < nNonInc; n++)
		{
			if (fileName.Contains(ninclusive_str[n]))
			{
				nonInc = true;
				valid++; //1
			}
		}
		if (!nonInc)
		{
			cout << "wrong file name, neither inclusive nor non-inclusive" << endl;
			return false;
		}
		pnfs = false;
		valid++; //2
		JZ = std::stoi(filename.substr(filename.find("JZ") + 2,1));
		if (JZ >= 1 && JZ <= gridsize)
			valid++; //3
		else
		{
			cout << "JZ wrong" << endl;
			return false;
		}
		if (fileName.Contains("PbPb"))
		{
			PbPb = true;
			valid++; //4
		}
		if (fileName.Contains("pp"))
		{
			PbPb = false;
			valid++; //4
		}
		if (valid != 4)
		{
			cout << "PbPb wrong" << endl;
			return false;
		}
		tag = 0;
		valid++; //5
		NUM = 0;
		valid++; //6
		dataType = filename.substr(filename.find("flav_")+5,filename.find("_Akt4HIJets")-filename.find("flav_")-5);
	}
	if (valid !=6) {
	  cout << valid << endl;
	  return false;
	}
	return true;
}


float get_weight(std::string filename)
{
	//int cent_N = PbPb ? cet_N : 1;
	int JZ = -1;
	int tag = -1;
	int NUM = -1;
	bool inclusive = false;
	bool PbPb = false;
	bool pnfs = false;
	std::string dataType = "";

	bool parsed = parse_filename(filename,JZ,tag,NUM,inclusive,PbPb,pnfs,dataType);
	if (!parsed){
		cout << "parsing failed" << endl;
		return -1;
	}

	std::string type_ = inclusive ? "" : Type[PbPb];

	std::ifstream fevtnb(Form("../GetStuff/%s%s_evtnb%s.txt", dataType.c_str(), type_.c_str(), suffix[pnfs]));
	if (!fevtnb){
		cout << "wrong parsing or filename" << filename << " " << dataType << endl;
		return -1;
	}
	std::string eline;
	//int JZ_wt[gridsize];
	int gridsize = inclusive ? grid_size : s50k_size;
	float JZ_wt[gridsize];
	int JZ_shift = inclusive ? 0 : 1;
	while (std::getline(fevtnb, eline))
	{
		JZ_wt[std::stoi(eline.substr(0, 1)) - JZ_shift] = std::stoi(eline.substr(3, eline.length() - 3));
	}

	
	if (JZ_wt[JZ] <= 0)
	{
		cout << "JZ weight <= 0 at weight = " << JZ_wt[JZ] << " for JZ = " << JZ << endl;
		return -1;
	}
	return JZ_wt[JZ];
}
