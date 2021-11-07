#ifndef INFRADRED_UI_MAIN_H_
#define INFRADRED_UI_MAIN_H_

#include<QtCore>
#include<QtGui>
#include<QtWidgets>

#include "base/reconstruction.h"
#include "controllers/incremental_mapper.h"
#include "controllers/feature_extract_matching.h"
//#include "ui/automatic_reconstruction_widget.h"
// #include "ui/bundle_adjustment_widget.h"
#include "ui/database_management_widget.h"
#include "ui/dense_reconstruction_widget.h"
//#include "ui/feature_extraction_widget.h"
//#include "ui/feature_matching_widget.h"
// #include "ui/license_widget.h"
#include "ui/log_widget.h"
// #include "ui/match_matrix_widget.h"
#include "ui/model_viewer_widget.h"
#include "ui/project_new_widget.h"
#include "ui/reconstruction_manager_widget.h"
#include "ui/reconstruction_options_widget.h"
#include "ui/reconstruction_stats_widget.h"
#include "ui/render_options_widget.h"
// #include "ui/undistortion_widget.h"
// #include "util/bitmap.h"
#include "util/option_manager.h"

namespace infrared {
class MainWindow : public QMainWindow {
    public:
        //explicit MainWindow(const OptionManager& options);
        MainWindow(const OptionManager& options);
        
    
    protected:
        void closeEvent(QCloseEvent* event);

    private:
        friend class AutomaticReconstructionWidget;
        friend class BundleAdjustmentWidget;
        friend class DenseReconstructionWidget;


        void CreateWidgets();
        void CreateActions();
        void CreateMenus();
        void CreateToolbar();
        void CreateStatusbar();
        void CreateControllers();
        void UpdateWindowTitle();
        void ProjectNew();
        void ProjectEdit();
        void RenderNow();
        void Render();
        void RenderModel();
        void Import();
        void ImportPLY();
        void SFM();
        void MVSTra();
        void RenderSelectedReconstruction();
        void RenderClear();
        void RenderOptions();
        void SelectReconstructionIdx(const size_t);
        size_t SelectedReconstructionIdx();
        bool HasSelectedReconstruction();
        bool IsSelectedReconstructionValid();
        void ReconstructionStats();
        void ShowInvalidProjectError();
        void UpdateTimer();



        bool window_closed_;
        QAction* action_project_new_;
        QAction* action_project_import_;
        QAction* action_project_importply_;
        QAction* action_SFM_;
        QAction* action_MVS_traditional_;
        QAction* action_MVS_deeplearning_;
        QAction* action_segmentation_;
        QAction* action_render_;
        QAction* action_render_now_;
        QAction* action_render_model_;
        QAction* action_project_edit_;

        OptionManager options_;
        ReconstructionManager reconstruction_manager_;
        FeatureExtractMatchController* feature_extract_matching_;
        std::unique_ptr<IncrementalMapperController> mapper_controller_;
        Timer timer_;

        ProjectWidget* project_widget_;
        ModelViewerWidget* model_viewer_widget_;
        LogWidget* log_widget_;
        ReconstructionManagerWidget* reconstruction_manager_widget_;
        ReconstructionStatsWidget* reconstruction_stats_widget_;
        ReconstructionOptionsWidget* reconstruction_options_widget_;
        DatabaseManagementWidget* database_management_widget_;
        DenseReconstructionWidget* dense_reconstruction_widget_;
        RenderOptionsWidget* render_options_widget_;
        ThreadControlWidget* thread_control_widget_;





        QDockWidget* dock_log_widget_;
        QTimer* statusbar_timer_;
        QLabel* statusbar_timer_label_;

        


};
}

#endif