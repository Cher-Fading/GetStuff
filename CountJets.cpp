#include "InfoHeaders.h"

#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "Getevtnb.cpp"
#endif

#ifdef __CINT__
gROOT->LoadMacro("Getevtnb.cpp");
#endif

const int nFlav = 3;
const int indexF[nFlav] = {0, 5, 4};
char indexF_str[nFlav][10] = {"Light", "B", "C"};
const int nType = 2;
char type_str[nType][10] = {"Original", "Passed"};


//stat for b and light are the same, ratio accounts for additional bad jets expected based on previous small samples.
//stat for c are some ratio to b/light.
void initBranches(TChain *fChain)
{
    fChain->SetBranchStatus("*", 1);
    //fChain->SetBranchStatus("eventnb", 1);
    //fChain->SetBranchStatus("mcwg", 1);
    fChain->SetBranchStatus("njets", 1);
    //fChain->SetBranchStatus("Fcal", 1);

    fChain->SetBranchStatus("jet_pt", 1);
    fChain->SetBranchStatus("jet_eta", 1);
    fChain->SetBranchStatus("jet_LabDr_HadF", 1);
    fChain->SetBranchStatus("jet_aliveAfterOR", 1);
    fChain->SetBranchStatus("jet_nConst", 1);
    fChain->SetBranchStatus("jet_truthMatch", 1);
}

void CountJets(const char *trainname, const char *filename)
{
    std::ifstream fstat(Form("../GetStuff/%s_stat.txt", trainname));
    std::string line;
    int stat = 0;
    int cStat = 0;
    int outStat = 0;
    int outcStat = 0;
    float ptLim = 50.;
    float aeta = 2.1;
    while (getline(fstat, line))
    {
        TString Line = line.data();
        if (Line.Contains("stat"))
        {
            TString stat_str = line.substr(4, line.length());
            stat = stat_str.ReplaceAll("k", "000").Atoi();
            outStat = stat;
            cStat = stat * 0.5;
            outcStat = cStat;
        }
        if (Line.Contains("cstat"))
        {
            TString cstat_str = line.substr(5, line.length());
            cStat = cstat_str.ReplaceAll("k", "000").Atoi();
            outcStat = cStat;
        }
        if (Line.Contains("badMargin"))
        {
            float ratio = 1 + std::stof(line.substr(9, line.length())) / 100.;
            outStat = ratio * stat;
            outcStat = ratio * cStat;
        }
        if (Line.Contains("pt"))
        {
            float ptLim = std::stof(line.substr(2, line.length()));
        }
        if (Line.Contains("aeta"))
        {
            float aeta = std::stof(line.substr(4, line.length())) / 10.;
        }
    }

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
		return;
	}


    std::ofstream fstatout(Form("/atlasgpfs01/usatlas/data/cher97/%s%s_Counts/%s_%d_%d_%d_counts.txt", dataType.c_str(), Type[PbPb], trainname, JZ, tag, NUM));

    std::string chain_name = "bTag_AntiKt4HIJets";
    TChain *fChain = new TChain(chain_name.c_str());
    initBranches(fChain);

    //Int_t eventnb;
    //Float_t mcwg;
    //Float_t Fcal;
    Int_t njets;
    std::vector<float> *jet_pt = 0;   //MV2
    std::vector<float> *jet_eta;      //MV2
    std::vector<int> *jet_LabDr_HadF; //label
    //std::vector<float> *jet_nConst;     //cuts not used (nConst > 1)
    std::vector<int> *jet_truthMatch;   //cuts (truthMatch == 1)
    std::vector<int> *jet_aliveAfterOR; //cuts (aliveAfterOR == 1) (no overlap with electron)

    //TBranch *b_eventnb;
    //TBranch *b_mcwg;
    //TBranch *b_Fcal;
    TBranch *b_njets;
    TBranch *b_jet_pt;
    TBranch *b_jet_eta;
    TBranch *b_jet_LabDr_HadF; //label
    //TBranch *b_jet_nConst;       //cuts not used (nConst > 1)
    TBranch *b_jet_truthMatch;   //cuts (truthMatch == 1)
    TBranch *b_jet_aliveAfterOR; //cuts (aliveAfterOR == 1) (no overlap with electron)

    //fChain->SetBranchAddress("eventnb", &eventnb, &b_eventnb);
    //fChain->SetBranchAddress("mcwg", &mcwg, &b_mcwg);
    //fChain->SetBranchStatus("mcwg", 1);

    fChain->SetBranchAddress("njets", &njets, &b_njets);
    //fChain->SetBranchAddress("Fcal", &Fcal, &b_Fcal);

    fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
    fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
    fChain->SetBranchAddress("jet_LabDr_HadF", &jet_LabDr_HadF, &b_jet_LabDr_HadF);
    fChain->SetBranchAddress("jet_aliveAfterOR", &jet_aliveAfterOR, &b_jet_aliveAfterOR);
    //fChain->SetBranchAddress("jet_nConst", &jet_nConst, &b_jet_nConst);
    fChain->SetBranchAddress("jet_truthMatch", &jet_truthMatch, &b_jet_truthMatch);

    Long64_t nentries = fChain->GetEntries();
    int n[nFlav][nType];
    for (int f = 0; f < nFlav; f++)
    {
        for (int t = 0; t < nType; t++)
        {
            n[f][t] = 0;
        }
    }
    int nOrder = 0;
    for (int i = 0; i < nFlav; i++)
    {
        nOrder = nOrder >= indexF[i] ? nOrder : indexF[i];
    }
    int order[nOrder + 1];
    for (int o = 0; o < nOrder + 1; o++)
    {
        for (int i = 0; i < nFlav; i++)
        {
            if (o == indexF[i])
                order[o] = i;
            else
                order[o] = -1;
        }
    }

    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
        b_njets->GetEntry(jentry);
        b_jet_pt->GetEntry(jentry);
        b_jet_eta->GetEntry(jentry);
        b_jet_LabDr_HadF->GetEntry(jentry); //label
        b_jet_truthMatch->GetEntry(jentry); //cuts (truthMatch == 1)
        b_jet_aliveAfterOR->GetEntry(jentry);
        for (int j = 0; j < njets; j++)
        {
            int flav = jet_LabDr_HadF->at(j);
            if (flav != 0 && flav != 4 && flav != 5)
                continue;
            int passed = jet_pt->at(j) > (ptLim * 1e3) && fabs(jet_eta->at(j)) <= 2.1 && jet_aliveAfterOR->at(j) == 1 && jet_truthMatch->at(j) == 1;
            n[order[flav]][passed]++;
        }
    }

    for (int f = 0; f < nFlav; f++)
    {
        for (int i = 0; i < nType; i++)
        {
            cout << type_str[i] << " for " << indexF_str[f] << " jet is " << n[f][i] << endl;
            fstatout << indexF_str[f] << " " << type_str[i] << " " << n[f][i] << endl;
        }
    }

    fstatout.close();
}