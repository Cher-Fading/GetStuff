#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "Getevtnb.cpp"
#endif

#ifdef __CINT__
gROOT->LoadMacro("Getevtnb.cpp");
#endif
const char *chain_name = "bTag_AntiKt4HIJets";

void initBranches(TChain *fChain)
{
    fChain->SetBranchStatus("*", 1);
    fChain->SetBranchStatus("eventnb", 1);
    //fChain->SetBranchStatus("mcwg", 1);
    fChain->SetBranchStatus("njets", 1);
    fChain->SetBranchStatus("Fcal", 1);

    fChain->SetBranchStatus("jet_pt", 1);
    fChain->SetBranchStatus("jet_eta", 1);
    fChain->SetBranchStatus("jet_LabDr_HadF", 1);
    fChain->SetBranchStatus("jet_aliveAfterOR", 1);
    fChain->SetBranchStatus("jet_nConst", 1);
    fChain->SetBranchStatus("jet_truthMatch", 1);

    //fChain->SetBranchStatus("jet_ip2d_llr", &jet_ip2d_llr, &b_jet_ip2d_llr);
    fChain->SetBranchStatus("jet_sv1_ntrkv", 1);
    fChain->SetBranchStatus("jet_sv1_n2t", 1);
    fChain->SetBranchStatus("jet_sv1_m", 1);
    fChain->SetBranchStatus("jet_sv1_efc", 1);
    //fChain->SetBranchStatus("jet_sv1_normdist", &jet_sv1_normdist, &b_jet_sv1_normdist);
    fChain->SetBranchStatus("jet_sv1_Lxy", 1);
    fChain->SetBranchStatus("jet_sv1_L3d", 1);
    fChain->SetBranchStatus("jet_sv1_deltaR", 1);
    fChain->SetBranchStatus("jet_sv1_sig3d", 1);

    fChain->SetBranchStatus("jet_jf_m", 1);
    fChain->SetBranchStatus("jet_jf_efc", 1);
    fChain->SetBranchStatus("jet_jf_deta", 1);
    fChain->SetBranchStatus("jet_jf_dphi", 1);
    fChain->SetBranchStatus("jet_jf_ntrkAtVx", 1);
    fChain->SetBranchStatus("jet_jf_nvtx", 1);
    fChain->SetBranchStatus("jet_jf_sig3d", 1);
    fChain->SetBranchStatus("jet_jf_nvtx1t", 1);
    fChain->SetBranchStatus("jet_jf_n2t", 1);

    fChain->SetBranchStatus("jet_ip2d_pb", 1);
    fChain->SetBranchStatus("jet_ip2d_pc", 1);
    fChain->SetBranchStatus("jet_ip2d_pu", 1);
    fChain->SetBranchStatus("jet_ip3d_pb", 1);
    fChain->SetBranchStatus("jet_ip3d_pc", 1);
    fChain->SetBranchStatus("jet_ip3d_pu", 1);
}

void makeSTree(std::string trainname, std::string filename, const char *outputFolder)
{

    gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");
    gInterpreter->GenerateDictionary("vector<vector<int> >", "vector");

    float stat = 0;
    float cStat = 0;
    float outStat = 0;
    float outcStat = 0;
    float ptLim = 50.;
    float aeta = 2.1;

    bool train_parsed = parse_trainname(trainname, stat, cStat, outStat, outcStat, ptLim, aeta);
    cout << "pT: " << ptLim << endl;
    cout << "eta: " << aeta << endl;
    if (!train_parsed)
    {
        cout << "[ERROR]: parsing failed" << endl;
        return;
    }

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
        cout << "[ERROR]: parsing failed" << endl;
        return;
    }
    std::string line2;
    std::vector<float> jets_count;
    std::vector<float> jets_total;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            jets_count.push_back(0.);
            jets_total.push_back(0.);
        }
    }
    //order: 0light fail, 1light pass, 2light total, 3b fail, 4b pass, 5b total, 6c fail, 7c pass, 8c total
    bool parsed_count = parse_count(Form("/atlasgpfs01/usatlas/data/cher97/%s%s_Counts%s/%s_%d_%d_%d_counts.txt", dataType.c_str(), Type[PbPb], suffix[pnfs], trainname.c_str(), JZ, tag, NUM), jets_count);
    if (!parsed_count)
    {
        cout << "[ERROR]: parsing failed for individual count" << endl;
        return;
    }

    bool parsed_total = parse_count(Form("../GetStuff/%s_totaljets%s.txt", dataType.c_str(), suffix[pnfs]), jets_total);
    if (!parsed_total)
    {
        cout << "[ERROR]: parsing failed for total count" << endl;
        return;
    }
    int stat_small = (int)round((jets_count[1] / jets_total[1] * stat));
    int stat_small_b = (int)round((jets_count[4] / jets_total[4] * stat));
    int cStat_small = (int)round((jets_count[7] / jets_total[7] * cStat));
    cout << "Need the following stat: "
         << "Light: " << stat_small << " B: " << stat_small_b << " C: " << cStat_small << endl;

