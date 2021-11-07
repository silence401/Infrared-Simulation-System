#include "dataset.h"
int main(){

    std::vector<std::string> listfile;
    listfile.push_back("test");
    std::string path = "home/下载/dtu";
    std::string mode = "test";
    auto mvsdateset = MVSDataset(path, listfile, mode, 3, 1600, 1200);
    
}
/***
 *  MVSDateset(std::string& path, std::vector<std::string>& listfile, std::string& mode, int nviews, int max_w, int max_h, int ndepth=192, float interval_scale=1.06):
    path(path), listfile(listfile), mode(mode), nviews(nviews), max_w(max_w), max_h(max_h), ndepth(ndepth), interval_scale(interval_scale){
        metas = build_list();
    }
***/