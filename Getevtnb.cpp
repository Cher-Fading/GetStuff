#include "InfoHeaders.h"

bool parse_count(const char *filename, std::vector<float> &jets_count)
{
	std::ifstream filec(filename);
	if (!filec)
	{
		cout << "missing: " << filename << endl;
		return false;
	}
	int n_types = 0;
	std::string line2;
	while (getline(filec, line2))
	{
		int l1 = line2.find(" ");
		if (l1 == std::string::npos)
		{
			if (n_types == 0) //if the first line is empty
			{
				cout << "file empty at " << filename << endl;
				return false;
			}
			continue; //else this migth be the last line
		}
		int l2 = line2.rfind(" ");
		float count = std::stof(line2.substr(l2 + 1, line2.length() - l2 - 1));
		if (count < 0)
		{
			cout << "wrong counts at line: " << n_types << endl;
			return false;
		}
		jets_count[n_types] = count;
		n_types++;
	}
	filec.close();
	return true;
}

bool parse_filename(std::string filename, int &JZ, int &tag, int &NUM, bool &inclusive, bool &PbPb, bool &pnfs, std::string &dataType, bool batch = true)
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
	//cout << filename << endl;

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
		int k = filename.rfind("Akt4HIJets");
		if (k == std::string::npos)
		{
			cout << "[ERROR] Wrong name: " << filename << endl;
			return false;
		}
		int jobID = std::stoi(filename.substr(k - 9, 8));
		if (!batch)
			cout << "[INFO] job ID: " << jobID << endl;
		for (int PNFS = 0; PNFS < 2; PNFS++)
		{
			std::ifstream fJZ_ID(Form("../GetStuff/JZ_ID%s.txt", suffix2[PNFS]));
			if (!fJZ_ID)
			{
				cout << "[ERROR] JZ_ID file not found" << endl;
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
							//cout << "Wrong file name: " << itemj << endl;
							continue;
						}
						else if (!batch)
							cout << "[INFO] found: " << itemj << endl;
						pnfs = PNFS;
						valid++; //2
						if (valid != 2)
						{
							cout << "[ERROR] pnfs info wrong" << endl;
							cout << pnfs << valid << endl;
							return false;
						}
						int k = itemj.find("JZ");
						//cout << itemj << endl;
						int j = 0;
						if (k == std::string::npos)
						{
							cout << "[ERROR] Wrong name" << itemj << endl;
							return -1;
						}
						JZ = std::stoi(itemj.substr(k + 2, 1));
						if (JZ >= 0 && JZ < gridsize)
							valid++;
						if (valid != 3)
						{
							cout << "[ERROR] JZ wrong: " << valid << endl;
							return false;
						}
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
							cout << "[ERROR] PbPb wrong: " << valid << endl;
							return false;
						}
						tag = itemJ.Contains("e6608") ? 0 : itemJ.Contains("e4108") ? 1 : -1;
						if (tag >= 0)
							valid++; //5
						else
						{
							cout << "[ERROR] tag wrong" << endl;
							return false;
						}

						dataType = itemj.substr(itemj.rfind("Working"), itemj.length() - 1 - itemj.rfind("Working"));
						goto here;
						//cout << itemj[k + 2] - 48 << "; " << j << ": " << id << endl;
					}
					++linePosj;
				}
			}
		}
	here:
		if (valid != 5)
		{
			cout << "[ERROR] jobID wrong" << endl;
			return false;
		}

		NUM = std::stoi(filename.substr(filename.length() - 11, 6));
		if (NUM >= 0)
			valid++; //6
		else
		{
			cout << "[ERROR] NUM wrong" << endl;
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
			cout << "[ERROR] wrong file name, neither inclusive nor non-inclusive" << endl;
			return false;
		}
		pnfs = false;
		valid++; //2
		JZ = std::stoi(filename.substr(filename.find("JZ") + 2, 1));
		if (JZ >= 1 && JZ <= gridsize)
			valid++; //3
		else
		{
			cout << "[ERROR] JZ wrong: " << valid << endl;
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
			cout << "[ERROR] PbPb wrong" << endl;
			return false;
		}
		tag = 0;
		valid++; //5
		NUM = 0;
		valid++; //6
		dataType = filename.substr(filename.find("flav_") + 5, filename.find("_Akt4HIJets") - filename.find("flav_") - 5);
	}
	if (valid != 6)
	{
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

	bool parsed = parse_filename(filename, JZ, tag, NUM, inclusive, PbPb, pnfs, dataType);
	if (!parsed)
	{
		cout << "[ERROR] parsing failed" << endl;
		return -1;
	}

	std::string type_ = inclusive ? "" : Type[PbPb];

	std::ifstream fevtnb(Form("../GetStuff/%s%s_evtnb%s.txt", dataType.c_str(), type_.c_str(), suffix[pnfs]));
	if (!fevtnb)
	{
		cout << "[ERROR] wrong parsing or filename" << filename << " " << dataType << endl;
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
		cout << "[ERROR] JZ weight <= 0 at weight = " << JZ_wt[JZ] << " for JZ = " << JZ << endl;
		return -1;
	}
	return JZ_wt[JZ];
}