<<<<<<< HEAD
=======
    if (stat_small == 0 && stat_small_b == 0 && cStat_small == 0)
    {
        cout << "[WARNING]: No jets pass in this file, pass file" << endl;
        return;
    }

    int b = 0;
    int light = 0;
    int c = 0;
>>>>>>> f3a4742c70c4ce7b3a845b479a30a6167f8d7be7
    TFile *fout = new TFile(Form("%s/%d_%d_%d_small.root", outputFolder, JZ, tag, NUM), "RECREATE");
    TTree *f_new = new TTree(chain_name, chain_name);

    Int_t m_eventnb;
    Float_t m_mcwg;
    Float_t m_Fcal;
    Int_t m_njets;
    std::vector<float> *m_jet_pt = 0;         //MV2
    std::vector<float> *m_jet_eta = 0;        //MV2
    std::vector<int> *m_jet_LabDr_HadF = 0;   //label
    std::vector<float> *m_jet_nConst = 0;     //cuts not used (nConst > 1)
    std::vector<int> *m_jet_truthMatch = 0;   //cuts (truthMatch == 1)
    std::vector<int> *m_jet_aliveAfterOR = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    std::vector<float> *m_jet_jf_m = 0;
    std::vector<float> *m_jet_jf_efc = 0;
    std::vector<float> *m_jet_jf_deta = 0; // for jet_jf_dR
    std::vector<float> *m_jet_jf_dphi = 0; // for jet_jf_dR
    std::vector<float> *m_jet_jf_nvtx = 0;
    std::vector<float> *m_jet_jf_sig3d = 0;
    std::vector<float> *m_jet_jf_nvtx1t = 0;
    std::vector<float> *m_jet_jf_n2t = 0;
    std::vector<float> *m_jet_jf_ntrkAtVx = 0;

    // ** SV1 Variable (8) ** //
    std::vector<float> *m_jet_sv1_ntrkv = 0;
    std::vector<float> *m_jet_sv1_n2t = 0;
    std::vector<float> *m_jet_sv1_m = 0;
    std::vector<float> *m_jet_sv1_efc = 0;
    std::vector<float> *m_jet_sv1_sig3d = 0;
    std::vector<float> *m_jet_sv1_Lxy = 0;
    std::vector<float> *m_jet_sv1_deltaR = 0;
    std::vector<float> *m_jet_sv1_L3d = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    std::vector<float> *m_jet_ip2d_pb = 0;
    std::vector<float> *m_jet_ip2d_pc = 0;
    std::vector<float> *m_jet_ip2d_pu = 0; //cuts & c_weight
    std::vector<float> *m_jet_ip3d_pb = 0;
    std::vector<float> *m_jet_ip3d_pc = 0;
    std::vector<float> *m_jet_ip3d_pu = 0;

    f_new->Branch("eventnb", &m_eventnb);
    f_new->Branch("mcwg", &m_mcwg);
    f_new->Branch("njets", &m_njets);
    f_new->Branch("Fcal", &m_Fcal);

    f_new->Branch("jet_pt", &m_jet_pt);
    //cout << m_jet_pt << endl;
    f_new->Branch("jet_eta", &m_jet_eta);
    f_new->Branch("jet_LabDr_HadF", &m_jet_LabDr_HadF);
    f_new->Branch("jet_aliveAfterOR", &m_jet_aliveAfterOR);
    f_new->Branch("jet_nConst", &m_jet_nConst);
    f_new->Branch("jet_truthMatch", &m_jet_truthMatch);
    //f_new->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new->Branch("jet_sv1_ntrkv", &m_jet_sv1_ntrkv);
    f_new->Branch("jet_sv1_n2t", &m_jet_sv1_n2t);
    f_new->Branch("jet_sv1_m", &m_jet_sv1_m);
    f_new->Branch("jet_sv1_efc", &m_jet_sv1_efc);
    //f_new->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new->Branch("jet_sv1_Lxy", &m_jet_sv1_Lxy);
    f_new->Branch("jet_sv1_L3d", &m_jet_sv1_L3d);
    f_new->Branch("jet_sv1_deltaR", &m_jet_sv1_deltaR);
    f_new->Branch("jet_sv1_sig3d", &m_jet_sv1_sig3d);

    f_new->Branch("jet_jf_m", &m_jet_jf_m);
    f_new->Branch("jet_jf_efc", &m_jet_jf_efc);
    f_new->Branch("jet_jf_deta", &m_jet_jf_deta);
    f_new->Branch("jet_jf_dphi", &m_jet_jf_dphi);
    f_new->Branch("jet_jf_ntrkAtVx", &m_jet_jf_ntrkAtVx);
    f_new->Branch("jet_jf_nvtx", &m_jet_jf_nvtx);
    f_new->Branch("jet_jf_sig3d", &m_jet_jf_sig3d);
    f_new->Branch("jet_jf_nvtx1t", &m_jet_jf_nvtx1t);
    f_new->Branch("jet_jf_n2t", &m_jet_jf_n2t);

    f_new->Branch("jet_ip2d_pb", &m_jet_ip2d_pb);
    f_new->Branch("jet_ip2d_pc", &m_jet_ip2d_pc);
    f_new->Branch("jet_ip2d_pu", &m_jet_ip2d_pu);
    f_new->Branch("jet_ip3d_pb", &m_jet_ip3d_pb);
    f_new->Branch("jet_ip3d_pc", &m_jet_ip3d_pc);
    f_new->Branch("jet_ip3d_pu", &m_jet_ip3d_pu);

