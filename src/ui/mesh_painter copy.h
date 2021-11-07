#ifndef COLMAP_SRC_UI_TRAINGLE_PAINTER_H_
#define COLMAP_SRC_UI_TRAINGLE_PAINTER_H_

#include <QtCore>
#include　<QtOpenGL>
#include "ui/point_painter.h"

namespace infrared{
class MeshPainter{
 public:
    MeshPainter();
    ~MeshPainter();
    
    struct DataPoint
    {
        DataPoint(){}
        DataPoint(const PointPainter::Data& p):point(p){}
        
        PointPainter::Data point;
        /* data */
    };
    struct DataIndices
    {
        DataIndices(){
        }
        DataIndices(const size_t ind):indices(ind){}

        size_t indices;
        /* data */
    };

    void Setup();
    void Upload(const std::vector<MeshPainter::DataPoint>& datap, const std::vector<MeshPainter::DataIndices>& datai);
    void Render(const QMatrix4x4& pmv_matrix);

 private:
    QOopenGLShaderProgram shader_program;
    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer vbo_;
    QOopenGLBuffer ebo_;

    size_t num_geoms_;
    
    
};



} //namespace
#endif