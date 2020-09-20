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
    cout << cut << endl;
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
    std::string Centrality;
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
        cout << "[ERROR]: training parsing failed" << endl;
        return;
    }
    if (cut)
    {
        bool parsed = parse_filename(filename, JZ, tag, NUM, inclusive, PbPb, pnfs, dataType);
        if (!parsed)
        {
            cout << "[ERROR]: filename parsing failed" << endl;
            return;
        }
        std::string line2;
    }

    else
    {
        int start = filename.rfind("/") + 1;
        //cout << filename.substr(start, 1) << endl;
        JZ = std::stoi(filename.substr(start, 1));
        //cout << filename.substr(start + 2, 1) << endl;
        tag = std::stoi(filename.substr(start + 2, 1));
        PbPb = (filename.substr(start - 8, 1) == "b");
        int cet_length = PbPb ? 2 : 0;
        //cout << filename.substr(start + 4, filename.length() - start - cet_length - 4 - 11) << endl;
        NUM = std::stoi(filename.substr(start + 4, filename.length() - start - cet_length - 4 - 11));
        int shift = PbPb ? 4 : 2;
        dataType = filename.substr(filename.rfind("cher97/") + 7, filename.find("_small/") - (filename.rfind("cher97/") + 7) - shift);
        //cout << dataType << endl;
        //cout << filename.substr(filename.length() - 12, 1) << endl;
        central = PbPb ? std::stoi(filename.substr(filename.length() - 12, 1)) : 0;
        inclusive = (filename.find("Working") != std::string::npos);
    }
    Centrality = PbPb ? "_" + std::to_string(central) : "";
    double file_weight = 1./get_weight(JZ, tag, NUM, inclusive, PbPb, pnfs, dataType);
    if (file_weight == -1)
    {
        cout << "[ERROR]: file weight is -1" << endl;
        return;
    }
    //return;
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

    int b = 0;
    int light = 0;
    int c = 0;

    TFile *fout = new TFile(Form("%s/%s_%d_%d_%d_smalls%s.root", outputFolder, trainname.c_str(), JZ, tag, NUM, Centrality.c_str()), "RECREATE");
    TTree *f_new_utr = new TTree(Form("%s_utr", chain_name), Form("%s_utr", chain_name));
    TTree *f_new_btr = new TTree(Form("%s_btr", chain_name), Form("%s_btr", chain_name));
    TTree *f_new_ctr = new TTree(Form("%s_ctr", chain_name), Form("%s_ctr", chain_name));
    TTree *f_new_ute = new TTree(Form("%s_ute", chain_name), Form("%s_ute", chain_name));
    TTree *f_new_bte = new TTree(Form("%s_bte", chain_name), Form("%s_bte", chain_name));
    TTree *f_new_cte = new TTree(Form("%s_cte", chain_name), Form("%s_cte", chain_name));

    Int_t m_eventnb;
    Double_t m_mcwg;
    Double_t m_Fcal;
    //Int_t m_njets_u;
    Double_t m_jet_pt = 0;        //MV2
    Double_t m_jet_eta = 0;       //MV2
    Int_t m_jet_LabDr_HadF = 0;   //label
    Int_t m_jet_nConst = 0;       //cuts not used (nConst > 1)
    Int_t m_jet_truthMatch = 0;   //cuts (truthMatch == 1)
    Int_t m_jet_aliveAfterOR = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    Double_t m_jet_jf_m = 0;
    Double_t m_jet_jf_efc = 0;
    //Double_t m_jet_jf_deta_u = 0; // for jet_jf_dR
    //Double_t m_jet_jf_dphi_u = 0; // for jet_jf_dR
    Double_t m_jet_jf_dR = 0;
    Double_t m_jet_jf_nvtx = 0;
    Double_t m_jet_jf_sig3d = 0;
    Int_t m_jet_jf_nvtx1t = 0;
    Int_t m_jet_jf_n2t = 0;
    Int_t m_jet_jf_ntrkAtVx = 0;

    // ** SV1 Variable (8) ** //
    Int_t m_jet_sv1_ntrkv = 0;
    Int_t m_jet_sv1_n2t = 0;
    Double_t m_jet_sv1_m = 0;
    Double_t m_jet_sv1_efc = 0;
    Double_t m_jet_sv1_sig3d = 0;
    Double_t m_jet_sv1_Lxy = 0;
    Double_t m_jet_sv1_deltaR = 0;
    Double_t m_jet_sv1_L3d = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    Float_t m_jet_ip2d_pb = 0;
    Float_t m_jet_ip2d_pc = 0;
    Float_t m_jet_ip2d_pu = 0; //cuts & c_weight
    Float_t m_jet_ip3d_pb = 0;
    Float_t m_jet_ip3d_pc = 0;
    Float_t m_jet_ip3d_pu = 0;
    //old
    /*Int_t m_eventnb_u;
    Double_t m_mcwg_u;
    Double_t m_Fcal_u;
    //Int_t m_njets_u;
    Double_t m_jet_pt_u = 0;        //MV2
    Double_t m_jet_eta_u = 0;       //MV2
    Int_t m_jet_LabDr_HadF_u = 0;   //label
    Int_t m_jet_nConst_u = 0;       //cuts not used (nConst > 1)
    Int_t m_jet_truthMatch_u = 0;   //cuts (truthMatch == 1)
    Int_t m_jet_aliveAfterOR_u = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    Double_t m_jet_jf_m_u = 0;
    Double_t m_jet_jf_efc_u = 0;
    //Double_t m_jet_jf_deta_u = 0; // for jet_jf_dR
    //Double_t m_jet_jf_dphi_u = 0; // for jet_jf_dR
    Double_t m_jet_jf_dR_u = 0;
    Double_t m_jet_jf_nvtx_u = 0;
    Double_t m_jet_jf_sig3d_u = 0;
    Int_t m_jet_jf_nvtx1t_u = 0;
    Int_t m_jet_jf_n2t_u = 0;
    Int_t m_jet_jf_ntrkAtVx_u = 0;

    // ** SV1 Variable (8) ** //
    Int_t m_jet_sv1_ntrkv_u = 0;
    Int_t m_jet_sv1_n2t_u = 0;
    Double_t m_jet_sv1_m_u = 0;
    Double_t m_jet_sv1_efc_u = 0;
    Double_t m_jet_sv1_sig3d_u = 0;
    Double_t m_jet_sv1_Lxy_u = 0;
    Double_t m_jet_sv1_deltaR_u = 0;
    Double_t m_jet_sv1_L3d_u = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    Float_t m_jet_ip2d_pb_u = 0;
    Float_t m_jet_ip2d_pc_u = 0;
    Float_t m_jet_ip2d_pu_u = 0; //cuts & c_weight
    Float_t m_jet_ip3d_pb_u = 0;
    Float_t m_jet_ip3d_pc_u = 0;
    Float_t m_jet_ip3d_pu_u = 0;
    //b
    Int_t m_eventnb_b;
    Double_t m_mcwg_b;
    Double_t m_Fcal_b;
    //Int_t m_njets_u;
    Double_t m_jet_pt_b = 0;        //MV2
    Double_t m_jet_eta_b = 0;       //MV2
    Int_t m_jet_LabDr_HadF_b = 0;   //label
    Int_t m_jet_nConst_b = 0;       //cuts not used (nConst > 1)
    Int_t m_jet_truthMatch_b = 0;   //cuts (truthMatch == 1)
    Int_t m_jet_aliveAfterOR_b = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    Double_t m_jet_jf_m_b = 0;
    Double_t m_jet_jf_efc_b = 0;
    //Double_t m_jet_jf_deta_b = 0; // for jet_jf_dR
    //Double_t m_jet_jf_dphi_b = 0; // for jet_jf_dR
    Double_t m_jet_jf_dR_b = 0;
    Double_t m_jet_jf_nvtx_b = 0;
    Double_t m_jet_jf_sig3d_b = 0;
    Int_t m_jet_jf_nvtx1t_b = 0;
    Int_t m_jet_jf_n2t_b = 0;
    Int_t m_jet_jf_ntrkAtVx_b = 0;

    // ** SV1 Variable (8) ** //
    Int_t m_jet_sv1_ntrkv_b = 0;
    Int_t m_jet_sv1_n2t_b = 0;
    Double_t m_jet_sv1_m_b = 0;
    Double_t m_jet_sv1_efc_b = 0;
    Double_t m_jet_sv1_sig3d_b = 0;
    Double_t m_jet_sv1_Lxy_b = 0;
    Double_t m_jet_sv1_deltaR_b = 0;
    Double_t m_jet_sv1_L3d_b = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    Float_t m_jet_ip2d_pb_b = 0;
    Float_t m_jet_ip2d_pc_b = 0;
    Float_t m_jet_ip2d_pu_b = 0; //cuts & c_weight
    Float_t m_jet_ip3d_pb_b = 0;
    Float_t m_jet_ip3d_pc_b = 0;
    Float_t m_jet_ip3d_pu_b = 0;
    //c
    Int_t m_eventnb_c;
    Double_t m_mcwg_c;
    Double_t m_Fcal_c;
    //Int_t m_njets_u;
    Double_t m_jet_pt_c = 0;        //MV2
    Double_t m_jet_eta_c = 0;       //MV2
    Int_t m_jet_LabDr_HadF_c = 0;   //label
    Int_t m_jet_nConst_c = 0;       //cuts not used (nConst > 1)
    Int_t m_jet_truthMatch_c = 0;   //cuts (truthMatch == 1)
    Int_t m_jet_aliveAfterOR_c = 0; //cuts (aliveAfterOR == 1) (no overlap with electron)
    //std::vector<float>   *jet_ip2d_pu; //cuts & c_weight & MV2

    // ** JetFitter Variables (8) ** //
    Double_t m_jet_jf_m_c = 0;
    Double_t m_jet_jf_efc_c = 0;
    //Double_t m_jet_jf_deta_c = 0; // for jet_jf_dR
    //Double_t m_jet_jf_dphi_c = 0; // for jet_jf_dR
    Double_t m_jet_jf_dR_c = 0;
    Double_t m_jet_jf_nvtx_c = 0;
    Double_t m_jet_jf_sig3d_c = 0;
    Int_t m_jet_jf_nvtx1t_c = 0;
    Int_t m_jet_jf_n2t_c = 0;
    Int_t m_jet_jf_ntrkAtVx_c = 0;

    // ** SV1 Variable (8) ** //
    Int_t m_jet_sv1_ntrkv_c = 0;
    Int_t m_jet_sv1_n2t_c = 0;
    Double_t m_jet_sv1_m_c = 0;
    Double_t m_jet_sv1_efc_c = 0;
    Double_t m_jet_sv1_sig3d_c = 0;
    Double_t m_jet_sv1_Lxy_c = 0;
    Double_t m_jet_sv1_deltaR_c = 0;
    Double_t m_jet_sv1_L3d_c = 0;

    //other MV2 variables ip_x,ip_x_c,ip_x_cu
    Float_t m_jet_ip2d_pb_c = 0;
    Float_t m_jet_ip2d_pc_c = 0;
    Float_t m_jet_ip2d_pu_c = 0; //cuts & c_weight
    Float_t m_jet_ip3d_pb_c = 0;
    Float_t m_jet_ip3d_pc_c = 0;
    Float_t m_jet_ip3d_pu_c = 0;*/

    /*f_new_u->Branch("eventnb", &m_eventnb_u);
    f_new_u->Branch("mcwg", &m_mcwg_u);
    //f_new_u->Branch("njets", &m_njets_u);
    f_new_u->Branch("Fcal", &m_Fcal_u);

    f_new_u->Branch("pt", &m_jet_pt_u);
    //cout << m_jet_pt << endl;
    f_new_u->Branch("eta", &m_jet_eta_u);
    f_new_u->Branch("label", &m_jet_LabDr_HadF_u);
    f_new_u->Branch("aliveAfterOR", &m_jet_aliveAfterOR_u);
    f_new_u->Branch("nConst", &m_jet_nConst_u);
    f_new_u->Branch("truthMatch", &m_jet_truthMatch_u);
    //f_new_u->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_u->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv_u);
    f_new_u->Branch("sv1_n2t", &m_jet_sv1_n2t_u);
    f_new_u->Branch("sv1_m", &m_jet_sv1_m_u);
    f_new_u->Branch("sv1_efc", &m_jet_sv1_efc_u);
    //f_new_u->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_u->Branch("sv1_Lxy", &m_jet_sv1_Lxy_u);
    f_new_u->Branch("sv1_L3d", &m_jet_sv1_L3d_u);
    f_new_u->Branch("sv1_deltaR", &m_jet_sv1_deltaR_u);
    f_new_u->Branch("sv1_sig3d", &m_jet_sv1_sig3d_u);

    f_new_u->Branch("jf_mass", &m_jet_jf_m_u);
    f_new_u->Branch("jf_efrc", &m_jet_jf_efc_u);
    //f_new_u->Branch("jet_jf_deta", &m_jet_jf_deta_u);
    //f_new_u->Branch("jet_jf_dphi", &m_jet_jf_dphi_u);
    f_new_u->Branch("jf_dR", &m_jet_jf_dR_u);
    f_new_u->Branch("jf_ntrkv", &m_jet_jf_ntrkAtVx_u);
    f_new_u->Branch("jf_nvtx", &m_jet_jf_nvtx_u);
    f_new_u->Branch("jf_sig", &m_jet_jf_sig3d_u);
    f_new_u->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t_u);
    f_new_u->Branch("jf_n2tv", &m_jet_jf_n2t_u);

    f_new_u->Branch("ip2d_pb", &m_jet_ip2d_pb_u);
    f_new_u->Branch("ip2d_pc", &m_jet_ip2d_pc_u);
    f_new_u->Branch("ip2d_pu", &m_jet_ip2d_pu_u);
    f_new_u->Branch("ip3d_pb", &m_jet_ip3d_pb_u);
    f_new_u->Branch("ip3d_pc", &m_jet_ip3d_pc_u);
    f_new_u->Branch("ip3d_pu", &m_jet_ip3d_pu_u);

    //b
    f_new_b->Branch("eventnb", &m_eventnb_b);
    f_new_b->Branch("mcwg", &m_mcwg_b);
    //f_new_b->Branch("njets", &m_njets_b);
    f_new_b->Branch("Fcal", &m_Fcal_b);

    f_new_b->Branch("pt", &m_jet_pt_b);
    //cout << m_jet_pt << endl;
    f_new_b->Branch("eta", &m_jet_eta_b);
    f_new_b->Branch("LabDr_HadF", &m_jet_LabDr_HadF_b);
    f_new_b->Branch("aliveAfterOR", &m_jet_aliveAfterOR_b);
    f_new_b->Branch("nConst", &m_jet_nConst_b);
    f_new_b->Branch("truthMatch", &m_jet_truthMatch_b);
    //f_new_b->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_b->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv_b);
    f_new_b->Branch("sv1_n2t", &m_jet_sv1_n2t_b);
    f_new_b->Branch("sv1_m", &m_jet_sv1_m_b);
    f_new_b->Branch("sv1_efc", &m_jet_sv1_efc_b);
    //f_new_b->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_b->Branch("sv1_Lxy", &m_jet_sv1_Lxy_b);
    f_new_b->Branch("sv1_L3d", &m_jet_sv1_L3d_b);
    f_new_b->Branch("sv1_deltaR", &m_jet_sv1_deltaR_b);
    f_new_b->Branch("sv1_sig3d", &m_jet_sv1_sig3d_b);

    f_new_b->Branch("jf_m", &m_jet_jf_m_b);
    f_new_b->Branch("jf_efc", &m_jet_jf_efc_b);
    //f_new_b->Branch("jet_jf_deta", &m_jet_jf_deta_b);
    //f_new_b->Branch("jet_jf_dphi", &m_jet_jf_dphi_b);
    f_new_b->Branch("jf_dR", &m_jet_jf_dR_b);
    f_new_b->Branch("jf_ntrkAtVx", &m_jet_jf_ntrkAtVx_b);
    f_new_b->Branch("jf_nvtx", &m_jet_jf_nvtx_b);
    f_new_b->Branch("jf_sig3d", &m_jet_jf_sig3d_b);
    f_new_b->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t_b);
    f_new_b->Branch("jf_n2t", &m_jet_jf_n2t_b);

    f_new_b->Branch("ip2d_pb", &m_jet_ip2d_pb_b);
    f_new_b->Branch("ip2d_pc", &m_jet_ip2d_pc_b);
    f_new_b->Branch("ip2d_pu", &m_jet_ip2d_pu_b);
    f_new_b->Branch("ip3d_pb", &m_jet_ip3d_pb_b);
    f_new_b->Branch("ip3d_pc", &m_jet_ip3d_pc_b);
    f_new_b->Branch("ip3d_pu", &m_jet_ip3d_pu_b);

    //c
    f_new_c->Branch("eventnb", &m_eventnb_c);
    f_new_c->Branch("mcwg", &m_mcwg_c);
    //f_new_c->Branch("njets", &m_njets_c);
    f_new_c->Branch("Fcal", &m_Fcal_c);

    f_new_c->Branch("pt", &m_jet_pt_c);
    //cout << m_jet_pt << endl;
    f_new_c->Branch("eta", &m_jet_eta_c);
    f_new_c->Branch("LabDr_HadF", &m_jet_LabDr_HadF_c);
    f_new_c->Branch("aliveAfterOR", &m_jet_aliveAfterOR_c);
    f_new_c->Branch("nConst", &m_jet_nConst_c);
    f_new_c->Branch("truthMatch", &m_jet_truthMatch_c);
    //f_new_c->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_c->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv_c);
    f_new_c->Branch("sv1_n2t", &m_jet_sv1_n2t_c);
    f_new_c->Branch("sv1_m", &m_jet_sv1_m_c);
    f_new_c->Branch("sv1_efc", &m_jet_sv1_efc_c);
    //f_new_c->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_c->Branch("sv1_Lxy", &m_jet_sv1_Lxy_c);
    f_new_c->Branch("sv1_L3d", &m_jet_sv1_L3d_c);
    f_new_c->Branch("sv1_deltaR", &m_jet_sv1_deltaR_c);
    f_new_c->Branch("sv1_sig3d", &m_jet_sv1_sig3d_c);

    f_new_c->Branch("jf_m", &m_jet_jf_m_c);
    f_new_c->Branch("jf_efc", &m_jet_jf_efc_c);
    //f_new_c->Branch("jet_jf_deta", &m_jet_jf_deta_c);
    //f_new_c->Branch("jet_jf_dphi", &m_jet_jf_dphi_c);
    f_new_c->Branch("jf_dR", &m_jet_jf_dR_c);
    f_new_c->Branch("jf_ntrkAtVx", &m_jet_jf_ntrkAtVx_c);
    f_new_c->Branch("jf_nvtx", &m_jet_jf_nvtx_c);
    f_new_c->Branch("jf_sig3d", &m_jet_jf_sig3d_c);
    f_new_c->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t_c);
    f_new_c->Branch("jf_n2t", &m_jet_jf_n2t_c);

    f_new_c->Branch("ip2d_pb", &m_jet_ip2d_pb_c);
    f_new_c->Branch("ip2d_pc", &m_jet_ip2d_pc_c);
    f_new_c->Branch("ip2d_pu", &m_jet_ip2d_pu_c);
    f_new_c->Branch("ip3d_pb", &m_jet_ip3d_pb_c);
    f_new_c->Branch("ip3d_pc", &m_jet_ip3d_pc_c);
    f_new_c->Branch("ip3d_pu", &m_jet_ip3d_pu_c);*/

    f_new_utr->Branch("eventnb", &m_eventnb);
    f_new_utr->Branch("mcwg", &m_mcwg);
    //f_new_u->Branch("njets", &m_njets_u);
    f_new_utr->Branch("Fcal", &m_Fcal);

    f_new_utr->Branch("pt", &m_jet_pt);
    //cout << m_jet_pt << endl;
    f_new_utr->Branch("eta", &m_jet_eta);
    f_new_utr->Branch("label", &m_jet_LabDr_HadF);
    f_new_utr->Branch("aliveAfterOR", &m_jet_aliveAfterOR);
    f_new_utr->Branch("nConst", &m_jet_nConst);
    f_new_utr->Branch("truthMatch", &m_jet_truthMatch);
    //f_new_u->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_utr->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv);
    f_new_utr->Branch("sv1_n2t", &m_jet_sv1_n2t);
    f_new_utr->Branch("sv1_m", &m_jet_sv1_m);
    f_new_utr->Branch("sv1_efc", &m_jet_sv1_efc);
    //f_new_u->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_utr->Branch("sv1_Lxy", &m_jet_sv1_Lxy);
    f_new_utr->Branch("sv1_L3d", &m_jet_sv1_L3d);
    f_new_utr->Branch("sv1_deltaR", &m_jet_sv1_deltaR);
    f_new_utr->Branch("sv1_sig3d", &m_jet_sv1_sig3d);

    f_new_utr->Branch("jf_mass", &m_jet_jf_m);
    f_new_utr->Branch("jf_efrc", &m_jet_jf_efc);
    //f_new_u->Branch("jet_jf_deta", &m_jet_jf_deta_u);
    //f_new_u->Branch("jet_jf_dphi", &m_jet_jf_dphi_u);
    f_new_utr->Branch("jf_dR", &m_jet_jf_dR);
    f_new_utr->Branch("jf_ntrkv", &m_jet_jf_ntrkAtVx);
    f_new_utr->Branch("jf_nvtx", &m_jet_jf_nvtx);
    f_new_utr->Branch("jf_sig", &m_jet_jf_sig3d);
    f_new_utr->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t);
    f_new_utr->Branch("jf_n2tv", &m_jet_jf_n2t);

    f_new_utr->Branch("ip2d_pb", &m_jet_ip2d_pb);
    f_new_utr->Branch("ip2d_pc", &m_jet_ip2d_pc);
    f_new_utr->Branch("ip2d_pu", &m_jet_ip2d_pu);
    f_new_utr->Branch("ip3d_pb", &m_jet_ip3d_pb);
    f_new_utr->Branch("ip3d_pc", &m_jet_ip3d_pc);
    f_new_utr->Branch("ip3d_pu", &m_jet_ip3d_pu);

    //b
    f_new_bte->Branch("eventnb", &m_eventnb);
    f_new_bte->Branch("mcwg", &m_mcwg);
    //f_new_bte->Branch("njets", &m_njets_b);
    f_new_bte->Branch("Fcal", &m_Fcal);

    f_new_bte->Branch("pt", &m_jet_pt);
    //cout << m_jet_pt << endl;
    f_new_bte->Branch("eta", &m_jet_eta);
    f_new_bte->Branch("LabDr_HadF", &m_jet_LabDr_HadF);
    f_new_bte->Branch("aliveAfterOR", &m_jet_aliveAfterOR);
    f_new_bte->Branch("nConst", &m_jet_nConst);
    f_new_bte->Branch("truthMatch", &m_jet_truthMatch);
    //f_new_bte->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_bte->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv);
    f_new_bte->Branch("sv1_n2t", &m_jet_sv1_n2t);
    f_new_bte->Branch("sv1_m", &m_jet_sv1_m);
    f_new_bte->Branch("sv1_efc", &m_jet_sv1_efc);
    //f_new_bte->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_bte->Branch("sv1_Lxy", &m_jet_sv1_Lxy);
    f_new_bte->Branch("sv1_L3d", &m_jet_sv1_L3d);
    f_new_bte->Branch("sv1_deltaR", &m_jet_sv1_deltaR);
    f_new_bte->Branch("sv1_sig3d", &m_jet_sv1_sig3d);

    f_new_bte->Branch("jf_m", &m_jet_jf_m);
    f_new_bte->Branch("jf_efc", &m_jet_jf_efc);
    //f_new_bte->Branch("jet_jf_deta", &m_jet_jf_deta_b);
    //f_new_bte->Branch("jet_jf_dphi", &m_jet_jf_dphi_b);
    f_new_bte->Branch("jf_dR", &m_jet_jf_dR);
    f_new_bte->Branch("jf_ntrkAtVx", &m_jet_jf_ntrkAtVx);
    f_new_bte->Branch("jf_nvtx", &m_jet_jf_nvtx);
    f_new_bte->Branch("jf_sig3d", &m_jet_jf_sig3d);
    f_new_bte->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t);
    f_new_bte->Branch("jf_n2t", &m_jet_jf_n2t);

    f_new_bte->Branch("ip2d_pb", &m_jet_ip2d_pb);
    f_new_bte->Branch("ip2d_pc", &m_jet_ip2d_pc);
    f_new_bte->Branch("ip2d_pu", &m_jet_ip2d_pu);
    f_new_bte->Branch("ip3d_pb", &m_jet_ip3d_pb);
    f_new_bte->Branch("ip3d_pc", &m_jet_ip3d_pc);
    f_new_bte->Branch("ip3d_pu", &m_jet_ip3d_pu);

    //c
    f_new_ctr->Branch("eventnb", &m_eventnb);
    f_new_ctr->Branch("mcwg", &m_mcwg);
    //f_new_ctr->Branch("njets", &m_njets_c);
    f_new_ctr->Branch("Fcal", &m_Fcal);

    f_new_ctr->Branch("pt", &m_jet_pt);
    //cout << m_jet_pt << endl;
    f_new_ctr->Branch("eta", &m_jet_eta);
    f_new_ctr->Branch("LabDr_HadF", &m_jet_LabDr_HadF);
    f_new_ctr->Branch("aliveAfterOR", &m_jet_aliveAfterOR);
    f_new_ctr->Branch("nConst", &m_jet_nConst);
    f_new_ctr->Branch("truthMatch", &m_jet_truthMatch);
    //f_new_ctr->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_ctr->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv);
    f_new_ctr->Branch("sv1_n2t", &m_jet_sv1_n2t);
    f_new_ctr->Branch("sv1_m", &m_jet_sv1_m);
    f_new_ctr->Branch("sv1_efc", &m_jet_sv1_efc);
    //f_new_ctr->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_ctr->Branch("sv1_Lxy", &m_jet_sv1_Lxy);
    f_new_ctr->Branch("sv1_L3d", &m_jet_sv1_L3d);
    f_new_ctr->Branch("sv1_deltaR", &m_jet_sv1_deltaR);
    f_new_ctr->Branch("sv1_sig3d", &m_jet_sv1_sig3d);

    f_new_ctr->Branch("jf_m", &m_jet_jf_m);
    f_new_ctr->Branch("jf_efc", &m_jet_jf_efc);
    //f_new_ctr->Branch("jet_jf_deta", &m_jet_jf_deta_c);
    //f_new_ctr->Branch("jet_jf_dphi", &m_jet_jf_dphi_c);
    f_new_ctr->Branch("jf_dR", &m_jet_jf_dR);
    f_new_ctr->Branch("jf_ntrkAtVx", &m_jet_jf_ntrkAtVx);
    f_new_ctr->Branch("jf_nvtx", &m_jet_jf_nvtx);
    f_new_ctr->Branch("jf_sig3d", &m_jet_jf_sig3d);
    f_new_ctr->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t);
    f_new_ctr->Branch("jf_n2t", &m_jet_jf_n2t);

    f_new_ctr->Branch("ip2d_pb", &m_jet_ip2d_pb);
    f_new_ctr->Branch("ip2d_pc", &m_jet_ip2d_pc);
    f_new_ctr->Branch("ip2d_pu", &m_jet_ip2d_pu);
    f_new_ctr->Branch("ip3d_pb", &m_jet_ip3d_pb);
    f_new_ctr->Branch("ip3d_pc", &m_jet_ip3d_pc);
    f_new_ctr->Branch("ip3d_pu", &m_jet_ip3d_pu);


