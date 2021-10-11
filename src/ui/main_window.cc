#include <clocale>
#include "ui/main_window.h"
#include<iostream>
namespace infrared{

    MainWindow::MainWindow(const OptionManager& options):options_(options),
        window_closed_(false){
            #ifdef CUDA_ENABLED
                std::cout<<"MainWindow10086"<<std::endl;
            #endif
            std::setlocale(LC_NUMERIC, "C");
            resize(1024, 600);
            UpdateWindowTitle();
            std::cout<<"Update"<<std::endl;

            CreateWidgets();
            std::cout<<" CreateWidgets()"<<std::endl;
            CreateActions();
            std::cout<<"CreateActions()"<<std::endl;
            CreateMenus();
            std::cout<<"CreateMeaus()"<<std::endl;
            CreateToolbar();
            std::cout<<"CreateToolbar"<<std::endl;
            CreateStatusbar();
            std::cout<<"CreateStatusbar"<<std::endl;
            CreateControllers();
            
           // ShowLog();                                            
        }
    
    void MainWindow::closeEvent(QCloseEvent* event){
        // if(window_closed_){
        //     event->accept();
        //     return ;
        // }

    }

    void MainWindow::CreateWidgets(){
        model_viewer_widget_ = new ModelViewerWidget(this, &options_);
        setCentralWidget(model_viewer_widget_);
        std::cout<<"model_viewer"<<std::endl;
        project_widget_ = new ProjectWidget(this, &options_);
        project_widget_->SetDatabasePath(*options_.database_path);
        project_widget_->SetImagePath(*options_.image_path);
        std::cout<<"project_widget"<<std::endl;

        // log_widget_ = new LogWidget(this);
        // std::cout<<"dock_log_widget1"<<std::endl;
        // dock_log_widget_ = new QDockWidget("Log", this);
        // std::cout<<"dock_log_widget2"<<std::endl;
        // dock_log_widget_->setWidget(log_widget_);
        // std::cout<<"dock_log_widget"<<std::endl;
        // addDockWidget(Qt::RightDockWidgetArea, dock_log_widget_);
        render_options_widget_ =
            new RenderOptionsWidget(this, &options_, model_viewer_widget_);
        
        reconstruction_manager_widget_ =
            new ReconstructionManagerWidget(this, &reconstruction_manager_);

        
        
    }
    void MainWindow::CreateActions(){

        action_project_new_ = new QAction(QIcon(":/media/project-new.png"), tr("New Project"), this);
        action_project_new_->setShortcuts(QKeySequence::New);
        connect(action_project_new_, &QAction::triggered, this, &MainWindow::ProjectNew);

        std::cout<<"action_project_now"<<std::endl;
        action_SFM_ = new QAction(QIcon(":./media/automatic-reconstruction.png"), tr("SFM"), this);
        connect(action_SFM_, &QAction::triggered, this, &MainWindow::SFM);
        //action__SFM_->
        action_MVS_traditional_ = new QAction(QIcon(":./media/project-new.png"), tr("MVS(traditional)"), this);

        //action_MVS_traditional_ = new QAction(QIcon(":./media/project-new.png"), tr("SFM"), this);
        action_MVS_deeplearning_ = new QAction(QIcon(":./media/project-new.png"), tr("MVS(deeplearning)"), this);
        //connect()
        action_segmentation_ = new QAction(QIcon(":./media/project-new.png"), tr("Segmentation"), this);
        
        std::cout<<"action_segmentation"<<std::endl;
        action_render_ = new QAction(tr("Render"), this);
        connect(action_render_, &QAction::triggered, this, &MainWindow::Render,
          Qt::BlockingQueuedConnection);
        std::cout<<"action_render"<<std::endl;

        action_render_now_ = new QAction(tr("Render now"), this);
        std::cout<<"action_render"<<std::endl;
        render_options_widget_->action_render_now = action_render_now_;
        std::cout<<"action_render"<<std::endl;
        connect(action_render_now_, &QAction::triggered, this, &MainWindow::RenderNow,
          Qt::BlockingQueuedConnection);
        std::cout<<"action_render_now"<<std::endl;

    }

