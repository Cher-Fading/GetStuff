#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "Getevtnb.cpp"
#endif

#ifdef __CINT__
gROOT->LoadMacro("Getevtnb.cpp");
#endif

void addCountJets(std::string trainname, std::string dataType, bool PbPb, bool pnfs, bool inclusive)
{
    //std::ifstream filenum(Form("../GetStuff/%s_fileevtnum%s.txt", dataType.c_str(), suffix[pnfs]));
    std::ifstream filenum(Form("../GetStuff/%s_root%s.txt", dataType.c_str(), suffix[pnfs]));
    std::ofstream mfile(Form("../GetStuff/%s_filemissed%s.txt", dataType.c_str(), suffix[pnfs]));
    std::string line;
    std::string line2;

    int counter = 0;
    map<int, int> counts;
    map<int, std::string> types;
    int JZ = -1;
    int tag = -1;
    int NUM = -1;
    /*int k1;
    int k2;
    int k3;*/
    int l1;
    int l2;
    int missed = 0;
    int n_types = 0;

    while (getline(filenum, line))
    {

        /*k1 = line.find("_");
        if (counter==0 && k1!=std::string::npos) done=true;
        if (!done) goto here;
        k2 = line.find("_", k1 + 1);
        k3 = line.find(":");
        JZ = std::stoi(line.substr(0, k1));
        tag = std::stoi(line.substr(k1 + 1, k2 - k1 - 1));
        NUM = std::stoi(line.substr());*/
        if (counter % 100 == 0)
            cout << "Counting file " << counter << "; " << line << endl;

        bool parsed = parse_filename_short(line, dataType, PbPb, pnfs, inclusive, JZ, tag, NUM, true);
        if (!parsed)
        {
            cout << "[ERROR]: parsing failed" << endl;
            return;
        }

        std::ifstream filec(Form("/atlasgpfs01/usatlas/data/cher97/%s%s_Counts%s/%s_%d_%d_%d_counts.txt", dataType.c_str(), Type[PbPb], suffix[pnfs], trainname.c_str(), JZ, tag, NUM));
        if (!filec)
        {
            missed++;
            mfile << Form("/atlasgpfs01/usatlas/data/cher97/%s%s_Counts%s/%s_%d_%d_%d_counts.txt", dataType.c_str(), Type[PbPb], suffix[pnfs], trainname.c_str(), JZ, tag, NUM) << endl;
            continue;
        }
        n_types = 0;
        while (getline(filec, line2))
        {
            l1 = line2.rfind(" ");
            if (l1 == std::string::npos)
            {
                if (n_types == 0) //if the first line is empty
                {
                    cout << "file empty at " << Form("/atlasgpfs01/usatlas/data/cher97/%s%s_Counts%s/%s_%d_%d_%d_counts.txt", dataType.c_str(), Type[PbPb], suffix[pnfs], trainname.c_str(), JZ, tag, NUM) << endl;
                    return;
                }
                continue; //else this migth be the last line
            }

            if (counter == 0)
            {
                types[n_types] = line2.substr(0, l1);
                cout << "Type " << n_types << ": " << types[n_types] << endl;
                counts[n_types] = std::stoi(line2.substr(l1 + 1, line2.length() - l1 - 1));
            }
            else
            {
                counts[n_types] += std::stoi(line2.substr(l1 + 1, line2.length() - l1 - 1));
            }
            n_types++;
        }
        filec.close();
        counter++;
    }
    filenum.close();
    mfile.close();
    cout << "missing " << missed << " files" << endl;
    std::ofstream ftotal(Form("../GetStuff/%s_totaljets%s.txt", dataType.c_str(), suffix[pnfs]));
    for (int i = 0; i < n_types; i++)
    {
        cout << types[i] << " " << counts[i] << endl;
        ftotal << types[i] << " " << counts[i] << endl;
    }
    ftotal.close();
}
