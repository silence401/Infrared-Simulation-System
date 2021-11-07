#include "dataset.h"

//namespace infrared{
    MVSDataset::MVSDataset(std::string& path, std::vector<std::string>& listfile, std::string& mode, int nviews, int max_w, int max_h, int ndepth, float interval_scale):
    path(path), listfile(listfile), mode(mode), nviews(nviews), max_w(max_w), max_h(max_h), ndepth(ndepth), interval_scale(interval_scale){
        metas = build_list();
    }

    torch::Tensor MVSDataset::read_data(std::string loc){
        cv::Mat img = cv::imread(loc, 1);
        torch::Tensor img_tensor = torch::from_blob(img.data, {img.rows, img.cols, 3}, torch::kByte);
        img_tensor = img_tensor.permute({2, 0, 1});
        img_tensor = img_tensor / 255.0;
        return img_tensor.clone();
    }

    std::tuple<torch::Tensor, torch::Tensor, std::pair<float, float> > MVSDataset::read_cam_file(std::string loc, const float interval_scale){
        std::ifstream cam_file(loc);
        std::string type_name;
        cam_file >> type_name;
        assert(type_name == "extrinsic"); //line 1
       // cv::Mat extrinsics = cv::Mat::zeros(4, 4, CV_32F);
        torch::Tensor extrinsics = torch::zeros({4,4});
       // extrinsics.index({3,3}) = 1; 
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                float temp;
                cam_file >> temp;
                extrinsics.index({i,j}) = temp;
            }
        }
        cam_file >> type_name;
        assert(type_name == "intrinsic");
       // cv::Mat intrinsic = cv::Mat::zeros(3, 3, CV_32F);
        torch::Tensor intrinsic = torch::zeros({3, 3});
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                float temp;
                cam_file >> temp;
               // cam_file>>intrinsic.at<float>(i, j);
                intrinsic.index({i, j}) = temp;
                if(i < 2){
                   // intrinsic.at<float>(i, j) /= 4.0;
                    intrinsic.index({i, j}) /= 4.0;
                }
            }
        }
        
        float depth_min = 0.0f;
        float depth_interval = 0.0f;

        cam_file >> depth_min;
        cam_file >> depth_interval;

        depth_interval *= interval_scale;

       // torch::Tensor extrinsics_tensor = torch::from_blob(extrinsics.data, {4, 4}, torch::kFloat);
        //torch::Tensor intrinsic_tensor = torch::from_blob(intrinsic.data, {3, 3}, torch::kFloat);



        return {extrinsics, intrinsic, {depth_min, depth_interval}};
    }

    void MVSDataset::scale_mvs_input(cv::Mat& img, torch::Tensor& intrinsic, int max_w, int max_h, int base){
        int h = img.rows;
        int w = img.cols;
        int new_h = h;
        int new_w = w;
        if(h > max_h || w > max_w){
            float scale = 1.0*max_h / h;
            if(scale * w > max_w){
                scale = 1.0 * max_w / w;
            }
            new_w = scale * w / base * base;
            new_h = scale * h / base * base;
        }
        else{
            new_w = 1.0 * w / base * base;
            new_h = 1.0 * h / base * base;
        }
        float scale_w = 1.0*new_w / w;
        float scale_h = 1.0*new_h / h;
        cv::Mat src = img.clone();
        cv::resize(src, img, {new_w, new_h});

        intrinsic.index({0, "..."}) = intrinsic.index({0, "..."}) *= scale_w;
        intrinsic.index({1, "..."}) = intrinsic.index({1, "..."}) *= scale_h;

    }
    std::vector<Meta> MVSDataset::build_list(){
        std::vector<std::string> meta;
        for(auto scan : listfile){
            interval_scale_dict[scan] = interval_scale;
            std::stringstream fmt;
            fmt<<scan<<"/pair.txt";
            std::string pair_loc = fmt.str();
            std::ifstream pair_file(pair_loc);
            int numviewpoints;
            pair_file>>numviewpoints;
            
            for(size_t i = 0; i < numviewpoints; i++){
                size_t ref_view;
                pair_file>>ref_view;
                std::vector<size_t> src_views;
                int nviews;
                float score;
                pair_file>>nviews;
                for(size_t j = 0; j < nviews; j++){
                    size_t temp_src_view;
                    pair_file>>temp_src_view>>score;
                    src_views.push_back(temp_src_view);
                    if(src_views.size() > 0){
                        metas.push_back(Meta(scan, ref_view, src_views));
                    }
                }
            }
        }
        return metas;
    }

    torch::data::Example<> MVSDataset::get(size_t index) override{
        Meta tmp = metas[index];
        std::vector<size_t> view_ids;
        view_ids.push_back(tmp.ref_view);
        std::vector<size_t> src_views = tmp.src_views;
        std::string scan = tmp.scan_name;
        view_ids.insert(view_ids.end(), src_views.begin(), src_views.begin()+nviews-1);

        torch::Tensor imgs;
        torch::Tensor proj_matrices = torch::zeros({4, 4});
        
        for(int i = 0; i < view_ids.size(); i++){
            std::string img_filename = string_format("%s/%s/images/%08d.jpg", datapath, scan, view_ids[i]);

            std::string  proj_mat_filename = string_format("%s/%s/cams/%08d_cam.txt", datapath, scan, view_ids[i]);

            torch::Tensor = read_data(img_filename);

            std::tuple<torch::Tensor, torch::Tensor, std::pair<float, float> > cameras_paramters = read_cam_file(proj_mat_filename);
            torch::Tensor intrinsics = std::get<0>(camera_paramters);
            torch::Tensor extrinsics = std::get<1>(camera_paramters);
            float depth_min = std::get<2>(camera_paramters).first;
            float depth_interval = std::get<2>(camera_paramters).second;

            scale_mvs_input(img, intrinsic, max_w, max_h);
            
            int s_h, s_w;
            if(fix_res){
                s_h = img.cols;
                s_w = img.rows;
                fix_res = False;
                fix_wh = True;
            }

            if(i == 0){
                if(!fix_wh){
                    s_h = img.cols;
                    s_w = img.rows;
                }
            }
            
            int c_w, c_h;
            c_h = img.cols;
            c_w = img.rows; 
            if(c_h != s_h || c_w != s_w){
                float scale_h = 1.0 * s_h / c_h;
                float scale_w = 1.0 * s_w / c_w;
                Mat img2 = img.clone();
                cv::resize(img2, img, (s_w, s_h));
            }
            intrinsics.index({0, "..."}) = intrinsics.index({0, "..."})*scale_w;
            intrinsics.index({1, "..."}) = intrinsics.index({1, "..."})*scale_h;

            torch::Tensor img_tensor = torch::from_blob(img.data, {img.rows, img.cols, 3}, torch::kFloat);
            img_tensor = img_tensor.permute({2, 0, 1});
            img_tensor = img_tensor / 255.0;
            //img_tensor = img_tensor.
            if(i == 0){
                imgs = img_tensor;
                proj_matrices.index({0, Slice(None,4), torch::Slice(None, 4)}) = extrinsics;
                proj_matrices.index({1, Slice{None, 3}, torch::Slice(None, 3)}) = intrinsics;

            }
            else{
                imgs = torch::stack({imgs, img_tensor}, 0);
                torch::Tensor proj_mat = torch::zeros({4,4});
                proj_mat.index({0, torch::Slice(None,4), torch::Slice(None, 4)}) = extrinsics;
                proj_mat.index({1, torch::Slice{None, 3}, torch::Slice(None, 3)}) = intrinsics;
                proj_matrices = torch::stack({proj_matrices, proj_mat}, 0);
            }
            
        }

        torch::Tensor stage2_pjmats = proj_matrices.clone();
        stage2_pjmats.index({"...", 1, torch::Slice(None, 2), "..."}) = proj_matrices.index({"...", 1, torch::Slice(None, 2), "..."}) * 2;
        torch::Tensor stage3_pjmar = proj_matrices.clone();
        stage3_pjmats.index({"...", 1, torch::Slice(None, 2), "..."}) = proj_matrices.index({"...", 1, torch::Slice(None, 2), "..."}) * 4;

        
        std::unordered_map<std::string, torch::Tensor> proj_matrices_ms{
            {"stage1":proj_matrices},
            {"stage2":stage2_pjmats},
            {"stage3":stage3_pjmats}
        }
        return {{{"imgs", imgs}, {"proj_matrices", proj_matrices_ms}, {"depth_value", depth_values}, {"filename", "temp"}}, None};
    }
    torch::optional size() const override{
        return metas.size();
    }
  
};

//} \\namespace infrared


