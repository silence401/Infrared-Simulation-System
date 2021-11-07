#include "ui/mesh_painter.h"
#include "util/opengl_utils.h"

namespace infrared
{

    MeshPainter::MeshPainter() : num_geoms_(0);
    MeshPainter::~MeshPainter(){
        vao_.destroy();
        vbo_.destroy();
        ebo_.destroy();
    }

    void MeshPainter::Setup(){
        vao_.destroy();
        vbo_.destroy();
        ebo_.destroy();
        if(shader_program_.isLinked()){
            shader_program_.release();
            shader_program_.removeAllShaders();
        }
        shader_program_.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          ":/shaders/triangles.v.glsl");
        shader_program_.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                          ":/shaders/triangles.f.glsl");
        shader_program_.link();
        shader_program_.bind();

        vao_.create();
        vbo_.create();
        ebo_.create();

        #if DEBUG
            glDebugLog();
        #endif
    }

    void MeshPainter::Upload(const std::vector<MeshPainter::DataPoint>& datap, const std::vector<MeshPainter::DataIndices>& datai){
        num_geoms_ = datai.size()/3;
        if(num_geoms_ == 0)
            return;

        vao_.bind();
        vbo_.bind();
        ebo_.bind();

        vbo_.allocate(datap.data(), static_cast<int>(datap.size()*sizeof(MeshPainter::DataPoint)));
        ebo_.allocate(datai.data(), static_cast<int>(datai.size()*sizeof(MeshPainter::DataIndices)));

        shader_program.enableAttributeArray("a_position");
        shader_program_.setAttributeBuffer("a_position", GL_FLOAT, 0, 3, sizeof(PointPainter::Data));

        
    }
    
} // namespace infrared