    void MainWindow::CreateMenus(){
        QMenu* file_menu = new QMenu(tr("File"), this);
        file_menu->addAction(action_project_new_);
        //file_menu->addAction(action_project_save_);
        //file_menu->addAction(action_project_import);
        //TODO 
        menuBar()->addAction(file_menu->menuAction());
        menuBar()->setNativeMenuBar(false);

        QMenu* reconstruction_menu = new QMenu(tr("3D Reconstruction"));
        reconstruction_menu->addAction(action_SFM_);
        reconstruction_menu->addAction(action_MVS_traditional_);
        reconstruction_menu->addAction(action_MVS_deeplearning_);
        menuBar()->addAction(reconstruction_menu->menuAction());
        menuBar()->setNativeMenuBar(false);


        QMenu* segmentation_menu = new QMenu(tr("Segmention"));
        segmentation_menu->addAction(action_segmentation_);
        menuBar()->addAction(segmentation_menu->menuAction());
        menuBar()->setNativeMenuBar(false);

        QMenu* simulation_menu = new QMenu(tr("Simulation"));
        menuBar()->addAction(simulation_menu->menuAction());
        menuBar()->setNativeMenuBar(false);
      //  simulation_menu->addAction("")


    }

    void MainWindow::CreateToolbar(){

    }

    void MainWindow::CreateControllers(){
        if(mapper_controller_){
            mapper_controller_->Stop();
            mapper_controller_->Wait();
        }

        mapper_controller_.reset(new IncrementalMapperController(
            options_.mapper.get(), *options_.image_path, *options_.database_path,
            &reconstruction_manager_));

        mapper_controller_->AddCallback(
            IncrementalMapperController::INITIAL_IMAGE_PAIR_REG_CALLBACK, [this]() {
            if (!mapper_controller_->IsStopped()) {
               action_render_now_->trigger();
            }
        });
        mapper_controller_->AddCallback(
            IncrementalMapperController::NEXT_IMAGE_REG_CALLBACK, [this]() {
            if (!mapper_controller_->IsStopped()) {
                action_render_->trigger();
            }
        });
        mapper_controller_->AddCallback(
            IncrementalMapperController::LAST_IMAGE_REG_CALLBACK, [this]() {
            if (!mapper_controller_->IsStopped()) {
                action_render_now_->trigger();
            }
        });
        mapper_controller_->AddCallback(
            IncrementalMapperController::FINISHED_CALLBACK, [this]() {
            if (!mapper_controller_->IsStopped()) {
                action_render_now_->trigger();
               // action_reconstruction_finish_->trigger();
            }
            if (reconstruction_manager_.Size() == 0) {
               // action_reconstruction_reset_->trigger();
            }
        });

    }

    void MainWindow::CreateStatusbar(){
        QFont font;
        font.setPointSize(11);

        statusbar_timer_label_ = new QLabel("Time 00:00:00:00", this);
        statusbar_timer_label_->setFont(font);
        statusbar_timer_label_->setAlignment(Qt::AlignCenter);
        statusBar()->addWidget(statusbar_timer_label_, 1);

        statusbar_timer_ = new QTimer(this);
        connect(statusbar_timer_, &QTimer::timeout, this, &MainWindow::UpdateTimer);
        statusbar_timer_->start(1000);

        model_viewer_widget_->statusbar_status_label =
            new QLabel("0 Images - 0 Points", this);
        model_viewer_widget_->statusbar_status_label->setFont(font);
        model_viewer_widget_->statusbar_status_label->setAlignment(Qt::AlignCenter);
        statusBar()->addWidget(model_viewer_widget_->statusbar_status_label, 1);



    }

    void MainWindow::UpdateWindowTitle(){
        setWindowTitle(QString::fromStdString("InFrared Simulation"));
    }

    void MainWindow::ProjectNew(){
        project_widget_->Reset();
        project_widget_->show();
        project_widget_->raise();
    }

    void MainWindow::UpdateTimer() {
        const int elapsed_time = static_cast<int>(timer_.ElapsedSeconds());
        const int seconds = elapsed_time % 60;
        const int minutes = (elapsed_time / 60) % 60;
        const int hours = (elapsed_time / 3600) % 24;
        const int days = elapsed_time / 86400;
        statusbar_timer_label_->setText(QString().sprintf(
      "Time %02d:%02d:%02d:%02d", days, hours, minutes, seconds));
    }
    