float get_weight(int JZ, int tag, int NUM, bool inclusive, bool PbPb, bool pnfs, std::string dataType)
{
	std::string type_ = inclusive ? "" : Type[PbPb];

	std::ifstream fevtnb(Form("../GetStuff/%s%s_evtnb%s.txt", dataType.c_str(), type_.c_str(), suffix[pnfs]));
	if (!fevtnb)
	{
		cout << "[ERROR] wrong parsing or wrong JZ " << JZ << " tag " << tag << " NUM " << NUM << " PbPb " << PbPb << " inclusiveness " << inclusive << " dataType " << dataType << endl;
		return -1;
	}
	std::string eline;
	//int JZ_wt[gridsize];
	int gridsize = inclusive ? grid_size : s50k_size;
	double weight = inclusive? s50kWeight[JZ-1]:Weight[JZ]*Filter[JZ];
	float JZ_wt[gridsize];
	int JZ_shift = inclusive ? 0 : 1;
	while (std::getline(fevtnb, eline))
	{
		JZ_wt[std::stoi(eline.substr(0, 1)) - JZ_shift] = std::stoi(eline.substr(3, eline.length() - 3));
		cout << eline.substr(0, 1) << ": " << eline.substr(3, eline.length() - 3) << endl;
	}

	if (JZ_wt[JZ] <= 0)
	{
		cout << "[ERROR] JZ weight <= 0 at weight = " << JZ_wt[JZ] << " for JZ = " << JZ << endl;
		return -1;
	}
	return JZ_wt[JZ];
}

bool parse_trainname(std::string trainname, float &stat, float &cStat, float &outStat, float &outcStat, float &ptLim, float &aeta)
{
	std::ifstream fstat(Form("../GetStuff/%s_stat.txt", trainname.c_str()));
	std::string line;

	stat = 0;
	cStat = 0;
	outStat = 0;
	outcStat = 0;
	ptLim = 50.;
	aeta = 2.1;
	float ratio = 1.;

	while (getline(fstat, line))
	{
		TString Line = line.data();
		//cout << line << endl;
		if (Line.Contains("stat"))
		{
			TString stat_str = line.substr(4, line.length());
			stat = stat_str.ReplaceAll("k", "000").Atof();
		}
		if (Line.Contains("cstat"))
		{
			TString cstat_str = line.substr(5, line.length());
			cStat = cstat_str.ReplaceAll("k", "000").Atof();
		}
		if (Line.Contains("badMargin"))
		{
			ratio = 1 + std::stof(line.substr(9, line.length())) / 100.;
		}
		if (Line.Contains("pT"))
		{
			ptLim = std::stof(line.substr(2, line.length()));
		}
		if (Line.Contains("aeta"))
		{
			aeta = std::stof(line.substr(4, line.length())) / 10.;
		}
	}

	if (stat == 0)
	{
		cout << "[ERROR]: no stat limit given" << endl;
		return false;
	}
	if (cStat == 0)
		cStat = stat * 0.5;
	outStat = ratio * stat;
	outcStat = ratio * cStat;
	fstat.close();
	return true;
}

