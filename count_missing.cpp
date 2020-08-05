void count_missing(const char *dataType)
{
    int JZ_ID[grid_size];
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
                JZ_ID[itemj[k + 2] - 48] = std::stoi(id);
                cout << itemj[k + 2] - 48 << ": " << id << endl;
                JZ_wt[itemj[k + 2] - 48] = 0;
            }
            ++linePosj;
        }
    }

    std::ifstream fnames(Form("../GetStuff/%s_root_pnfs.txt", dataType));
    std::ifstream ipnames("../GetStuff/test_ipnames.txt");
		std::string line;
		int counter = 0;
		while (std::getline(fnames, line))
		{
			int k = line.find("Akt4HIJets");
			if (k == std::string::npos)
			{
				cout << "Wrong name in reading: " << line << endl;
				return;
			}
			counter++;

			bool found = false;

			for (int j = 0; j < grid_size; j++)
			{
				if (std::stoi(line.substr(k - 9, 8)) == JZ_ID[j])
				{
					found = true;
					JZ = j;
				}
			}
			if (!found)
			{
				cout << line << endl;
				cout << "filename JZ info not found" << endl;
				return;
			}
			//if (JZ != 0) continue;
			f = TFile::Open(line.c_str(), "READ");
			myChain = (TTree *)f->Get(chain_name.c_str());
			int newct = myChain->GetEntries();
			if (newct < 0 || newct > 30000)
			{
				cout << "file: " << line << "; ct: " << newct << endl;
				return;
			}
			//cout << counter << ": " << line << ": " << newct << endl;
			//cout << JZ_wt[JZ] << endl;
			JZ_wt[JZ] = JZ_wt[JZ] + newct;
			//cout << JZ_wt[JZ] << endl;
			//cout << endl;
			f->Close();
			f = 0;
			myChain = 0;
			if (counter % 100 == 0)
			{
				cout << "counted " << counter << " files" << endl;
			}
		}
}