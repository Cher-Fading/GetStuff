#include <istream>
#include <ostream>

#include <sstream>
#include <dirent.h>

void get_to_root(const char *dataType = "")
{
<<<<<<< HEAD
    ofstream out(Form("%s_root.txt", dataType));
    std::string input = "/pnfs/usatlas.bnl.gov/users/cher97/rucio/user.xiaoning";
=======
    ofstream out(Form("../GetStuff/%s_root.txt", dataType), std::ofstream::trunc);
    std::string input = "/pnfs/usatlas.bnl.gov/users/cher97/rucio/user.xiaoning/";
>>>>>>> ea467e342808a50d88c44205ed53350ef11b63c3
    DIR *dir1;
    dirent *pdir;
    dir1 = opendir(input.c_str());
    while ((pdir = readdir(dir1)))
    {
        std::string foldName = pdir->d_name;
        cout << pdir->d_name << endl;
        if (foldName.find(dataType) == std::string::npos)
            continue;
        if (!(foldName.find(".root") == std::string::npos))
            continue;
        //if(foldName.find("JZ5")==std::string::npos) continue;
        cout << "Success:" << pdir->d_name << endl;
        DIR *dir2;
        dirent *pdir2;
        dir2 = opendir((input + "/" + foldName).c_str());
        while ((pdir2 = readdir(dir2)))
        {
            std::string fName = pdir2->d_name;
            if (fName.find(".root") == std::string::npos){
                cout << "wrong file name" << input + "/" + foldName + "/" + fName << endl;
                continue;
            }

            //if (fName.find("JZ5") == std::string::npos)
            //	continue;
            out << input + "/" + foldName + "/" + fName << endl;
            //goto here;
        }
    }
    out.close();
}