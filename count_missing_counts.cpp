#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "Getevtnb.cpp"
#endif

#ifdef __CINT__
gROOT->LoadMacro("Getevtnb.cpp");
#endif

void count_missing_counts(std::string dataType, std::string trainname, bool PbPb, bool pnfs, bool inclusive)
{
    int gridsize = inclusive ? grid_size : s50k_size;
    int JZ = -1;
    int tag = -1;
    int NUM = -1;
    std::ifstream froot(Form("../GetStuff/%s_root%s.txt", dataType.c_str(), suffix[pnfs]));
    std::ifstream fdone(Form("../GetStuff/%s_countsdone%s.txt", dataType.c_str(), suffix[pnfs]));
    std::ofstream fmiss(Form("../GetStuff/%s_countsmiss%s.txt", dataType.c_str(), suffix[pnfs]));

    int cent_N = PbPb ? cet_N : 1;
    int totf = 0;
    int donef[cent_N];
    int outf[cent_N];
    for (int c = 0; c < cent_N; c++)
    {
        donef[c] = 0;
        outf[c] = 0;
    }

    int done[6][3000][2][cent_N];
    int full[6][3000][2];
    for (int i = 0; i < gridsize; i++)
    {
        for (int j = 0; j < 3000; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                full[i][j][k] = 0;
                for (int c = 0; c < cent_N; c++)
                {
                    done[i][j][k][c] = 0;
                }
            }
        }
    }

    std::string line0;
    while (getline(fdone, line0))
    {
        if (line0.find("_") == std::string::npos)
            continue;
        cout << line0.substr(5, 1) << endl;
        JZ = std::stoi(line0.substr(5, 1));
        cout << line0.substr(7, 1) << endl;
        tag = std::stoi(line0.substr(7, 1));
        int cet_length = PbPb ? 2 : 0;
        cout << line0.substr(9, line0.length() - 20 - cet_length) << endl;
        NUM = std::stoi(line0.substr(9, line0.length() - 20 - cet_length));
        cout << line0.substr(line0.length() - 11, 1) << endl;
        int central = PbPb ? std::stoi(line0.substr(line0.length() - 11, 1)) : 0;
        done[JZ][tag][NUM][central] = 1;
        donef[central]++;
    }

    std::string line;
    while (getline(froot, line))
    {
        bool parsed = parse_filename_short(line, dataType, PbPb, pnfs, inclusive, JZ, tag, NUM, true);
        if (!parsed)
        {
            cout << "[ERROR]: parsing failed" << endl;
            return;
        }
        if (full[JZ][NUM][tag] == 1)
        {
            cout << "name repeated for full at JZ: " << JZ << "; NUM: " << NUM << "; tag: " << tag << endl;
            return;
        }
        full[JZ][NUM][tag] = 1;
        totf++;
        bool missing = false;
        for (int c = 0; c < cent_N; c++)
        {
            if (done[JZ][NUM][tag][c] != 1)
            {
                missing = true;
                //cout << filename << " for centrality: " << c << endl;
                outf[c]++;
            }
        }
        if (missing)
            fmiss << line << endl;
    }

    for (int i = 0; i < gridsize; i++)
    {
        for (int j = 0; j < 3000; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                for (int c = 0; c < cent_N; c++)
                {
                    if (done[i][j][k][c] == 1 && full[i][j][k] != 1)
                    {
                        cout << "JZ: " << i << endl;
                        cout << "ID: " << j << endl;
                        cout << "tag: " << k << endl;
                        std::string C = PbPb ? std::to_string(c) : "pp";
                        cout << "centrality: " << C << endl;
                    }
                }
            }
        }
    }

    cout << totf << endl;
    for (int c = 0; c < cent_N; c++)
    {
        cout << "done: centrality: " << c << ": " << donef[c] << endl;
        cout << "rerun: centrality: " << c << ": " << outf[c] << endl;
    }
    fmiss.close();
    fdone.close();
    froot.close();
}