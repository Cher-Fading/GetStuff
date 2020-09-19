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

void makeSTrees(std::string trainname, std::string filename, const char *outputFolder, int central = 0)
{

    gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");
    gInterpreter->GenerateDictionary("vector<vector<int> >", "vector");
    bool cut = true; //not filtered samples
    if (filename.find("_small.root") != std::string::npos)
        cut = false; //already filtered samples

    float stat = 0;
    float cStat = 0;
    float outStat = 0;
    float outcStat = 0;
    float ptLim = 50.;
    float aeta = 2.1;
    int JZ = -1;
    int tag = -1;
    int NUM = -1;
    bool inclusive = false;
    bool PbPb = false;
    bool pnfs = false;
    //int central = -1;
    std::string dataType = "";
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

    bool train_parsed = parse_trainname(trainname, stat, cStat, outStat, outcStat, ptLim, aeta);
    cout << "pT: " << ptLim << endl;
    cout << "eta: " << aeta << endl;
    if (!train_parsed)
    {
        cout << "[ERROR]: parsing failed" << endl;
        return;
    }
    if (cut)
    {
        bool parsed = parse_filename(filename, JZ, tag, NUM, inclusive, PbPb, pnfs, dataType);
        if (!parsed)
        {
            cout << "[ERROR]: parsing failed" << endl;
            return;
        }
        std::string line2;
    }

    else
    {
        int start = filename.rfind("/") + 1;
        JZ = std::stoi(filename.substr(start, 1));
        cout << line0.substr(7-shift, 1) << endl;
        tag = std::stoi(filename.substr(start + 2, 1));
        PbPb = (filename[start-8]=="b");
        int cet_length = PbPb ? 2 : 0;
        cout << line0.substr(9-shift, line0.length() - (20-shift) - cet_length) << endl;
        NUM = std::stoi(filename.substr(start+4, filename.length() - start - cet_length - 4 - 11));
        int shift = PbPb?4:2;
        dataType = filename.substr(filename.rfind("cher97/")+7, filename.find("_small/")-(filename.rfind("cher97/")+7)-shift);
        cout << dataType << endl;
        //cout << line0.substr(line0.length() - 12, 1) << endl;
        //central = PbPb ? std::stoi(filename.substr(filename.length() - 12, 1)) : 0;
    }
    return;
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

    if (stat_small == 0 && stat_small_b == 0 && cStat_small == 0)
    {
        cout << "[WARNING]: No jets pass in this file, pass file" << endl;
        return;
    }

    int b = 0;
    int light = 0;
    int c = 0;

    TFile *fout = new TFile(Form("%s/%d_%d_%d_smalls.root", outputFolder, JZ, tag, NUM,Centrality), "RECREATE");
    TTree *f_new_b_u = new TTree(Form("%s_u", chain_name.c_str()), Form("%s_u", chain_name.c_str()));
    TTree *f_new_b_b = new TTree(Form("%s_b", chain_name.c_str()), Form("%s_b", chain_name.c_str()));
    TTree *f_new_b_c = new TTree(Form("%s_c", chain_name.c_str()), Form("%s_c", chain_name.c_str()));

    Int_t m_eventnb_u;
    Float_t m_mcwg_u;
    Float_t m_Fcal_u;
    Int_t m_njets_u;
    std::vector<float> *m_jet_pt_u = 0;         //MV2
    std::vector<float> *m_jet_eta_u = 0;        //MV2
    std::vector<int> *m_jet_LabDr_HadF_u = 0;   //label
    std::vector<float> *m_jet_nConst_u = 0;     //cuts not used (nConst > 1)
    std::vector<int> *m_jet_truthMatch_u = 0;   //cuts (truthMatch == 1)
    std::vector<int> *m_jet_aliveAfterOR_u = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    std::vector<float> *m_jet_jf_m_u = 0;
    std::vector<float> *m_jet_jf_efc_u = 0;
    std::vector<float> *m_jet_jf_deta_u = 0; // for jet_jf_dR
    std::vector<float> *m_jet_jf_dphi_u = 0; // for jet_jf_dR
    std::vector<float> *m_jet_jf_nvtx_u = 0;
    std::vector<float> *m_jet_jf_sig3d_u = 0;
    std::vector<float> *m_jet_jf_nvtx1t_u = 0;
    std::vector<float> *m_jet_jf_n2t_u = 0;
    std::vector<float> *m_jet_jf_ntrkAtVx_u = 0;

    // ** SV1 Variable (8) ** //
    std::vector<float> *m_jet_sv1_ntrkv_u = 0;
    std::vector<float> *m_jet_sv1_n2t_u = 0;
    std::vector<float> *m_jet_sv1_m_u = 0;
    std::vector<float> *m_jet_sv1_efc_u = 0;
    std::vector<float> *m_jet_sv1_sig3d_u = 0;
    std::vector<float> *m_jet_sv1_Lxy_u = 0;
    std::vector<float> *m_jet_sv1_deltaR_u = 0;
    std::vector<float> *m_jet_sv1_L3d_u = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    std::vector<float> *m_jet_ip2d_pb_u = 0;
    std::vector<float> *m_jet_ip2d_pc_u = 0;
    std::vector<float> *m_jet_ip2d_pu_u = 0; //cuts & c_weight
    std::vector<float> *m_jet_ip3d_pb_u = 0;
    std::vector<float> *m_jet_ip3d_pc_u = 0;
    std::vector<float> *m_jet_ip3d_pu_u = 0;

    Int_t m_eventnb_b;
    Float_t m_mcwg_b;
    Float_t m_Fcal_b;
    Int_t m_njets_b;
    std::vector<float> *m_jet_pt_b = 0;         //MV2
    std::vector<float> *m_jet_eta_b = 0;        //MV2
    std::vector<int> *m_jet_LabDr_HadF_b = 0;   //label
    std::vector<float> *m_jet_nConst_b = 0;     //cuts not used (nConst > 1)
    std::vector<int> *m_jet_truthMatch_b = 0;   //cuts (truthMatch == 1)
    std::vector<int> *m_jet_aliveAfterOR_b = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    std::vector<float> *m_jet_jf_m_b = 0;
    std::vector<float> *m_jet_jf_efc_b = 0;
    std::vector<float> *m_jet_jf_deta_b = 0; // for jet_jf_dR
    std::vector<float> *m_jet_jf_dphi_b = 0; // for jet_jf_dR
    std::vector<float> *m_jet_jf_nvtx_b = 0;
    std::vector<float> *m_jet_jf_sig3d_b = 0;
    std::vector<float> *m_jet_jf_nvtx1t_b = 0;
    std::vector<float> *m_jet_jf_n2t_b = 0;
    std::vector<float> *m_jet_jf_ntrkAtVx_b = 0;

    // ** SV1 Variable (8) ** //
    std::vector<float> *m_jet_sv1_ntrkv_b = 0;
    std::vector<float> *m_jet_sv1_n2t_b = 0;
    std::vector<float> *m_jet_sv1_m_b = 0;
    std::vector<float> *m_jet_sv1_efc_b = 0;
    std::vector<float> *m_jet_sv1_sig3d_b = 0;
    std::vector<float> *m_jet_sv1_Lxy_b = 0;
    std::vector<float> *m_jet_sv1_deltaR_b = 0;
    std::vector<float> *m_jet_sv1_L3d_b = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    std::vector<float> *m_jet_ip2d_pb_b = 0;
    std::vector<float> *m_jet_ip2d_pc_b = 0;
    std::vector<float> *m_jet_ip2d_pu_b = 0; //cuts & c_weight
    std::vector<float> *m_jet_ip3d_pb_b = 0;
    std::vector<float> *m_jet_ip3d_pc_b = 0;
    std::vector<float> *m_jet_ip3d_pu_b = 0;

    Int_t m_eventnb_c;
    Float_t m_mcwg_c;
    Float_t m_Fcal_c;
    Int_t m_njets_c;
    std::vector<float> *m_jet_pt_c = 0;         //MV2
    std::vector<float> *m_jet_eta_c = 0;        //MV2
    std::vector<int> *m_jet_LabDr_HadF_c = 0;   //label
    std::vector<float> *m_jet_nConst_c = 0;     //cuts not used (nConst > 1)
    std::vector<int> *m_jet_truthMatch_c = 0;   //cuts (truthMatch == 1)
    std::vector<int> *m_jet_aliveAfterOR_c = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    std::vector<float> *m_jet_jf_m_c = 0;
    std::vector<float> *m_jet_jf_efc_c = 0;
    std::vector<float> *m_jet_jf_deta_c = 0; // for jet_jf_dR
    std::vector<float> *m_jet_jf_dphi_c = 0; // for jet_jf_dR
    std::vector<float> *m_jet_jf_nvtx_c = 0;
    std::vector<float> *m_jet_jf_sig3d_c = 0;
    std::vector<float> *m_jet_jf_nvtx1t_c = 0;
    std::vector<float> *m_jet_jf_n2t_c = 0;
    std::vector<float> *m_jet_jf_ntrkAtVx_c = 0;

    // ** SV1 Variable (8) ** //
    std::vector<float> *m_jet_sv1_ntrkv_c = 0;
    std::vector<float> *m_jet_sv1_n2t_c = 0;
    std::vector<float> *m_jet_sv1_m_c = 0;
    std::vector<float> *m_jet_sv1_efc_c = 0;
    std::vector<float> *m_jet_sv1_sig3d_c = 0;
    std::vector<float> *m_jet_sv1_Lxy_c = 0;
    std::vector<float> *m_jet_sv1_deltaR_c = 0;
    std::vector<float> *m_jet_sv1_L3d_c = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    std::vector<float> *m_jet_ip2d_pb_c = 0;
    std::vector<float> *m_jet_ip2d_pc_c = 0;
    std::vector<float> *m_jet_ip2d_pu_c = 0; //cuts & c_weight
    std::vector<float> *m_jet_ip3d_pb_c = 0;
    std::vector<float> *m_jet_ip3d_pc_c = 0;
    std::vector<float> *m_jet_ip3d_pu_c = 0;

    f_new_u->Branch("eventnb", &m_eventnb_u);
    f_new_u->Branch("mcwg", &m_mcwg_u);
    f_new_u->Branch("njets", &m_njets_u);
    f_new_u->Branch("Fcal", &m_Fcal_u);

    f_new_u->Branch("jet_pt", &m_jet_pt_u);
    //cout << m_jet_pt << endl;
    f_new_u->Branch("jet_eta", &m_jet_eta_u);
    f_new_u->Branch("jet_LabDr_HadF", &m_jet_LabDr_HadF_u);
    f_new_u->Branch("jet_aliveAfterOR", &m_jet_aliveAfterOR) ub;
    f_new_u->Branch("jet_nConst", &m_jet_nConst_u);
    f_new_u->Branch("jet_truthMatch", &m_jet_truthMatch_u);
    //f_new_u->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_u->Branch("jet_sv1_ntrkv", &m_jet_sv1_ntrkv_u);
    f_new_u->Branch("jet_sv1_n2t", &m_jet_sv1_n2t_u);
    f_new_u->Branch("jet_sv1_m", &m_jet_sv1_m_u);
    f_new_u->Branch("jet_sv1_efc", &m_jet_sv1_efc_u);
    //f_new_u->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_u->Branch("jet_sv1_Lxy", &m_jet_sv1_Lxy_u);
    f_new_u->Branch("jet_sv1_L3d", &m_jet_sv1_L3d_u);
    f_new_u->Branch("jet_sv1_deltaR", &m_jet_sv1_deltaR_u);
    f_new_u->Branch("jet_sv1_sig3d", &m_jet_sv1_sig3d_u);

    f_new_u->Branch("jet_jf_m", &m_jet_jf_m_u);
    f_new_u->Branch("jet_jf_efc", &m_jet_jf_efc_u);
    f_new_u->Branch("jet_jf_deta", &m_jet_jf_deta_u);
    f_new_u->Branch("jet_jf_dphi", &m_jet_jf_dphi_u);
    f_new_u->Branch("jet_jf_ntrkAtVx", &m_jet_jf_ntrkAtVx_u);
    f_new_u->Branch("jet_jf_nvtx", &m_jet_jf_nvtx_u);
    f_new_u->Branch("jet_jf_sig3d", &m_jet_jf_sig3d_u);
    f_new_u->Branch("jet_jf_nvtx1t", &m_jet_jf_nvtx1t_u);
    f_new_u->Branch("jet_jf_n2t", &m_jet_jf_n2t_u);

    f_new_u->Branch("jet_ip2d_pb", &m_jet_ip2d_pb_u);
    f_new_u->Branch("jet_ip2d_pc", &m_jet_ip2d_pc_u);
    f_new_u->Branch("jet_ip2d_pu", &m_jet_ip2d_pu_u);
    f_new_u->Branch("jet_ip3d_pb", &m_jet_ip3d_pb_u);
    f_new_u->Branch("jet_ip3d_pc", &m_jet_ip3d_pc_u);
    f_new_u->Branch("jet_ip3d_pu", &m_jet_ip3d_pu_u);

    //b
    f_new_b->Branch("eventnb", &m_eventnb_b);
    f_new_b->Branch("mcwg", &m_mcwg_b);
    f_new_b->Branch("njets", &m_njets_b);
    f_new_b->Branch("Fcal", &m_Fcal_b);

    f_new_b->Branch("jet_pt", &m_jet_pt_b);
    //cout << m_jet_pt << endl;
    f_new_b->Branch("jet_eta", &m_jet_eta_b);
    f_new_b->Branch("jet_LabDr_HadF", &m_jet_LabDr_HadF_b);
    f_new_b->Branch("jet_aliveAfterOR", &m_jet_aliveAfterOR) _b;
    f_new_b->Branch("jet_nConst", &m_jet_nConst_b);
    f_new_b->Branch("jet_truthMatch", &m_jet_truthMatch_b);
    //f_new_b->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_b->Branch("jet_sv1_ntrkv", &m_jet_sv1_ntrkv_b);
    f_new_b->Branch("jet_sv1_n2t", &m_jet_sv1_n2t_b);
    f_new_b->Branch("jet_sv1_m", &m_jet_sv1_m_b);
    f_new_b->Branch("jet_sv1_efc", &m_jet_sv1_efc_b);
    //f_new_b->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_b->Branch("jet_sv1_Lxy", &m_jet_sv1_Lxy_b);
    f_new_b->Branch("jet_sv1_L3d", &m_jet_sv1_L3d_b);
    f_new_b->Branch("jet_sv1_deltaR", &m_jet_sv1_deltaR_b);
    f_new_b->Branch("jet_sv1_sig3d", &m_jet_sv1_sig3d_b);

    f_new_b->Branch("jet_jf_m", &m_jet_jf_m_b);
    f_new_b->Branch("jet_jf_efc", &m_jet_jf_efc_b);
    f_new_b->Branch("jet_jf_deta", &m_jet_jf_deta_b);
    f_new_b->Branch("jet_jf_dphi", &m_jet_jf_dphi_b);
    f_new_b->Branch("jet_jf_ntrkAtVx", &m_jet_jf_ntrkAtVx_b);
    f_new_b->Branch("jet_jf_nvtx", &m_jet_jf_nvtx_b);
    f_new_b->Branch("jet_jf_sig3d", &m_jet_jf_sig3d_b);
    f_new_b->Branch("jet_jf_nvtx1t", &m_jet_jf_nvtx1t_b);
    f_new_b->Branch("jet_jf_n2t", &m_jet_jf_n2t_b);

    f_new_b->Branch("jet_ip2d_pb", &m_jet_ip2d_pb_b);
    f_new_b->Branch("jet_ip2d_pc", &m_jet_ip2d_pc_b);
    f_new_b->Branch("jet_ip2d_pu", &m_jet_ip2d_pu_b);
    f_new_b->Branch("jet_ip3d_pb", &m_jet_ip3d_pb_b);
    f_new_b->Branch("jet_ip3d_pc", &m_jet_ip3d_pc_b);
    f_new_b->Branch("jet_ip3d_pu", &m_jet_ip3d_pu_b);

    //c
    f_new_c->Branch("eventnb", &m_eventnb_c);
    f_new_c->Branch("mcwg", &m_mcwg_c);
    f_new_c->Branch("njets", &m_njets_c);
    f_new_c->Branch("Fcal", &m_Fcal_c);

    f_new_c->Branch("jet_pt", &m_jet_pt_c);
    //cout << m_jet_pt << endl;
    f_new_c->Branch("jet_eta", &m_jet_eta_c);
    f_new_c->Branch("jet_LabDr_HadF", &m_jet_LabDr_HadF_c);
    f_new_c->Branch("jet_aliveAfterOR", &m_jet_aliveAfterOR) cb;
    f_new_c->Branch("jet_nConst", &m_jet_nConst_c);
    f_new_c->Branch("jet_truthMatch", &m_jet_truthMatch_c);
    //f_new_c->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_c->Branch("jet_sv1_ntrkv", &m_jet_sv1_ntrkv_c);
    f_new_c->Branch("jet_sv1_n2t", &m_jet_sv1_n2t_c);
    f_new_c->Branch("jet_sv1_m", &m_jet_sv1_m_c);
    f_new_c->Branch("jet_sv1_efc", &m_jet_sv1_efc_c);
    //f_new_c->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_c->Branch("jet_sv1_Lxy", &m_jet_sv1_Lxy_c);
    f_new_c->Branch("jet_sv1_L3d", &m_jet_sv1_L3d_c);
    f_new_c->Branch("jet_sv1_deltaR", &m_jet_sv1_deltaR_c);
    f_new_c->Branch("jet_sv1_sig3d", &m_jet_sv1_sig3d_c);

    f_new_c->Branch("jet_jf_m", &m_jet_jf_m_c);
    f_new_c->Branch("jet_jf_efc", &m_jet_jf_efc_c);
    f_new_c->Branch("jet_jf_deta", &m_jet_jf_deta_c);
    f_new_c->Branch("jet_jf_dphi", &m_jet_jf_dphi_c);
    f_new_c->Branch("jet_jf_ntrkAtVx", &m_jet_jf_ntrkAtVx_c);
    f_new_c->Branch("jet_jf_nvtx", &m_jet_jf_nvtx_c);
    f_new_c->Branch("jet_jf_sig3d", &m_jet_jf_sig3d_c);
    f_new_c->Branch("jet_jf_nvtx1t", &m_jet_jf_nvtx1t_c);
    f_new_c->Branch("jet_jf_n2t", &m_jet_jf_n2t_c);

    f_new_c->Branch("jet_ip2d_pb", &m_jet_ip2d_pb_c);
    f_new_c->Branch("jet_ip2d_pc", &m_jet_ip2d_pc_c);
    f_new_c->Branch("jet_ip2d_pu", &m_jet_ip2d_pu_c);
    f_new_c->Branch("jet_ip3d_pb", &m_jet_ip3d_pb_c);
    f_new_c->Branch("jet_ip3d_pc", &m_jet_ip3d_pc_c);
    f_new_c->Branch("jet_ip3d_pu", &m_jet_ip3d_pu_c);

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
        int njets_u = 0;
        int njets_b = 0;
        int njets_c = 0;

        for (int i = 0; i < njets; i++)
        {
            if (b >= stat_small_b && light >= stat_small && c >= cStat_small)
                break;

            //cout << jet_pt->at(i) << endl;
            //cout << jet_eta->at(i) << endl;
            //cout << jet_truthMatch->at(i) << endl;
            //cout << jet_aliveAfterOR->at(i) << endl;
            //cout << !(jet_pt->at(i) >= ptLim * 1e3 && fabs(jet_eta->at(i)) <= 2.1 && jet_truthMatch->at(i) == 1 && jet_aliveAfterOR->at(i) == 1) << endl;
            if (cut && !(jet_pt->at(i) >= ptLim * 1e3 && fabs(jet_eta->at(i)) <= 2.1 && jet_truthMatch->at(i) == 1 && jet_aliveAfterOR->at(i) == 1))
                continue;
            //cout << (jet_LabDr_HadF->at(i) != 0 || jet_LabDr_HadF->at(i) != 4 || jet_LabDr_HadF->at(i) != 5) << endl;
            //cout << jet_LabDr_HadF->at(i) << endl;
            if (cut && jet_LabDr_HadF->at(i) != 0 && jet_LabDr_HadF->at(i) != 4 && jet_LabDr_HadF->at(i) != 5)
                continue;
            if (jet_LabDr_HadF->at(i) == 5)
            {
                if (b >= stat_small_b)
                    continue;
                b++;
                njets_b++;
                m_jet_pt_b->push_back(jet_pt->at(i));
                m_jet_eta_b->push_back(jet_eta->at(i));
                m_jet_LabDr_HadF_b->push_back(jet_LabDr_HadF->at(i));
                m_jet_nConst_b->push_back(jet_nConst->at(i));
                m_jet_truthMatch_b->push_back(jet_truthMatch->at(i));
                m_jet_aliveAfterOR_b->push_back(jet_aliveAfterOR->at(i));

                // ** JetFitter Variables (8) ** //
                m_jet_jf_m_b->push_back(jet_jf_m->at(i));
                m_jet_jf_efc_b->push_back(jet_jf_efc->at(i));
                m_jet_jf_deta_b->push_back(jet_jf_deta->at(i));
                m_jet_jf_dphi_b->push_back(jet_jf_dphi->at(i));
                m_jet_jf_nvtx_b->push_back(jet_jf_nvtx->at(i));
                m_jet_jf_sig3d_b->push_back(jet_jf_sig3d->at(i));
                m_jet_jf_nvtx1t_b->push_back(jet_jf_nvtx1t->at(i));
                m_jet_jf_n2t_b->push_back(jet_jf_n2t->at(i));
                m_jet_jf_ntrkAtVx_b->push_back(jet_jf_ntrkAtVx->at(i));

                // ** SV1 Variable (8) ** //
                m_jet_sv1_ntrkv_b->push_back(jet_sv1_ntrkv->at(i));
                m_jet_sv1_n2t_b->push_back(jet_sv1_n2t->at(i));
                m_jet_sv1_m_b->push_back(jet_sv1_m->at(i));
                m_jet_sv1_efc_b->push_back(jet_sv1_efc->at(i));
                m_jet_sv1_sig3d_b->push_back(jet_sv1_sig3d->at(i));
                m_jet_sv1_Lxy_b->push_back(jet_sv1_Lxy->at(i));
                m_jet_sv1_deltaR_b->push_back(jet_sv1_deltaR->at(i));
                m_jet_sv1_L3d_b->push_back(jet_sv1_L3d->at(i));

                //other MV2 variables ip_x,ip_x_c,ip_x_cu
                m_jet_ip2d_pb_b->push_back(jet_ip2d_pb->at(i));
                m_jet_ip2d_pc_b->push_back(jet_ip2d_pc->at(i));
                m_jet_ip2d_pu_b->push_back(jet_ip2d_pu->at(i));
                m_jet_ip3d_pb_b->push_back(jet_ip3d_pb->at(i));
                m_jet_ip3d_pc_b->push_back(jet_ip3d_pc->at(i));
                m_jet_ip3d_pu_b->push_back(jet_ip3d_pu->at(i));
            }
            if (jet_LabDr_HadF->at(i) == 4)
            {
                if (c >= cStat_small)
                    continue;
                c++;
                njets_c++;
                m_jet_pt_c->push_back(jet_pt->at(i));
                m_jet_eta_c->push_back(jet_eta->at(i));
                m_jet_LabDr_HadF_c->push_back(jet_LabDr_HadF->at(i));
                m_jet_nConst_c->push_back(jet_nConst->at(i));
                m_jet_truthMatch_c->push_back(jet_truthMatch->at(i));
                m_jet_aliveAfterOR_c->push_back(jet_aliveAfterOR->at(i));

                // ** JetFitter Variables (8) ** //
                m_jet_jf_m_c->push_back(jet_jf_m->at(i));
                m_jet_jf_efc_c->push_back(jet_jf_efc->at(i));
                m_jet_jf_deta_c->push_back(jet_jf_deta->at(i));
                m_jet_jf_dphi_c->push_back(jet_jf_dphi->at(i));
                m_jet_jf_nvtx_c->push_back(jet_jf_nvtx->at(i));
                m_jet_jf_sig3d_c->push_back(jet_jf_sig3d->at(i));
                m_jet_jf_nvtx1t_c->push_back(jet_jf_nvtx1t->at(i));
                m_jet_jf_n2t_c->push_back(jet_jf_n2t->at(i));
                m_jet_jf_ntrkAtVx_c->push_back(jet_jf_ntrkAtVx->at(i));

                // ** SV1 Variable (8) ** //
                m_jet_sv1_ntrkv_c->push_back(jet_sv1_ntrkv->at(i));
                m_jet_sv1_n2t_c->push_back(jet_sv1_n2t->at(i));
                m_jet_sv1_m_c->push_back(jet_sv1_m->at(i));
                m_jet_sv1_efc_c->push_back(jet_sv1_efc->at(i));
                m_jet_sv1_sig3d_c->push_back(jet_sv1_sig3d->at(i));
                m_jet_sv1_Lxy_c->push_back(jet_sv1_Lxy->at(i));
                m_jet_sv1_deltaR_c->push_back(jet_sv1_deltaR->at(i));
                m_jet_sv1_L3d_c->push_back(jet_sv1_L3d->at(i));

                //other MV2 variables ip_x,ip_x_c,ip_x_cu
                m_jet_ip2d_pb_c->push_back(jet_ip2d_pb->at(i));
                m_jet_ip2d_pc_c->push_back(jet_ip2d_pc->at(i));
                m_jet_ip2d_pu_c->push_back(jet_ip2d_pu->at(i));
                m_jet_ip3d_pb_c->push_back(jet_ip3d_pb->at(i));
                m_jet_ip3d_pc_c->push_back(jet_ip3d_pc->at(i));
                m_jet_ip3d_pu_c->push_back(jet_ip3d_pu->at(i));
            }
            if (jet_LabDr_HadF->at(i))
            {
                if (light >= stat_small)
                    continue;
                light++;
                njets_u++;
                m_jet_pt_u->push_back(jet_pt->at(i));
                m_jet_eta_u->push_back(jet_eta->at(i));
                m_jet_LabDr_HadF_u->push_back(jet_LabDr_HadF->at(i));
                m_jet_nConst_u->push_back(jet_nConst->at(i));
                m_jet_truthMatch_u->push_back(jet_truthMatch->at(i));
                m_jet_aliveAfterOR_u->push_back(jet_aliveAfterOR->at(i));

                // ** JetFitter Variables (8) ** //
                m_jet_jf_m_u->push_back(jet_jf_m->at(i));
                m_jet_jf_efc_u->push_back(jet_jf_efc->at(i));
                m_jet_jf_deta_u->push_back(jet_jf_deta->at(i));
                m_jet_jf_dphi_u->push_back(jet_jf_dphi->at(i));
                m_jet_jf_nvtx_u->push_back(jet_jf_nvtx->at(i));
                m_jet_jf_sig3d_u->push_back(jet_jf_sig3d->at(i));
                m_jet_jf_nvtx1t_u->push_back(jet_jf_nvtx1t->at(i));
                m_jet_jf_n2t_u->push_back(jet_jf_n2t->at(i));
                m_jet_jf_ntrkAtVx_u->push_back(jet_jf_ntrkAtVx->at(i));

                // ** SV1 Variable (8) ** //
                m_jet_sv1_ntrkv_u->push_back(jet_sv1_ntrkv->at(i));
                m_jet_sv1_n2t_u->push_back(jet_sv1_n2t->at(i));
                m_jet_sv1_m_u->push_back(jet_sv1_m->at(i));
                m_jet_sv1_efc_u->push_back(jet_sv1_efc->at(i));
                m_jet_sv1_sig3d_u->push_back(jet_sv1_sig3d->at(i));
                m_jet_sv1_Lxy_u->push_back(jet_sv1_Lxy->at(i));
                m_jet_sv1_deltaR_u->push_back(jet_sv1_deltaR->at(i));
                m_jet_sv1_L3d_u->push_back(jet_sv1_L3d->at(i));

                //other MV2 variables ip_x,ip_x_c,ip_x_cu
                m_jet_ip2d_pb_u->push_back(jet_ip2d_pb->at(i));
                m_jet_ip2d_pc_u->push_back(jet_ip2d_pc->at(i));
                m_jet_ip2d_pu_u->push_back(jet_ip2d_pu->at(i));
                m_jet_ip3d_pb_u->push_back(jet_ip3d_pb->at(i));
                m_jet_ip3d_pc_u->push_back(jet_ip3d_pc->at(i));
                m_jet_ip3d_pu_u->push_back(jet_ip3d_pu->at(i));
            }
            //cout << jet_pt->at(i) << endl;
            cout << "b: " << b << "c: " << c << "u: " << light << endl;
        }
        if (m_jet_pt_b->size() >= 0)
        {

            m_njets_b = njets_b;
            m_eventnb_b = eventnb;
            m_Fcal_b = Fcal;
            //cout << Fcal << endl;
            m_mcwg_b = 1;
            if (!inclusive)
            {
                //b_mcwg->GetEntry(jentry);
                //cout << mcwg << endl;
                m_mcwg_b = mcwg;
            }
            f_new_b->Fill();
        }
        if (m_jet_pt_c->size() >= 0)
        {
            m_njets_c = njets_c;
            m_eventnb_c = eventnb;
            m_Fcal_c = Fcal;
            //cout << Fcal << endl;
            m_mcwg_c = 1;
            if (!inclusive)
            {
                //b_mcwg->GetEntry(jentry);
                //cout << mcwg << endl;
                m_mcwg_c = mcwg;
            }
            f_new_c->Fill();
        }
        if (m_jet_pt_u->size() >= 0)
        {
            m_njets_u = njets_u;
            m_eventnb_u = eventnb;
            m_Fcal_u = Fcal;
            //cout << Fcal << endl;
            m_mcwg_u = 1;
            if (!inclusive)
            {
                //b_mcwg->GetEntry(jentry);
                //cout << mcwg << endl;
                m_mcwg_u = mcwg;
            }
            f_new_u->Fill();
        }

        m_jet_pt_b->clear();
        m_jet_eta_b->clear();
        m_jet_LabDr_HadF_b->clear();
        m_jet_nConst_b->clear();
        m_jet_truthMatch_b->clear();
        m_jet_aliveAfterOR_b->clear();

        // ** JetFitter Variables (8) ** //
        m_jet_jf_m_b->clear();
        m_jet_jf_efc_b->clear();
        m_jet_jf_deta_b->clear();
        m_jet_jf_dphi_b->clear();
        m_jet_jf_nvtx_b->clear();
        m_jet_jf_sig3d_b->clear();
        m_jet_jf_nvtx1t_b->clear();
        m_jet_jf_n2t_b->clear();
        m_jet_jf_ntrkAtVx_b->clear();

        // ** SV1 Variable (8) ** //
        m_jet_sv1_ntrkv_b->clear();
        m_jet_sv1_n2t_b->clear();
        m_jet_sv1_m_b->clear();
        m_jet_sv1_efc_b->clear();
        m_jet_sv1_sig3d_b->clear();
        m_jet_sv1_Lxy_b->clear();
        m_jet_sv1_deltaR_b->clear();
        m_jet_sv1_L3d_b->clear();

        //other MV2 variables ip_x,ip_x_c,ip_x_cu
        m_jet_ip2d_pb_b->clear();
        m_jet_ip2d_pc_b->clear();
        m_jet_ip2d_pu_b->clear();
        m_jet_ip3d_pb_b->clear();
        m_jet_ip3d_pc_b->clear();
        m_jet_ip3d_pu_b->clear();

        m_jet_pt_b->clear();
        m_jet_eta_b->clear();
        m_jet_LabDr_HadF_b->clear();
        m_jet_nConst_b->clear();
        m_jet_truthMatch_b->clear();
        m_jet_aliveAfterOR_b->clear();

        // ** JetFitter Variables (8) ** //
        m_jet_jf_m_b->clear();
        m_jet_jf_efc_b->clear();
        m_jet_jf_deta_b->clear();
        m_jet_jf_dphi_b->clear();
        m_jet_jf_nvtx_b->clear();
        m_jet_jf_sig3d_b->clear();
        m_jet_jf_nvtx1t_b->clear();
        m_jet_jf_n2t_b->clear();
        m_jet_jf_ntrkAtVx_b->clear();

        // ** SV1 Variable (8) ** //
        m_jet_sv1_ntrkv_b->clear();
        m_jet_sv1_n2t_b->clear();
        m_jet_sv1_m_b->clear();
        m_jet_sv1_efc_b->clear();
        m_jet_sv1_sig3d_b->clear();
        m_jet_sv1_Lxy_b->clear();
        m_jet_sv1_deltaR_b->clear();
        m_jet_sv1_L3d_b->clear();

        //other MV2 variables ip_x,ip_x_c,ip_x_cu
        m_jet_ip2d_pb_b->clear();
        m_jet_ip2d_pc_b->clear();
        m_jet_ip2d_pu_b->clear();
        m_jet_ip3d_pb_b->clear();
        m_jet_ip3d_pc_b->clear();
        m_jet_ip3d_pu_b->clear();

        m_jet_pt_c->clear();
        m_jet_eta_c->clear();
        m_jet_LabDr_HadF_c->clear();
        m_jet_nConst_c->clear();
        m_jet_truthMatch_c->clear();
        m_jet_aliveAfterOR_c->clear();

        // ** JetFitter Variables (8) ** //
        m_jet_jf_m_c->clear();
        m_jet_jf_efc_c->clear();
        m_jet_jf_deta_c->clear();
        m_jet_jf_dphi_c->clear();
        m_jet_jf_nvtx_c->clear();
        m_jet_jf_sig3d_c->clear();
        m_jet_jf_nvtx1t_c->clear();
        m_jet_jf_n2t_c->clear();
        m_jet_jf_ntrkAtVx_c->clear();

        // ** SV1 Variable (8) ** //
        m_jet_sv1_ntrkv_c->clear();
        m_jet_sv1_n2t_c->clear();
        m_jet_sv1_m_c->clear();
        m_jet_sv1_efc_c->clear();
        m_jet_sv1_sig3d_c->clear();
        m_jet_sv1_Lxy_c->clear();
        m_jet_sv1_deltaR_c->clear();
        m_jet_sv1_L3d_c->clear();

        //other MV2 variables ip_x,ip_x_c,ip_x_cu
        m_jet_ip2d_pb_c->clear();
        m_jet_ip2d_pc_c->clear();
        m_jet_ip2d_pu_c->clear();
        m_jet_ip3d_pb_c->clear();
        m_jet_ip3d_pc_c->clear();
        m_jet_ip3d_pu_c->clear();
    }
    f_new_b->Write();
    f_new_c->Write();
    f_new_u->Write();
    fout->Close();
}
