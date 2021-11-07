#include<torch/torch.h>
#include<string>
#include<iostream>
#include<sstream>
#include<opencv2/opencv.hpp>


//namespace infrared{
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args){
    size_t size = 1 + snprintf(nullptr, 0, format.c_str(), args ...);
    char bytes[size];
    snprintf(bytes, size, format.c_str(), args ...);

    return std::string(bytes);
}
struct Meta{
    std::string scan_name;
    size_t ref_view;
    std::vector<size_t> src_views;
    Meta(std::string& scan_name, int ref_view, std::vector<size_t>& src_views)
    :scan_name(scan_name), ref_view(ref_view), src_views(src_views){}
};

class MVSDataset : public torch::data::Dataset<MVSDataset>{
 private:
    std::unordered_map<std::string, torch::Tensor> sample;
    std::string path;
    std::string mode;
    int nviews;
    int ndepth;
    int max_w;
    int max_h;
    float interval_scale;
    std::vector<Meta> metas;
    std::vector<std::string> listfile;
    std::unordered_map<std::string, float> interval_scale_dict;

 public:
    MVSDataset(std::string& path, std::vector<std::string>& listfile, std::string& mode, int nviews, int max_w, int max_h, int ndepth=192, float interval_scale=1.06);
    torch::Tensor read_data(std::string location);
    std::tuple<torch::Tensor, torch::Tensor, std::pair<float, float> > read_cam_file(std::string loc, const float interval_scale);
    void scale_mvs_input(cv::Mat& img, torch::Tensor& intrinsic, int max_w, int max_h, int base=32);
   // torch::data::Example<> get(size_t index) override;
    torch::optional<size_t> size() const override{
        return metas.size();
    }
    std::vector<Meta> build_list();
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
            std::string img_filename = string_format("%s/%s/images/%08d.jpg", path, scan, view_ids[i]);

            std::string  proj_mat_filename = string_format("%s/%s/cams/%08d_cam.txt", path, scan, view_ids[i]);

            torch::Tensor img = read_data(img_filename);

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
};

//} \\namespace infrared


