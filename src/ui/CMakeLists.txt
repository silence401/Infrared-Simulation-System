set(FOLDER_NAME "ui")
#message("UIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIi")
COLMAP_ADD_SOURCES(
	main_window.h main_window.cc
	project_new_widget.h project_new_widget.cc
	render_options.h render_options.cc
	colormaps.cc colormaps.h
	image_viewer_widget.cc image_viewer_widget.h
	line_painter.cc line_painter.h
	log_widget.cc log_widget.h
	movie_grabber_widget.cc movie_grabber_widget.h
	point_painter.cc point_painter.h
	qt_utils.cc qt_utils.h
	triangle_painter.cc triangle_painter.h
	point_viewer_widget.cc point_viewer_widget.h
	model_viewer_widget.cc model_viewer_widget.h
	options_widget.cc options_widget.h
	reconstruction_manager_widget.cc reconstruction_manager_widget.h
	reconstruction_options_widget.cc reconstruction_options_widget.cc
	render_options_widget.cc render_options_widget.h
	database_management_widget.cc database_management_widget.h
	dense_reconstruction_widget.cc dense_reconstruction_widget.h
	thread_control_widget.cc thread_control_widget.h
	bundle_adjustment_widget.cc bundle_adjustment_widget.h
	reconstruction_stats_widget.cc reconstruction_stats_widget.h
	mesh_painter.cc mesh_painter.h

)
#qt5_add_resource()