//testing trees
    f_new_ute->Branch("eventnb", &m_eventnb);
    f_new_ute->Branch("mcwg", &m_mcwg);
    //f_new_u->Branch("njets", &m_njets_u);
    f_new_ute->Branch("Fcal", &m_Fcal);

    f_new_ute->Branch("pt", &m_jet_pt);
    //cout << m_jet_pt << endl;
    f_new_ute->Branch("eta", &m_jet_eta);
    f_new_ute->Branch("label", &m_jet_LabDr_HadF);
    f_new_ute->Branch("aliveAfterOR", &m_jet_aliveAfterOR);
    f_new_ute->Branch("nConst", &m_jet_nConst);
    f_new_ute->Branch("truthMatch", &m_jet_truthMatch);
    //f_new_u->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_ute->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv);
    f_new_ute->Branch("sv1_n2t", &m_jet_sv1_n2t);
    f_new_ute->Branch("sv1_m", &m_jet_sv1_m);
    f_new_ute->Branch("sv1_efc", &m_jet_sv1_efc);
    //f_new_u->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_ute->Branch("sv1_Lxy", &m_jet_sv1_Lxy);
    f_new_ute->Branch("sv1_L3d", &m_jet_sv1_L3d);
    f_new_ute->Branch("sv1_deltaR", &m_jet_sv1_deltaR);
    f_new_ute->Branch("sv1_sig3d", &m_jet_sv1_sig3d);

    f_new_ute->Branch("jf_mass", &m_jet_jf_m);
    f_new_ute->Branch("jf_efrc", &m_jet_jf_efc);
    //f_new_u->Branch("jet_jf_deta", &m_jet_jf_deta_u);
    //f_new_u->Branch("jet_jf_dphi", &m_jet_jf_dphi_u);
    f_new_ute->Branch("jf_dR", &m_jet_jf_dR);
    f_new_ute->Branch("jf_ntrkv", &m_jet_jf_ntrkAtVx);
    f_new_ute->Branch("jf_nvtx", &m_jet_jf_nvtx);
    f_new_ute->Branch("jf_sig", &m_jet_jf_sig3d);
    f_new_ute->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t);
    f_new_ute->Branch("jf_n2tv", &m_jet_jf_n2t);

    f_new_ute->Branch("ip2d_pb", &m_jet_ip2d_pb);
    f_new_ute->Branch("ip2d_pc", &m_jet_ip2d_pc);
    f_new_ute->Branch("ip2d_pu", &m_jet_ip2d_pu);
    f_new_ute->Branch("ip3d_pb", &m_jet_ip3d_pb);
    f_new_ute->Branch("ip3d_pc", &m_jet_ip3d_pc);
    f_new_ute->Branch("ip3d_pu", &m_jet_ip3d_pu);

    //b
    f_new_btr->Branch("eventnb", &m_eventnb);
    f_new_btr->Branch("mcwg", &m_mcwg);
    //f_new_btr->Branch("njets", &m_njets_b);
    f_new_btr->Branch("Fcal", &m_Fcal);

    f_new_btr->Branch("pt", &m_jet_pt);
    //cout << m_jet_pt << endl;
    f_new_btr->Branch("eta", &m_jet_eta);
    f_new_btr->Branch("LabDr_HadF", &m_jet_LabDr_HadF);
    f_new_btr->Branch("aliveAfterOR", &m_jet_aliveAfterOR);
    f_new_btr->Branch("nConst", &m_jet_nConst);
    f_new_btr->Branch("truthMatch", &m_jet_truthMatch);
    //f_new_btr->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_btr->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv);
    f_new_btr->Branch("sv1_n2t", &m_jet_sv1_n2t);
    f_new_btr->Branch("sv1_m", &m_jet_sv1_m);
    f_new_btr->Branch("sv1_efc", &m_jet_sv1_efc);
    //f_new_btr->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_btr->Branch("sv1_Lxy", &m_jet_sv1_Lxy);
    f_new_btr->Branch("sv1_L3d", &m_jet_sv1_L3d);
    f_new_btr->Branch("sv1_deltaR", &m_jet_sv1_deltaR);
    f_new_btr->Branch("sv1_sig3d", &m_jet_sv1_sig3d);

    f_new_btr->Branch("jf_m", &m_jet_jf_m);
    f_new_btr->Branch("jf_efc", &m_jet_jf_efc);
    //f_new_btr->Branch("jet_jf_deta", &m_jet_jf_deta_b);
    //f_new_btr->Branch("jet_jf_dphi", &m_jet_jf_dphi_b);
    f_new_btr->Branch("jf_dR", &m_jet_jf_dR);
    f_new_btr->Branch("jf_ntrkAtVx", &m_jet_jf_ntrkAtVx);
    f_new_btr->Branch("jf_nvtx", &m_jet_jf_nvtx);
    f_new_btr->Branch("jf_sig3d", &m_jet_jf_sig3d);
    f_new_btr->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t);
    f_new_btr->Branch("jf_n2t", &m_jet_jf_n2t);

    f_new_btr->Branch("ip2d_pb", &m_jet_ip2d_pb);
    f_new_btr->Branch("ip2d_pc", &m_jet_ip2d_pc);
    f_new_btr->Branch("ip2d_pu", &m_jet_ip2d_pu);
    f_new_btr->Branch("ip3d_pb", &m_jet_ip3d_pb);
    f_new_btr->Branch("ip3d_pc", &m_jet_ip3d_pc);
    f_new_btr->Branch("ip3d_pu", &m_jet_ip3d_pu);

    //c
    f_new_cte->Branch("eventnb", &m_eventnb);
    f_new_cte->Branch("mcwg", &m_mcwg);
    //f_new_cte->Branch("njets", &m_njets_c);
    f_new_cte->Branch("Fcal", &m_Fcal);

    f_new_cte->Branch("pt", &m_jet_pt);
    //cout << m_jet_pt << endl;
    f_new_cte->Branch("eta", &m_jet_eta);
    f_new_cte->Branch("LabDr_HadF", &m_jet_LabDr_HadF);
    f_new_cte->Branch("aliveAfterOR", &m_jet_aliveAfterOR);
    f_new_cte->Branch("nConst", &m_jet_nConst);
    f_new_cte->Branch("truthMatch", &m_jet_truthMatch);
    //f_new_cte->Branch("jet_ip2d_llr", &m_jet_ip2d_llr);
    f_new_cte->Branch("sv1_ntrkv", &m_jet_sv1_ntrkv);
    f_new_cte->Branch("sv1_n2t", &m_jet_sv1_n2t);
    f_new_cte->Branch("sv1_m", &m_jet_sv1_m);
    f_new_cte->Branch("sv1_efc", &m_jet_sv1_efc);
    //f_new_cte->Branch("jet_sv1_normdist", &m_jet_sv1_normdist);
    f_new_cte->Branch("sv1_Lxy", &m_jet_sv1_Lxy);
    f_new_cte->Branch("sv1_L3d", &m_jet_sv1_L3d);
    f_new_cte->Branch("sv1_deltaR", &m_jet_sv1_deltaR);
    f_new_cte->Branch("sv1_sig3d", &m_jet_sv1_sig3d);

    f_new_cte->Branch("jf_m", &m_jet_jf_m);
    f_new_cte->Branch("jf_efc", &m_jet_jf_efc);
    //f_new_cte->Branch("jet_jf_deta", &m_jet_jf_deta_c);
    //f_new_cte->Branch("jet_jf_dphi", &m_jet_jf_dphi_c);
    f_new_cte->Branch("jf_dR", &m_jet_jf_dR);
    f_new_cte->Branch("jf_ntrkAtVx", &m_jet_jf_ntrkAtVx);
    f_new_cte->Branch("jf_nvtx", &m_jet_jf_nvtx);
    f_new_cte->Branch("jf_sig3d", &m_jet_jf_sig3d);
    f_new_cte->Branch("jf_nvtx1t", &m_jet_jf_nvtx1t);
    f_new_cte->Branch("jf_n2t", &m_jet_jf_n2t);

    f_new_cte->Branch("ip2d_pb", &m_jet_ip2d_pb);
    f_new_cte->Branch("ip2d_pc", &m_jet_ip2d_pc);
    f_new_cte->Branch("ip2d_pu", &m_jet_ip2d_pu);
    f_new_cte->Branch("ip3d_pb", &m_jet_ip3d_pb);
    f_new_cte->Branch("ip3d_pc", &m_jet_ip3d_pc);
    f_new_cte->Branch("ip3d_pu", &m_jet_ip3d_pu);

    if (stat_small == 0 && stat_small_b == 0 && cStat_small == 0)
    {
        cout << "[WARNING]: No jets pass in this file, pass file" << endl;
        f_new_ute->Write();
        f_new_bte->Write();
        f_new_cte->Write();
        f_new_utr->Write();
        f_new_btr->Write();
        f_new_ctr->Write();
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
        //cout << njets << endl;
        int njets_u = 0;
        int njets_b = 0;
        int njets_c = 0;

        for (int i = 0; i < jet_pt->size(); i++)
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
                //njets_u++;
                m_eventnb = eventnb;
                m_Fcal = (double)Fcal;
                //cout << Fcal << endl;
                m_mcwg = file_weight;
                if (!inclusive)
                {
                    //b_mcwg->GetEntry(jentry);
                    //cout << mcwg << endl;
                    m_mcwg = (double)mcwg * file_weight;
                }
                //njets_b++;
                m_jet_pt = (double)(jet_pt->at(i));
                m_jet_eta = (double)(jet_eta->at(i));
                m_jet_LabDr_HadF = (int)(jet_LabDr_HadF->at(i));
                m_jet_nConst = (int)(jet_nConst->at(i));
                m_jet_truthMatch = (int)(jet_truthMatch->at(i));
                m_jet_aliveAfterOR = (int)(jet_aliveAfterOR->at(i));

                // ** JetFitter Variables (8) ** //
                m_jet_jf_m = (double)(jet_jf_m->at(i));
                m_jet_jf_efc = (double)(jet_jf_efc->at(i));
                m_jet_jf_dR = jet_jf_m->at(i) > 0 ? hypot(jet_jf_deta->at(i), jet_jf_dphi->at(i)) : -10;
                //m_jet_jf_deta_b = (double)(jet_jf_deta->at(i));
                //m_jet_jf_dphi_b = (double)(jet_jf_dphi->at(i));
                m_jet_jf_nvtx = (int)(jet_jf_nvtx->at(i));
                m_jet_jf_sig3d = (double)(jet_jf_sig3d->at(i));
                m_jet_jf_nvtx1t = (int)(jet_jf_nvtx1t->at(i));
                m_jet_jf_n2t = (int)(jet_jf_n2t->at(i));
                m_jet_jf_ntrkAtVx = (int)(jet_jf_ntrkAtVx->at(i));

                // ** SV1 Variable (8) ** //
                m_jet_sv1_ntrkv = (int)(jet_sv1_ntrkv->at(i));
                m_jet_sv1_n2t = (int)(jet_sv1_n2t->at(i));
                m_jet_sv1_m = (double)(jet_sv1_m->at(i));
                m_jet_sv1_efc = (double)(jet_sv1_efc->at(i));
                m_jet_sv1_sig3d = (double)(jet_sv1_sig3d->at(i));
                m_jet_sv1_Lxy = (double)(jet_sv1_Lxy->at(i));
                m_jet_sv1_deltaR = (double)(jet_sv1_deltaR->at(i));
                m_jet_sv1_L3d = (double)(jet_sv1_L3d->at(i));

                //other MV2 variables ip_x,ip_x_c,ip_x_cu
                m_jet_ip2d_pb = (jet_ip2d_pb->at(i));
                m_jet_ip2d_pc = (jet_ip2d_pc->at(i));
                m_jet_ip2d_pu = (jet_ip2d_pu->at(i));
                m_jet_ip3d_pb = (jet_ip3d_pb->at(i));
                m_jet_ip3d_pc = (jet_ip3d_pc->at(i));
                m_jet_ip3d_pu = (jet_ip3d_pu->at(i));

                if (b % 2 == 0)
                    f_new_btr->Fill();
                else
                    f_new_bte->Fill();
            }
            if (jet_LabDr_HadF->at(i) == 4)
            {
                if (c >= cStat_small)
                    continue;
                c++;
                //njets_u++;
                m_eventnb = eventnb;
                m_Fcal = (double)Fcal;
                //cout << Fcal << endl;
                m_mcwg = file_weight;
                if (!inclusive)
                {
                    //b_mcwg->GetEntry(jentry);
                    //cout << mcwg << endl;
                    m_mcwg = (double)mcwg * file_weight;
                }
                //njets_c++;
                m_jet_pt = (double)(jet_pt->at(i));
                m_jet_eta = (double)(jet_eta->at(i));
                m_jet_LabDr_HadF = (int)(jet_LabDr_HadF->at(i));
                m_jet_nConst = (int)(jet_nConst->at(i));
                m_jet_truthMatch = (int)(jet_truthMatch->at(i));
                m_jet_aliveAfterOR = (int)(jet_aliveAfterOR->at(i));

                // ** JetFitter Variables (8) ** //
                m_jet_jf_m = (double)(jet_jf_m->at(i));
                m_jet_jf_efc = (double)(jet_jf_efc->at(i));
                m_jet_jf_dR = jet_jf_m->at(i) > 0 ? hypot(jet_jf_deta->at(i), jet_jf_dphi->at(i)) : -10;
                m_jet_jf_nvtx = (int)(jet_jf_nvtx->at(i));
                m_jet_jf_sig3d = (double)(jet_jf_sig3d->at(i));
                m_jet_jf_nvtx1t = (int)(jet_jf_nvtx1t->at(i));
                m_jet_jf_n2t = (int)(jet_jf_n2t->at(i));
                m_jet_jf_ntrkAtVx = (int)(jet_jf_ntrkAtVx->at(i));

                // ** SV1 Variable (8) ** //
                m_jet_sv1_ntrkv = (int)(jet_sv1_ntrkv->at(i));
                m_jet_sv1_n2t = (int)(jet_sv1_n2t->at(i));
                m_jet_sv1_m = (double)(jet_sv1_m->at(i));
                m_jet_sv1_efc = (double)(jet_sv1_efc->at(i));
                m_jet_sv1_sig3d = (double)(jet_sv1_sig3d->at(i));
                m_jet_sv1_Lxy = (double)(jet_sv1_Lxy->at(i));
                m_jet_sv1_deltaR = (double)(jet_sv1_deltaR->at(i));
                m_jet_sv1_L3d = (double)(jet_sv1_L3d->at(i));

                //other MV2 variables ip_x,ip_x_c,ip_x_cu
                m_jet_ip2d_pb = (jet_ip2d_pb->at(i));
                m_jet_ip2d_pc = (jet_ip2d_pc->at(i));
                m_jet_ip2d_pu = (jet_ip2d_pu->at(i));
                m_jet_ip3d_pb = (jet_ip3d_pb->at(i));
                m_jet_ip3d_pc = (jet_ip3d_pc->at(i));
                m_jet_ip3d_pu = (jet_ip3d_pu->at(i));

                if (c % 2 == 0)
                    f_new_ctr->Fill();
                else
                    f_new_cte->Fill();
            }
            if (jet_LabDr_HadF->at(i) == 0)
            {
                if (light >= stat_small)
                    continue;
                light++;
                //njets_u++;
                m_eventnb = eventnb;
                m_Fcal = (double)Fcal;
                //cout << Fcal << endl;
                m_mcwg = file_weight;
                if (!inclusive)
                {
                    //b_mcwg->GetEntry(jentry);
                    //cout << mcwg << endl;
                    m_mcwg = (double)mcwg * file_weight;
                }
                m_jet_pt = (double)(jet_pt->at(i));
                m_jet_eta = (double)(jet_eta->at(i));
                m_jet_LabDr_HadF = (int)(jet_LabDr_HadF->at(i));
                m_jet_nConst = (int)(jet_nConst->at(i));
                m_jet_truthMatch = (int)(jet_truthMatch->at(i));
                m_jet_aliveAfterOR = (int)(jet_aliveAfterOR->at(i));

                // ** JetFitter Variables (8) ** //
                m_jet_jf_m = (double)(jet_jf_m->at(i));
                m_jet_jf_efc = (double)(jet_jf_efc->at(i));
                m_jet_jf_dR = jet_jf_m->at(i) > 0 ? hypot(jet_jf_deta->at(i), jet_jf_dphi->at(i)) : -10;
                m_jet_jf_nvtx = (int)(jet_jf_nvtx->at(i));
                m_jet_jf_sig3d = (double)(jet_jf_sig3d->at(i));
                m_jet_jf_nvtx1t = (int)(jet_jf_nvtx1t->at(i));
                m_jet_jf_n2t = (int)(jet_jf_n2t->at(i));
                m_jet_jf_ntrkAtVx = (int)(jet_jf_ntrkAtVx->at(i));

                // ** SV1 Variable (8) ** //
                m_jet_sv1_ntrkv = (int)(jet_sv1_ntrkv->at(i));
                m_jet_sv1_n2t = (int)(jet_sv1_n2t->at(i));
                m_jet_sv1_m = (double)(jet_sv1_m->at(i));
                m_jet_sv1_efc = (double)(jet_sv1_efc->at(i));
                m_jet_sv1_sig3d = (double)(jet_sv1_sig3d->at(i));
                m_jet_sv1_Lxy = (double)(jet_sv1_Lxy->at(i));
                m_jet_sv1_deltaR = (double)(jet_sv1_deltaR->at(i));
                m_jet_sv1_L3d = (double)(jet_sv1_L3d->at(i));

                //other MV2 variables ip_x,ip_x_c,ip_x_cu
                m_jet_ip2d_pb = (jet_ip2d_pb->at(i));
                m_jet_ip2d_pc = (jet_ip2d_pc->at(i));
                m_jet_ip2d_pu = (jet_ip2d_pu->at(i));
                m_jet_ip3d_pb = (jet_ip3d_pb->at(i));
                m_jet_ip3d_pc = (jet_ip3d_pc->at(i));
                m_jet_ip3d_pu = (jet_ip3d_pu->at(i));

                if (light % 2 == 0)
                    f_new_utr->Fill();
                else
                    f_new_ute->Fill();
            }
            //cout << jet_pt->at(i) << endl;
            cout << "b: " << b << "c: " << c << "u: " << light << endl;
            //cout << i << endl;
        }

        /*m_jet_pt_u->clear();
        m_jet_eta_u->clear();
        m_jet_LabDr_HadF_u->clear();
        m_jet_nConst_u->clear();
        m_jet_truthMatch_u->clear();
        m_jet_aliveAfterOR_u->clear();

        // ** JetFitter Variables (8) ** //
        m_jet_jf_m_u->clear();
        m_jet_jf_efc_u->clear();
        m_jet_jf_deta_u->clear();
        m_jet_jf_dphi_u->clear();
        m_jet_jf_nvtx_u->clear();
        m_jet_jf_sig3d_u->clear();
        m_jet_jf_nvtx1t_u->clear();
        m_jet_jf_n2t_u->clear();
        m_jet_jf_ntrkAtVx_u->clear();

        // ** SV1 Variable (8) ** //
        m_jet_sv1_ntrkv_u->clear();
        m_jet_sv1_n2t_u->clear();
        m_jet_sv1_m_u->clear();
        m_jet_sv1_efc_u->clear();
        m_jet_sv1_sig3d_u->clear();
        m_jet_sv1_Lxy_u->clear();
        m_jet_sv1_deltaR_u->clear();
        m_jet_sv1_L3d_u->clear();

        //other MV2 variables ip_x,ip_x_c,ip_x_cu
        m_jet_ip2d_pb_u->clear();
        m_jet_ip2d_pc_u->clear();
        m_jet_ip2d_pu_u->clear();
        m_jet_ip3d_pb_u->clear();
        m_jet_ip3d_pc_u->clear();
        m_jet_ip3d_pu_u->clear();

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
        m_jet_ip3d_pu_c->clear();*/
        //return;
    }
    f_new_bte->Write();
    f_new_cte->Write();
    f_new_ute->Write();
    f_new_btr->Write();
    f_new_ctr->Write();
    f_new_utr->Write();
    fout->Close();
}
