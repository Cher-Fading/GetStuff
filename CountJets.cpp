//#include "InfoHeaders.h"

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
char type_str[nType][10] = {"Failed", "Passed"};

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

void CountJets(std::string trainname, std::string filename)
{
    int stat = 0;
    int cStat = 0;
    int outStat = 0;
    int outcStat = 0;
    float ptLim = 50.;
    float aeta = 2.1;

    bool train_parsed = parse_trainname(trainname, stat, cStat, outStat, outcStat, ptLim, aeta);

    int JZ = -1;
    int tag = -1;
    int NUM = -1;
    bool inclusive = false;
    bool PbPb = false;
    bool pnfs = false;
    std::string dataType = "";
    //cout << filename << endl;

    bool parsed = parse_filename(filename, JZ, tag, NUM, inclusive, PbPb, pnfs, dataType);
    if (!parsed)
    {
        cout << "parsing failed" << endl;
        return;
    }

    std::ofstream fstatout(Form("/atlasgpfs01/usatlas/data/cher97/%s%s_Counts/%s_%d_%d_%d_counts.txt", dataType.c_str(), Type[PbPb], trainname, JZ, tag, NUM));
    if (!fstatout)
    {
        cout << "[ERROR] out file name wrong: " << Form("/atlasgpfs01/usatlas/data/cher97/%s%s_Counts/%s_%d_%d_%d_counts.txt", dataType.c_str(), Type[PbPb], trainname, JZ, tag, NUM) << endl;
        return;
    }

    std::string chain_name = "bTag_AntiKt4HIJets";
    TChain *fChain = new TChain(chain_name.c_str());
    fChain->Add(filename);
    initBranches(fChain);

    //Int_t eventnb;
    //Float_t mcwg;
    //Float_t Fcal;
    Int_t njets = 0;
    std::vector<float> *jet_pt = 0;       //MV2
    std::vector<float> *jet_eta = 0;      //MV2
    std::vector<int> *jet_LabDr_HadF = 0; //label
    //std::vector<float> *jet_nConst;     //cuts not used (nConst > 1)
    std::vector<int> *jet_truthMatch = 0;   //cuts (truthMatch == 1)
    std::vector<int> *jet_aliveAfterOR = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)

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
    cout << "Number of entries: " << nentries << endl;
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
            {
                order[o] = i;
                break;
            }
            else
                order[o] = -1;
        }
    }
    /*for (int o = 0; o < nOrder + 1; o++){
cout << order[o] << endl;
}*/

    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
        //cout << jentry << endl;
        b_njets->GetEntry(jentry);
        b_jet_pt->GetEntry(jentry);
        b_jet_eta->GetEntry(jentry);
        b_jet_LabDr_HadF->GetEntry(jentry); //label
        b_jet_truthMatch->GetEntry(jentry); //cuts (truthMatch == 1)
        b_jet_aliveAfterOR->GetEntry(jentry);
        //cout << njets << endl;
        for (int j = 0; j < njets; j++)
        {
            int flav = jet_LabDr_HadF->at(j);
            if (flav != 0 && flav != 4 && flav != 5)
                continue;
            int passed = jet_pt->at(j) > (ptLim * 1e3) && fabs(jet_eta->at(j)) <= 2.1 && jet_aliveAfterOR->at(j) == 1 && jet_truthMatch->at(j) == 1;
            if (order[flav] < 0)
            {
                cout << flav << endl;
                cout << order[flav] << endl;
                cout << passed << endl;
                cout << "pt (MeV): " << jet_pt->at(j) << "; eta: " << jet_eta->at(j) << "; No electron overlap: " << jet_aliveAfterOR->at(j) << "; truthMatch: " << jet_truthMatch->at(j) << endl;
                return;
            }
            n[order[flav]][passed]++;
        }
    }

    for (int f = 0; f < nFlav; f++)
    {
        int total = 0;
        for (int i = 0; i < nType; i++)
        {
            cout << type_str[i] << " for " << indexF_str[f] << " jet is " << n[f][i] << endl;
            fstatout << indexF_str[f] << " " << type_str[i] << " " << n[f][i] << endl;
            total = total + n[f][i];
        }
        cout << "Total for " << indexF_str[f] << " jet is " << total << endl;
        fstatout << indexF_str[f] << " Total " << total << endl;
    }

    fstatout.close();
}
