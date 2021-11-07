#ifndef COLMAP_SRC_UI_TRAINGLE_PAINTER_H_
#define COLMAP_SRC_UI_TRAINGLE_PAINTER_H_

#include <QtCore>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_2_Core>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#include "ui/point_painter.h"
#include "util/stb_image.h"

namespace infrared{

struct Vertex{
    //position
    Vertex():x(0), y(0), z(0), r(0), g(0),b(0),a(0),nx(0),ny(0),nz(0),tx(0),ty(0){}
    float x, y, z;

    float r, g, b, a;
    //normal
    float nx, ny, nz;
    // texture
    float tx, ty;
    //color
    //unsigned char r, g, b, a;
};

struct Texture{
  // Texture(size_t id_, std::string type_, std::string path_):id(id_), type(type_), path(path_){}
    int id;
    std::string type;
    std::string path;
};


class Mesh{
 public:
    Mesh(std::vector<Vertex>& vertices_, std::vector<Texture>& textures_, std::vector<unsigned int>& indices);
    ~Mesh();
 //private:
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;
    std::vector<unsigned int> indices;
};


class MeshPainter{
 public:
    MeshPainter(); 
    ~MeshPainter();
  
    void Setup();
    void Upload(const Mesh& mesh);
    void Render(const QMatrix4x4& pmv_matrix);

 private:
    int num_geos;
    std::vector<Texture> textures;
    QOpenGLShaderProgram shader_program_;
    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer vbo_;
    QOpenGLBuffer ebo_;
    QOpenGLTexture texture_;



};

size_t TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model{
 public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;
    
    Model(std::string const& path, bool gamma = false);

   // void Draw(const QMatrix4x4& pmv_matrix){}

 private:
  void loadModel(std::string const &path);
  void processNode(aiNode* node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);

  void loadMaterialTextures(aiMaterial *mat, aiTextureType type, Texture& textures);
 };
} //namespace
#endif