if (stat_small == 0 && stat_small_b == 0 && cStat_small == 0)
    {
        cout << "[WARNING]: No jets pass in this file, pass file" << endl;
f_new->Write();
fout->Close();
        return;
    }

    //Int_t           runnb;
    Int_t eventnb;
    Float_t mcwg;
    Float_t Fcal;
    Int_t njets;
    std::vector<float> *jet_pt = 0;         //MV2
    std::vector<float> *jet_eta = 0;        //MV2
    std::vector<int> *jet_LabDr_HadF = 0;   //label
    std::vector<float> *jet_nConst = 0;     //cuts not used (nConst > 1)
    std::vector<int> *jet_truthMatch = 0;   //cuts (truthMatch == 1)
    std::vector<int> *jet_aliveAfterOR = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    std::vector<float> *jet_jf_m = 0;
    std::vector<float> *jet_jf_efc = 0;
    std::vector<float> *jet_jf_deta = 0; // for jet_jf_dR
    std::vector<float> *jet_jf_dphi = 0; // for jet_jf_dR
    std::vector<float> *jet_jf_nvtx = 0;
    std::vector<float> *jet_jf_sig3d = 0;
    std::vector<float> *jet_jf_nvtx1t = 0;
    std::vector<float> *jet_jf_n2t = 0;
    std::vector<float> *jet_jf_ntrkAtVx = 0;

    // ** SV1 Variable (8) ** //
    std::vector<float> *jet_sv1_ntrkv = 0;
    std::vector<float> *jet_sv1_n2t = 0;
    std::vector<float> *jet_sv1_m = 0;
    std::vector<float> *jet_sv1_efc = 0;
    std::vector<float> *jet_sv1_sig3d = 0;
    std::vector<float> *jet_sv1_Lxy = 0;
    std::vector<float> *jet_sv1_deltaR = 0;
    std::vector<float> *jet_sv1_L3d = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    std::vector<float> *jet_ip2d_pb = 0;
    std::vector<float> *jet_ip2d_pc = 0;
    std::vector<float> *jet_ip2d_pu = 0; //cuts & c_weight
    std::vector<float> *jet_ip3d_pb = 0;
    std::vector<float> *jet_ip3d_pc = 0;
    std::vector<float> *jet_ip3d_pu = 0;

    TBranch *b_eventnb;
    TBranch *b_mcwg;
    TBranch *b_Fcal;
    TBranch *b_njets;
    TBranch *b_jet_pt;
    TBranch *b_jet_eta;
    TBranch *b_jet_LabDr_HadF;   //label
    TBranch *b_jet_nConst;       //cuts not used (nConst > 1)
    TBranch *b_jet_truthMatch;   //cuts (truthMatch == 1)
    TBranch *b_jet_aliveAfterOR; //cuts (aliveAfterOR == 1) (no overlap with electron)
    TBranch *b_jet_jf_m;
    TBranch *b_jet_jf_efc;
    TBranch *b_jet_jf_deta; // for jet_jf_dR
    TBranch *b_jet_jf_dphi; // for jet_jf_dR
    TBranch *b_jet_jf_nvtx;
    TBranch *b_jet_jf_sig3d;
    TBranch *b_jet_jf_nvtx1t;
    TBranch *b_jet_jf_n2t;
    TBranch *b_jet_jf_ntrkAtVx;

    TBranch *b_jet_sv1_ntrkv;
    TBranch *b_jet_sv1_n2t;
    TBranch *b_jet_sv1_m;
    TBranch *b_jet_sv1_efc;
    TBranch *b_jet_sv1_sig3d;
    TBranch *b_jet_sv1_Lxy;
    TBranch *b_jet_sv1_deltaR;
    TBranch *b_jet_sv1_L3d;

    TBranch *b_jet_ip2d_pu; //cuts & c_weight
    TBranch *b_jet_ip2d_pb;
    TBranch *b_jet_ip2d_pc;
    TBranch *b_jet_ip3d_pu;
    TBranch *b_jet_ip3d_pb;
    TBranch *b_jet_ip3d_pc;

    TChain *fChain = new TChain(chain_name);
    fChain->Add(filename.c_str());
    initBranches(fChain);

    fChain->SetBranchAddress("eventnb", &eventnb, &b_eventnb);
    if (!inclusive)
    {
        fChain->SetBranchAddress("mcwg", &mcwg, &b_mcwg);
        fChain->SetBranchStatus("mcwg", 1);
    }
    fChain->SetBranchAddress("njets", &njets, &b_njets);
    fChain->SetBranchAddress("Fcal", &Fcal, &b_Fcal);

    fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
    fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
    fChain->SetBranchAddress("jet_LabDr_HadF", &jet_LabDr_HadF, &b_jet_LabDr_HadF);
    fChain->SetBranchAddress("jet_aliveAfterOR", &jet_aliveAfterOR, &b_jet_aliveAfterOR);
    fChain->SetBranchAddress("jet_nConst", &jet_nConst, &b_jet_nConst);
    fChain->SetBranchAddress("jet_truthMatch", &jet_truthMatch, &b_jet_truthMatch);

    //fChain->SetBranchAddress("jet_ip2d_llr", &jet_ip2d_llr, &b_jet_ip2d_llr);
    fChain->SetBranchAddress("jet_sv1_ntrkv", &jet_sv1_ntrkv, &b_jet_sv1_ntrkv);
    fChain->SetBranchAddress("jet_sv1_n2t", &jet_sv1_n2t, &b_jet_sv1_n2t);
    fChain->SetBranchAddress("jet_sv1_m", &jet_sv1_m, &b_jet_sv1_m);
    fChain->SetBranchAddress("jet_sv1_efc", &jet_sv1_efc, &b_jet_sv1_efc);
    //fChain->SetBranchAddress("jet_sv1_normdist", &jet_sv1_normdist, &b_jet_sv1_normdist);
    fChain->SetBranchAddress("jet_sv1_Lxy", &jet_sv1_Lxy, &b_jet_sv1_Lxy);
    fChain->SetBranchAddress("jet_sv1_L3d", &jet_sv1_L3d, &b_jet_sv1_L3d);
    fChain->SetBranchAddress("jet_sv1_deltaR", &jet_sv1_deltaR, &b_jet_sv1_deltaR);
    fChain->SetBranchAddress("jet_sv1_sig3d", &jet_sv1_sig3d, &b_jet_sv1_sig3d);

    fChain->SetBranchAddress("jet_jf_m", &jet_jf_m, &b_jet_jf_m);
    fChain->SetBranchAddress("jet_jf_efc", &jet_jf_efc, &b_jet_jf_efc);
    fChain->SetBranchAddress("jet_jf_deta", &jet_jf_deta, &b_jet_jf_deta);
    fChain->SetBranchAddress("jet_jf_dphi", &jet_jf_dphi, &b_jet_jf_dphi);
    fChain->SetBranchAddress("jet_jf_ntrkAtVx", &jet_jf_ntrkAtVx, &b_jet_jf_ntrkAtVx);
    fChain->SetBranchAddress("jet_jf_nvtx", &jet_jf_nvtx, &b_jet_jf_nvtx);
    fChain->SetBranchAddress("jet_jf_sig3d", &jet_jf_sig3d, &b_jet_jf_sig3d);
    fChain->SetBranchAddress("jet_jf_nvtx1t", &jet_jf_nvtx1t, &b_jet_jf_nvtx1t);
    fChain->SetBranchAddress("jet_jf_n2t", &jet_jf_n2t, &b_jet_jf_n2t);

    fChain->SetBranchAddress("jet_ip2d_pb", &jet_ip2d_pb, &b_jet_ip2d_pb);
    fChain->SetBranchAddress("jet_ip2d_pc", &jet_ip2d_pc, &b_jet_ip2d_pc);
    fChain->SetBranchAddress("jet_ip2d_pu", &jet_ip2d_pu, &b_jet_ip2d_pu);
    fChain->SetBranchAddress("jet_ip3d_pb", &jet_ip3d_pb, &b_jet_ip3d_pb);
    fChain->SetBranchAddress("jet_ip3d_pc", &jet_ip3d_pc, &b_jet_ip3d_pc);
    fChain->SetBranchAddress("jet_ip3d_pu", &jet_ip3d_pu, &b_jet_ip3d_pu);

    Long64_t nentries = fChain->GetEntries();
    cout << "Entries: " << nentries << endl;
    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
        //if (jentry > 100) break;
        if (b >= stat_small_b && light >= stat_small && c >= cStat_small)
            break;
        fChain->GetEntry(jentry);
        /*b_eventnb->GetEntry(jentry);

        b_Fcal->GetEntry(jentry);
        b_njets->GetEntry(jentry);*/
        //cout << eventnb << endl;
        m_njets = njets;
        m_eventnb = eventnb;
        m_Fcal = Fcal;
        //cout << Fcal << endl;
        m_mcwg = 1;
        if (!inclusive)
        {
            //b_mcwg->GetEntry(jentry);
            //cout << mcwg << endl;
            m_mcwg = mcwg;
        }
        //cout << njets << endl;

        /*b_jet_pt->GetEntry(jentry);
        b_jet_eta->GetEntry(jentry);
        b_jet_LabDr_HadF->GetEntry(jentry);   //label
        b_jet_nConst->GetEntry(jentry);       //cuts not used (nConst > 1)
        b_jet_truthMatch->GetEntry(jentry);   //cuts (truthMatch == 1)
        b_jet_aliveAfterOR->GetEntry(jentry); //cuts (aliveAfterOR == 1) (no overlap with electron)
        b_jet_jf_m->GetEntry(jentry);
        b_jet_jf_efc->GetEntry(jentry);
        b_jet_jf_deta->GetEntry(jentry); // for jet_jf_dR
        b_jet_jf_dphi->GetEntry(jentry); // for jet_jf_dR
        b_jet_jf_nvtx->GetEntry(jentry);
        b_jet_jf_sig3d->GetEntry(jentry);
        b_jet_jf_nvtx1t->GetEntry(jentry);
        b_jet_jf_n2t->GetEntry(jentry);
        b_jet_jf_ntrkAtVx->GetEntry(jentry);

        b_jet_sv1_ntrkv->GetEntry(jentry);
        b_jet_sv1_n2t->GetEntry(jentry);
        b_jet_sv1_m->GetEntry(jentry);
        b_jet_sv1_efc->GetEntry(jentry);
        b_jet_sv1_sig3d->GetEntry(jentry);
        b_jet_sv1_Lxy->GetEntry(jentry);
        b_jet_sv1_deltaR->GetEntry(jentry);
        b_jet_sv1_L3d->GetEntry(jentry);

        b_jet_ip2d_pu->GetEntry(jentry); //cuts & c_weight
        b_jet_ip2d_pb->GetEntry(jentry);
        b_jet_ip2d_pc->GetEntry(jentry);
        b_jet_ip3d_pu->GetEntry(jentry);
        b_jet_ip3d_pb->GetEntry(jentry);
        b_jet_ip3d_pc->GetEntry(jentry);*/
        //int Njets = 0;
        for (int i = 0; i < njets; i++)
        {
            if (b >= stat_small_b && light >= stat_small && c >= cStat_small)
                break;

            //cout << jet_pt->at(i) << endl;
            //cout << jet_eta->at(i) << endl;
            //cout << jet_truthMatch->at(i) << endl;
            //cout << jet_aliveAfterOR->at(i) << endl;
            //cout << !(jet_pt->at(i) >= ptLim * 1e3 && fabs(jet_eta->at(i)) <= 2.1 && jet_truthMatch->at(i) == 1 && jet_aliveAfterOR->at(i) == 1) << endl;
            if (!(jet_pt->at(i) >= ptLim * 1e3 && fabs(jet_eta->at(i)) <= 2.1 && jet_truthMatch->at(i) == 1 && jet_aliveAfterOR->at(i) == 1))
                continue;
            //cout << (jet_LabDr_HadF->at(i) != 0 || jet_LabDr_HadF->at(i) != 4 || jet_LabDr_HadF->at(i) != 5) << endl;
            //cout << jet_LabDr_HadF->at(i) << endl;
            if (jet_LabDr_HadF->at(i) != 0 && jet_LabDr_HadF->at(i) != 4 && jet_LabDr_HadF->at(i) != 5)
                continue;
            if (jet_LabDr_HadF->at(i) == 5 && b >= stat_small_b)
                continue;
            if (jet_LabDr_HadF->at(i) == 4 && c >= cStat_small)
                continue;
            if (jet_LabDr_HadF->at(i) == 0 && light >= stat_small)
                continue;

            b += jet_LabDr_HadF->at(i) == 5;
            c += jet_LabDr_HadF->at(i) == 4;
            light += jet_LabDr_HadF->at(i) == 0;
            //cout << jet_pt->at(i) << endl;
            cout << "b: " << b << "c: " << c << "u: " << light << endl;
            m_jet_pt->push_back(jet_pt->at(i));

            //cout << m_jet_pt->at(Njets) << endl;
            //Njets++;
            m_jet_eta->push_back(jet_eta->at(i));
            m_jet_LabDr_HadF->push_back(jet_LabDr_HadF->at(i));
            m_jet_nConst->push_back(jet_nConst->at(i));
            m_jet_truthMatch->push_back(jet_truthMatch->at(i));
            m_jet_aliveAfterOR->push_back(jet_aliveAfterOR->at(i));

            // ** JetFitter Variables (8) ** //
            m_jet_jf_m->push_back(jet_jf_m->at(i));
            m_jet_jf_efc->push_back(jet_jf_efc->at(i));
            m_jet_jf_deta->push_back(jet_jf_deta->at(i));
            m_jet_jf_dphi->push_back(jet_jf_dphi->at(i));
            m_jet_jf_nvtx->push_back(jet_jf_nvtx->at(i));
            m_jet_jf_sig3d->push_back(jet_jf_sig3d->at(i));
            m_jet_jf_nvtx1t->push_back(jet_jf_nvtx1t->at(i));
            m_jet_jf_n2t->push_back(jet_jf_n2t->at(i));
            m_jet_jf_ntrkAtVx->push_back(jet_jf_ntrkAtVx->at(i));

            // ** SV1 Variable (8) ** //
            m_jet_sv1_ntrkv->push_back(jet_sv1_ntrkv->at(i));
            m_jet_sv1_n2t->push_back(jet_sv1_n2t->at(i));
            m_jet_sv1_m->push_back(jet_sv1_m->at(i));
            m_jet_sv1_efc->push_back(jet_sv1_efc->at(i));
            m_jet_sv1_sig3d->push_back(jet_sv1_sig3d->at(i));
            m_jet_sv1_Lxy->push_back(jet_sv1_Lxy->at(i));
            m_jet_sv1_deltaR->push_back(jet_sv1_deltaR->at(i));
            m_jet_sv1_L3d->push_back(jet_sv1_L3d->at(i));

            //other MV2 variables ip_x,ip_x_c,ip_x_cu
            m_jet_ip2d_pb->push_back(jet_ip2d_pb->at(i));
            m_jet_ip2d_pc->push_back(jet_ip2d_pc->at(i));
            m_jet_ip2d_pu->push_back(jet_ip2d_pu->at(i));
            m_jet_ip3d_pb->push_back(jet_ip3d_pb->at(i));
            m_jet_ip3d_pc->push_back(jet_ip3d_pc->at(i));
            m_jet_ip3d_pu->push_back(jet_ip3d_pu->at(i));
        }

        /*m_jet_pt = jet_pt;
        m_jet_eta = jet_eta;
        m_jet_LabDr_HadF = jet_LabDr_HadF;
        m_jet_nConst = jet_nConst;
        m_jet_truthMatch = jet_truthMatch;
        m_jet_aliveAfterOR = jet_aliveAfterOR;

        // ** JetFitter Variables (8) ** //
        m_jet_jf_m = jet_jf_m;
        m_jet_jf_efc = jet_jf_efc;
        m_jet_jf_deta = jet_jf_deta;
        m_jet_jf_dphi = jet_jf_dphi;
        m_jet_jf_nvtx = jet_jf_nvtx;
        m_jet_jf_sig3d = jet_jf_sig3d;
        m_jet_jf_nvtx1t = jet_jf_nvtx1t;
        m_jet_jf_n2t = jet_jf_n2t;
        m_jet_jf_ntrkAtVx = jet_jf_ntrkAtVx;

        // ** SV1 Variable (8) ** //
        m_jet_sv1_ntrkv = jet_sv1_ntrkv;
        m_jet_sv1_n2t = jet_sv1_n2t;
        m_jet_sv1_m = jet_sv1_m;
        m_jet_sv1_efc = jet_sv1_efc;
        m_jet_sv1_sig3d = jet_sv1_sig3d;
        m_jet_sv1_Lxy = jet_sv1_Lxy;
        m_jet_sv1_deltaR = jet_sv1_deltaR;
        m_jet_sv1_L3d = jet_sv1_L3d;

        //other MV2 variables ip_x,ip_x_c,ip_x_cu
        m_jet_ip2d_pb = jet_ip2d_pb;
        m_jet_ip2d_pc = jet_ip2d_pc;
        m_jet_ip2d_pu = jet_ip2d_pu;
        m_jet_ip3d_pb = jet_ip3d_pb;
        m_jet_ip3d_pc = jet_ip3d_pc;
        m_jet_ip3d_pu = jet_ip3d_pu;*/

        f_new->Fill();
        m_jet_pt->clear();
        m_jet_eta->clear();
        m_jet_LabDr_HadF->clear();
        m_jet_nConst->clear();
        m_jet_truthMatch->clear();
        m_jet_aliveAfterOR->clear();

        // ** JetFitter Variables (8) ** //
        m_jet_jf_m->clear();
        m_jet_jf_efc->clear();
        m_jet_jf_deta->clear();
        m_jet_jf_dphi->clear();
        m_jet_jf_nvtx->clear();
        m_jet_jf_sig3d->clear();
        m_jet_jf_nvtx1t->clear();
        m_jet_jf_n2t->clear();
        m_jet_jf_ntrkAtVx->clear();

        // ** SV1 Variable (8) ** //
        m_jet_sv1_ntrkv->clear();
        m_jet_sv1_n2t->clear();
        m_jet_sv1_m->clear();
        m_jet_sv1_efc->clear();
        m_jet_sv1_sig3d->clear();
        m_jet_sv1_Lxy->clear();
        m_jet_sv1_deltaR->clear();
        m_jet_sv1_L3d->clear();

        //other MV2 variables ip_x,ip_x_c,ip_x_cu
        m_jet_ip2d_pb->clear();
        m_jet_ip2d_pc->clear();
        m_jet_ip2d_pu->clear();
        m_jet_ip3d_pb->clear();
        m_jet_ip3d_pc->clear();
        m_jet_ip3d_pu->clear();
    }
    f_new->Write();
    fout->Close();
}