    void MainWindow::SFM(){
        feature_extract_matching_ = new FeatureExtractMatchController(options_, &reconstruction_manager_);
        feature_extract_matching_->Start();
        feature_extract_matching_->Wait();

        std::cout<<"feature extract match over"<<std::endl;


        if(!mapper_controller_->IsStarted() && !options_.Check()){
            ShowInvalidProjectError();
            return ;
        }
        std::cerr<<"here 1"<<std::endl;

        if(mapper_controller_->IsFinished() && HasSelectedReconstruction()){
            QMessageBox::critical(this, "", tr("Reset reconstruction before starting."));
            return ;
        }

        std::cerr<<"here 2"<<std::endl;
        if(mapper_controller_->IsStarted()){
            timer_.Resume();
            mapper_controller_->Resume();
        }
        else{
            std::cerr<<"here 3"<<std::endl;
            CreateControllers();
            std::cout<<"here"<<std::endl;
            timer_.Restart();
            mapper_controller_->Start();
            //action_reos
        }
       // DisableBlockingActions();


        
    }
    void MainWindow::Render() {
        if (reconstruction_manager_.Size() == 0) {
        return;
        }

        const Reconstruction& reconstruction =
        reconstruction_manager_.Get(SelectedReconstructionIdx());

        int refresh_rate;
        if (options_.render->adapt_refresh_rate) {
        refresh_rate = static_cast<int>(reconstruction.NumRegImages() / 50 + 1);
        } else {
        refresh_rate = options_.render->refresh_rate;
         }

        if (!render_options_widget_->automatic_update ||
        render_options_widget_->counter % refresh_rate != 0) {
        render_options_widget_->counter += 1;
         return;
        }

        render_options_widget_->counter += 1;

        RenderNow();
    }

    void MainWindow::RenderNow() {
        std::cerr<<"render now"<<std::endl;
        reconstruction_manager_widget_->Update();
        RenderSelectedReconstruction();
    }

    void MainWindow::RenderSelectedReconstruction() {
        std::cerr<<"RenderSelectedReconstruction"<<std::endl;
        if (reconstruction_manager_.Size() == 0) {
        RenderClear();
        return;
        }
        std::cerr<<"RenderSelectedReconstruction2"<<std::endl;
        const size_t reconstruction_idx = SelectedReconstructionIdx();
        std::cerr<<"RenderSelectedReconstruction3"<<std::endl;
        model_viewer_widget_->reconstruction =
            &reconstruction_manager_.Get(reconstruction_idx);
        std::cerr<<"RenderSelectedReconstruction4"<<std::endl;
        model_viewer_widget_->ReloadReconstruction();
        std::cerr<<"RenderSelectedReconstruction Over"<<std::endl;
    }
    void MainWindow::RenderClear() {
        reconstruction_manager_widget_->SelectReconstruction(
        ReconstructionManagerWidget::kNewestReconstructionIdx);
        model_viewer_widget_->ClearReconstruction();
    }

    void MainWindow::RenderOptions() {
        render_options_widget_->show();
        render_options_widget_->raise();
    }

    void MainWindow::SelectReconstructionIdx(const size_t) {
        RenderSelectedReconstruction();
    }

    size_t MainWindow::SelectedReconstructionIdx() {
        size_t reconstruction_idx =
            reconstruction_manager_widget_->SelectedReconstructionIdx();
        if (reconstruction_idx ==
        ReconstructionManagerWidget::kNewestReconstructionIdx) {
        if (reconstruction_manager_.Size() > 0) {
            reconstruction_idx = reconstruction_manager_.Size() - 1;
        }
        }
        return reconstruction_idx;
    }

    bool MainWindow::HasSelectedReconstruction() {
        const size_t reconstruction_idx =
        reconstruction_manager_widget_->SelectedReconstructionIdx();
        if (reconstruction_idx ==
        ReconstructionManagerWidget::kNewestReconstructionIdx) {
        if (reconstruction_manager_.Size() == 0) {
            return false;
        }
        }
        return true;
    }   

    bool MainWindow::IsSelectedReconstructionValid() {
        if (!HasSelectedReconstruction()) {
        QMessageBox::critical(this, "", tr("No reconstruction selected"));
        return false;
    }
    return true;
    }


    void MainWindow::ReconstructionStats() {
        if (!IsSelectedReconstructionValid()) {
            return;
        }
        reconstruction_stats_widget_->show();
        reconstruction_stats_widget_->raise();
        reconstruction_stats_widget_->Show(
        reconstruction_manager_.Get(SelectedReconstructionIdx()));
    }

    void MainWindow::ShowInvalidProjectError(){
        QMessageBox::critical(this, "",
                        tr("You must create a valid project using: <i>File > "
                           "New project</i> or <i>File > Edit project</i>"));
    }

   // void MainWindow::EnableBlockingActions(){
        //for(auto)
   // }
} //namespace infrared