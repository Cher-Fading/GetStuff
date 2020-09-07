#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"

void makeSTree(const char* filename, bool PbPb = true, bool pnfs = true){
   Int_t           runnb;
   //Int_t           eventnb;
   Double_t        mcwg;
   Float_t         Fcal;
   Int_t           njets;
   vector<float>   *jet_pt;
   vector<float>   *jet_eta;
   vector<int>     *jet_LabDr_HadF; //label
   vector<float>   *jet_nConst; //cuts not used (nConst > 1)
   vector<int>     *jet_truthMatch; //cuts (truthMatch == 1)
   vector<int>     *jet_aliveAfterOR; //cuts (aliveAfterOR == 1) (no overlap with electron)
   vector<float>   *jet_ip2d_pu; //cuts & c_weight
   vector<float>   *jet_jf_m;
   vector<float>   *jet_jf_efc;
   vector<float>   *jet_jf_deta;// for jet_jf_dR
   vector<float>   *jet_jf_dphi;// for jet_jf_dR
   vector<float>   *jet_jf_nvtx;
   vector<float>   *jet_jf_sig3d;
   vector<float>   *jet_jf_nvtx1t;
   vector<float>   *jet_jf_n2t;
   vector<float>   *jet_jf_ntrkAtVx;

   vector<float>   *jet_sv1_ntrkv;
   vector<float>   *jet_sv1_n2t;
   vector<float>   *jet_sv1_m;
   vector<float>   *jet_sv1_efc;
   vector<float>   *jet_sv1_sig3d;
   vector<float>   *jet_sv1_Lxy;
   vector<float>   *jet_sv1_deltaR;
   vector<float>   *jet_sv1_L3d;
      


   // List of branches
   TBranch        *b_classID;   //!
   TBranch        *b_className;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_abs_eta_;   //!
   TBranch        *b_ip2;   //!
   TBranch        *b_ip2_c;   //!
   TBranch        *b_ip2_cu;   //!
   TBranch        *b_ip3;   //!
   TBranch        *b_ip3_c;   //!
   TBranch        *b_ip3_cu;   //!
   TBranch        *b_sv1_ntkv;   //!
   TBranch        *b_sv1_mass;   //!
   TBranch        *b_sv1_efrc;   //!
   TBranch        *b_sv1_n2t;   //!
   TBranch        *b_sv1_Lxy;   //!
   TBranch        *b_sv1_L3d;   //!
   TBranch        *b_sv1_sig3;   //!
   TBranch        *b_sv1_dR;   //!
   TBranch        *b_jf_n2tv;   //!
   TBranch        *b_jf_ntrkv;   //!
   TBranch        *b_jf_nvtx;   //!
   TBranch        *b_jf_nvtx1t;   //!
   TBranch        *b_jf_mass;   //!
   TBranch        *b_jf_efrc;   //!
   TBranch        *b_jf_dR;   //!
   TBranch        *b_jf_sig3;   //!
   TBranch        *b_label;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_BDTG;   //!

   fChain->SetBranchAddress("runnb", &runnb, &b_runnb);
   fChain->SetBranchAddress("eventnb", &eventnb, &b_eventnb);
   fChain->SetBranchAddress("mcchan", &mcchan, &b_mcchan);
   fChain->SetBranchAddress("mcwg", &mcwg, &b_mcwg);
   //fChain->SetBranchAddress("nPV", &nPV, &b_nPV);
   fChain->SetBranchAddress("avgmu", &avgmu, &b_avgmu);
   fChain->SetBranchAddress("PVx", &PVx, &b_PVx);
   fChain->SetBranchAddress("PVy", &PVy, &b_PVy);
   fChain->SetBranchAddress("PVz", &PVz, &b_PVz);
   fChain->SetBranchAddress("njets", &njets, &b_njets);
   //fChain->SetBranchAddress("nbjets", &nbjets, &b_nbjets);
   //fChain->SetBranchAddress("nbjets_q", &nbjets_q, &b_nbjets_q);
   //fChain->SetBranchAddress("nbjets_HadI", &nbjets_HadI, &b_nbjets_HadI);
   //fChain->SetBranchAddress("nbjets_HadF", &nbjets_HadF, &b_nbjets_HadF);
   fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet_pt_orig", &jet_pt_orig, &b_jet_pt_orig);   
   fChain->SetBranchAddress("jet_eta_orig", &jet_eta_orig, &b_jet_eta_orig);
   fChain->SetBranchAddress("jet_phi", &jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet_E", &jet_E, &b_jet_E);
   fChain->SetBranchAddress("jet_m", &jet_m, &b_jet_m);
   fChain->SetBranchAddress("jet_LabDr_HadF", &jet_LabDr_HadF, &b_jet_LabDr_HadF);
   //fChain->SetBranchAddress("jet_truthflav", &jet_truthflav, &b_jet_truthflav);
   //fChain->SetBranchAddress("jet_GhostL_q", &jet_GhostL_q, &b_jet_GhostL_q);
   //fChain->SetBranchAddress("jet_GhostL_HadI", &jet_GhostL_HadI, &b_jet_GhostL_HadI);
   //fChain->SetBranchAddress("jet_GhostL_HadF", &jet_GhostL_HadF, &b_jet_GhostL_HadF);
   fChain->SetBranchAddress("jet_aliveAfterOR", &jet_aliveAfterOR, &b_jet_aliveAfterOR);
   fChain->SetBranchAddress("jet_aliveAfterORmu", &jet_aliveAfterORmu, &b_jet_aliveAfterORmu);
   fChain->SetBranchAddress("jet_nConst", &jet_nConst, &b_jet_nConst);
   fChain->SetBranchAddress("jet_truthMatch", &jet_truthMatch, &b_jet_truthMatch);
   fChain->SetBranchAddress("jet_truthPt", &jet_truthPt, &b_jet_truthPt);
   /* fChain->SetBranchAddress("jet_dRiso", &jet_dRiso, &b_jet_dRiso); */
   //fChain->SetBranchAddress("jet_JVF", &jet_JVF, &b_jet_JVF);
   fChain->SetBranchAddress("jet_JVT", &jet_JVT, &b_jet_JVT);
   fChain->SetBranchAddress("jet_ip2d_pb", &jet_ip2d_pb, &b_jet_ip2d_pb);
   fChain->SetBranchAddress("jet_ip2d_pc", &jet_ip2d_pc, &b_jet_ip2d_pc);
   fChain->SetBranchAddress("jet_ip2d_pu", &jet_ip2d_pu, &b_jet_ip2d_pu);
   fChain->SetBranchAddress("jet_ip2d_llr", &jet_ip2d_llr, &b_jet_ip2d_llr);
   fChain->SetBranchAddress("jet_ip3d_pb", &jet_ip3d_pb, &b_jet_ip3d_pb);
   fChain->SetBranchAddress("jet_ip3d_pc", &jet_ip3d_pc, &b_jet_ip3d_pc);
   fChain->SetBranchAddress("jet_ip3d_pu", &jet_ip3d_pu, &b_jet_ip3d_pu);
   fChain->SetBranchAddress("jet_ip3d_llr", &jet_ip3d_llr, &b_jet_ip3d_llr);
   fChain->SetBranchAddress("jet_rnnip_pb", &jet_rnnip_pb, &b_jet_rnnip_pb);
   fChain->SetBranchAddress("jet_rnnip_pc", &jet_rnnip_pc, &b_jet_rnnip_pc);
   fChain->SetBranchAddress("jet_rnnip_pu", &jet_rnnip_pu, &b_jet_rnnip_pu);
   fChain->SetBranchAddress("jet_rnnip_ptau", &jet_rnnip_ptau, &b_jet_rnnip_ptau);
   fChain->SetBranchAddress("jet_sv1_ntrkv", &jet_sv1_ntrkv, &b_jet_sv1_ntrkv);
   fChain->SetBranchAddress("jet_sv1_n2t", &jet_sv1_n2t, &b_jet_sv1_n2t);
   fChain->SetBranchAddress("jet_sv1_m", &jet_sv1_m, &b_jet_sv1_m);
   fChain->SetBranchAddress("jet_sv1_efc", &jet_sv1_efc, &b_jet_sv1_efc);
   fChain->SetBranchAddress("jet_sv1_normdist", &jet_sv1_normdist, &b_jet_sv1_normdist);
   //fChain->SetBranchAddress("jet_sv1_pb", &jet_sv1_pb, &b_jet_sv1_pb);
   //fChain->SetBranchAddress("jet_sv1_pc", &jet_sv1_pc, &b_jet_sv1_pc);
   //fChain->SetBranchAddress("jet_sv1_pu", &jet_sv1_pu, &b_jet_sv1_pu);
   //fChain->SetBranchAddress("jet_sv1_llr", &jet_sv1_llr, &b_jet_sv1_llr);
   fChain->SetBranchAddress("jet_sv1_Lxy",&jet_sv1_Lxy,&b_jet_sv1_Lxy);
   fChain->SetBranchAddress("jet_sv1_L3d",&jet_sv1_L3d,&b_jet_sv1_L3d);
   fChain->SetBranchAddress("jet_sv1_deltaR",&jet_sv1_deltaR,&b_jet_sv1_deltaR);

   fChain->SetBranchAddress("jet_sv1_sig3d", &jet_sv1_sig3d, &b_jet_sv1_sig3d);
   fChain->SetBranchAddress("jet_sv1_vtx_x", &jet_sv1_vtxx, &b_jet_sv1_vtxx);
   fChain->SetBranchAddress("jet_sv1_vtx_y", &jet_sv1_vtxy, &b_jet_sv1_vtxy);
   fChain->SetBranchAddress("jet_sv1_vtx_z", &jet_sv1_vtxz, &b_jet_sv1_vtxz);
   /* fChain->SetBranchAddress("jet_jf_pb", &jet_jf_pb, &b_jet_jf_pb); */
   /* fChain->SetBranchAddress("jet_jf_pc", &jet_jf_pc, &b_jet_jf_pc); */
   /* fChain->SetBranchAddress("jet_jf_pu", &jet_jf_pu, &b_jet_jf_pu); */
   /* fChain->SetBranchAddress("jet_jf_llr", &jet_jf_llr, &b_jet_jf_llr); */
   fChain->SetBranchAddress("jet_jf_m", &jet_jf_m, &b_jet_jf_m);
   fChain->SetBranchAddress("jet_jf_efc", &jet_jf_efc, &b_jet_jf_efc);
   fChain->SetBranchAddress("jet_jf_deta", &jet_jf_deta, &b_jet_jf_deta);
   fChain->SetBranchAddress("jet_jf_dphi", &jet_jf_dphi, &b_jet_jf_dphi);
   fChain->SetBranchAddress("jet_jf_ntrkAtVx", &jet_jf_ntrkAtVx, &b_jet_jf_ntrkAtVx);
   fChain->SetBranchAddress("jet_jf_nvtx", &jet_jf_nvtx, &b_jet_jf_nvtx);
   fChain->SetBranchAddress("jet_jf_sig3d", &jet_jf_sig3d, &b_jet_jf_sig3d);
   fChain->SetBranchAddress("jet_jf_nvtx1t", &jet_jf_nvtx1t, &b_jet_jf_nvtx1t);
   fChain->SetBranchAddress("jet_jf_n2t", &jet_jf_n2t, &b_jet_jf_n2t);
   /* fChain->SetBranchAddress("jet_jf_chi2", &jet_jf_chi2, &b_jet_jf_chi2); */
   /* fChain->SetBranchAddress("jet_jf_ndf", &jet_jf_ndf, &b_jet_jf_ndf); */
   /* fChain->SetBranchAddress("jet_jfcombnn_pb", &jet_jfcombnn_pb, &b_jet_jfcombnn_pb); */
   /* fChain->SetBranchAddress("jet_jfcombnn_pc", &jet_jfcombnn_pc, &b_jet_jfcombnn_pc); */
   /* fChain->SetBranchAddress("jet_jfcombnn_pu", &jet_jfcombnn_pu, &b_jet_jfcombnn_pu); */
   /* fChain->SetBranchAddress("jet_jfcombnn_llr", &jet_jfcombnn_llr, &b_jet_jfcombnn_llr); */
   /* fChain->SetBranchAddress("jet_sv1ip3d", &jet_sv1ip3d, &b_jet_sv1ip3d); */
   /* fChain->SetBranchAddress("jet_mv1", &jet_mv1, &b_jet_mv1); */
   /* fChain->SetBranchAddress("jet_mv1c", &jet_mv1c, &b_jet_mv1c); */
   fChain->SetBranchAddress("jet_mv2c00", &jet_mv2c00, &b_jet_mv2c00);
   fChain->SetBranchAddress("jet_mv2c10", &jet_mv2c10, &b_jet_mv2c10);
   fChain->SetBranchAddress("jet_mv2c20", &jet_mv2c20, &b_jet_mv2c20);
   fChain->SetBranchAddress("jet_mv2c100", &jet_mv2c100, &b_jet_mv2c100);
}