bool parse_filename_short(std::string filename, std::string dataType, bool PbPb, bool pnfs, bool inclusive, int &JZ, int &tag, int &NUM, bool batch = true)
{
	std::string dataType2 = "";
	bool PbPb2 = false;
	bool pnfs2 = false;
	bool inclusive2 = false;
	bool parsed = parse_filename(filename, JZ, tag, NUM, inclusive2, PbPb2, pnfs2, dataType2, batch);
	if (!parsed)
	{
		cout << "[ERROR]: parsing failed at parse filename short calling parse filename" << endl;
		return false;
	}
	if (PbPb2 != PbPb)
	{
		cout << "[ERROR]: parsing failed at parse filename short, PbPb mismatch: " << PbPb2 << "/" << PbPb << endl;
		return false;
	}
	if (pnfs2 != pnfs)
	{
		cout << "[ERROR]: parsing failed at parse filename short, pnfs mismatch: " << pnfs2 << "/" << pnfs << endl;
		return false;
	}
	if (inclusive2 != inclusive)
	{
		cout << "[ERROR]: parsing failed at parse filename short, inclusiveness mismatch: " << inclusive2 << "/" << inclusive << endl;
		return false;
	}
	if (dataType2 != dataType)
	{
		cout << "[ERROR]: parsing failed at parse filename short, dataType mismatch: " << dataType2 << "/" << dataType << endl;
		return false;
	}
	return true;
}

void Getevtnb(const char *dataType = "", bool PbPb = true, bool pnfs = true, bool inclusive = true)
{
	std::string chain_name = "bTag_AntiKt4HIJets";
	int gridsize = inclusive ? grid_size : s50k_size;
	int JZ_shift = inclusive ? 0 : 1;
	std::string type_ = inclusive ? "" : Type[PbPb];

	std::ofstream fileo(Form("../GetStuff/%s%s_evtnb%s.txt", dataType, type_.c_str(), suffix[pnfs]));
	std::ofstream fileo2(Form("../GetStuff/%s_fileevtnum%s.txt", dataType, suffix[pnfs]));
	std::ofstream fileo3(Form("../GetStuff/%s_filenum%s.txt", dataType, suffix[pnfs]));

	float JZ_wt[gridsize];
	int JZ_FN[gridsize][2];
	int JZ = -1;
	int tag = -1;
	int NUM = -1;
	for (int j = 0; j < gridsize; j++)
	{
		JZ_wt[j] = 0;
		for (int jj = 0; jj < sizeof(JZ_FN[j]) / sizeof(int); jj++)
		{
			//JZ_ID[j][jj] = 0;
			JZ_FN[j][jj] = 0;
		}
	}

	std::ifstream filein(Form("../GetStuff/%s_root%s.txt", dataType, suffix[pnfs]));
	std::string filename;
	int counter = 0;
	while (getline(filein, filename))
	{
		if (counter % 100 == 0)
			cout << "Counted " << counter << " files. At JZ " << JZ << ", tag " << tag << ", NUM " << NUM << endl;
		bool parsed = parse_filename_short(filename, dataType, PbPb, pnfs, inclusive, JZ, tag, NUM, true);
		if (!parsed)
		{
			cout << "[ERROR]: parsing failed" << endl;
			return;
		}
		TFile *f = TFile::Open(filename.c_str(), "READ");
		if (!f)
		{
			cout << "[ERROR]: file opening failed: " << filename << endl;
			return;
		}
		JZ_FN[JZ][tag]++;
		counter++;
		Float_t mcwg;
		TBranch *b_mcwg; //!
		TTree *tree = (TTree *)f->Get(chain_name.c_str());
		if (!tree)
		{
			cout << "[ERROR]: tree not found: " << filename << chain_name << endl;
			return;
		}
		Long64_t nentries = tree->GetEntries();
		fileo2 << JZ + JZ_shift << "_" << tag << "_" << NUM << ": " << nentries << endl;
		if (!inclusive)
		{
			tree->SetBranchAddress("mcwg", &mcwg, &b_mcwg);
			for (int jentry = 0; jentry < nentries; jentry++)
			{
				b_mcwg->GetEntry(jentry);
				JZ_wt[JZ] = JZ_wt[JZ] + mcwg;
			}
		}
		else
		{
			JZ_wt[JZ] = JZ_wt[JZ] + nentries;
		}
		delete tree;
		delete f;
	}

	for (int jz = 0; jz < gridsize; jz++)
	{
		cout << "JZ" << jz + JZ_shift << ": " << JZ_wt[jz] << endl;
		fileo << jz + JZ_shift << ": " << JZ_wt[jz] << endl;
		for (int jj = 0; jj < sizeof(JZ_FN[jz]) / sizeof(int); jj++)
		{
			fileo3 << jz + JZ_shift << "_" << jj << ": " << std::setprecision(15) << JZ_FN[jz][jj] << endl;
			cout << jz + JZ_shift << "_" << jj << ": " << std::setprecision(15) << JZ_FN[jz][jj] << endl;
		}
	}
	fileo.close();
	fileo2.close();
	fileo3.close();
}
