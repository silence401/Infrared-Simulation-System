#include "ui/mesh_painter.h"
#include "util/opengl_utils.h"
#include <iostream>
#include <time.h>

namespace infrared
{

    Mesh::Mesh(std::vector<Vertex>& vertices_, std::vector<Texture>& textures_, std::vector<unsigned int>& indices_){
        vertices = vertices_;
        textures = textures_;
        indices = indices_;
    }
    Mesh::~Mesh(){
        vertices.clear();
        textures.clear();
        indices.clear();
    }



    MeshPainter::MeshPainter(): num_geos(0), vbo_(QOpenGLBuffer::VertexBuffer), ebo_(QOpenGLBuffer::IndexBuffer), texture_(QOpenGLTexture::Target2D){}
    //MeshPainter::MeshPainter():vbo_(QOpenGLBuffer::VertexBuffer), ebo_(QOpenGLBuffer::IndexBuffer),num_geos(0){}
    MeshPainter::~MeshPainter(){
        vao_.destroy();
        vbo_.destroy();
        ebo_.destroy();
        texture_.destroy();
    }

    void MeshPainter::Setup(){
        vao_.destroy();
        vbo_.destroy();
        ebo_.destroy();
        texture_.destroy();
        if(shader_program_.isLinked()){
            shader_program_.release();
            shader_program_.removeAllShaders();
        }
        shader_program_.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          ":/shaders/model_loading.vs");
        shader_program_.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                          ":/shaders/model_loading.fs");
        shader_program_.link();
        shader_program_.bind();

        vao_.create();
        vbo_.create();
        ebo_.create();
        texture_.create();

        #if DEBUG
            glDebugLog();
        #endif
    }

    void MeshPainter::Upload(const Mesh& mesh){
        std::cout<<"UPLOAD"<<std::endl;
        // std::cout<<mesh.textures[0].path<<std::endl;
        // std::cout<<mesh.indices.size()<<std::endl;
        // std::cout<<mesh.vertices.size()<<std::endl;
        // std::cout<<"texcoord: "<<mesh.vertices[0].tx<<' '<<mesh.vertices[0].ty<<std::endl;
        // std::cout<<mesh.vertices[0].x<<' '<<mesh.vertices[0].y<<' '<<mesh.vertices[0].z<<std::endl;
       // std::cout<<mesh.indices[0]<<' '<<mesh.indices[1]<<' '<<mesh.indices[2]<<' '<<mesh.indices[3]<<std::endl;
        //textures = mesh.textures;
       // num_geos = mesh.vertices.size();
        num_geos = mesh.indices.size();
        vao_.bind();
        vbo_.bind();
        ebo_.bind();
        QImage* image = new QImage(mesh.textures[0].path.c_str());
        // if(image->isNull()){
        //     std::cout<<"???????????"<<std::endl;
        // }
        // //std::cout<<image->height<<std::endl;
        // std::cout<<mesh.textures[0].path.c_str()<<std::endl;
       // if(!Exists)
        //if(QImage())
        texture_.setData(*image);
        texture_.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
        texture_.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
        texture_.setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Linear); 
        //texture_.bind(0);
        vbo_.setUsagePattern(QOpenGLBuffer::DynamicDraw);
        ebo_.setUsagePattern(QOpenGLBuffer::DynamicDraw);
        vbo_.allocate(mesh.vertices.data(), static_cast<int>(mesh.vertices.size()*sizeof(Vertex)));
        ebo_.allocate(mesh.indices.data(), static_cast<int>(mesh.indices.size()*sizeof(unsigned int)));
       
        shader_program_.enableAttributeArray("a_position");
        shader_program_.setAttributeBuffer("a_position", GL_FLOAT, 0, 3, sizeof(Vertex));

        shader_program_.enableAttributeArray("a_color");
        shader_program_.setAttributeBuffer("a_color", GL_FLOAT, 3 * sizeof(GLfloat),
                                     4, sizeof(Vertex));
         shader_program_.enableAttributeArray("a_normal");
         shader_program_.setAttributeBuffer("a_normal", GL_FLOAT, 7*sizeof(GLfloat), 3, sizeof(Vertex));
         shader_program_.enableAttributeArray("a_texcoord");
         shader_program_.setAttributeBuffer("a_texcoord", GL_FLOAT, 10*sizeof(GLfloat), 2, sizeof(Vertex));
        // shader_program_.enableAttributeArray("a_color");
        // shader_program_.setAttributeBuffer("a_color", GL_UNSIGNED_BYTE, 8*sizeof(GLubyte), 4, sizeof(Vertex));
        shader_program_.setUniformValue("texture_map", 0);

        vao_.release();
        vbo_.release();
        ebo_.release();
        texture_.release();

        #if DEBUG
            glDebugLog();
        #endif

    }

    void MeshPainter::Render(const QMatrix4x4& pmv_matrix){
        if(num_geos == 0)
            return ;
        
        shader_program_.bind();
        vao_.bind();
        texture_.bind(0);
       // std::cout<"VAOID:"<<vao_.objectId()<<std::endl;
        //std::cout<<"Mesh Render"<<std::endl;
        QOpenGLFunctions* gl_funcs = QOpenGLContext::currentContext()->functions();
       // std::cout<<QOpenGLContext::currentContext()->
        shader_program_.setUniformValue("u_pmv_matrix", pmv_matrix);
        shader_program_.setUniformValue("texture_map", 0);
       // shader_program_.setUniformValue("u_point_size", 1.0f);
       // std::cout<<"here1"<<std::endl;
        // size_t diffuseNr  = 1;
        // size_t specularNr = 1;
        // size_t normalNr   = 1;
        // size_t heightNr   = 1;
        // for(size_t i = 0; i<textures.size(); i++){
           
        //     gl_funcs->glActiveTexture(GL_TEXTURE0 + i);
        //     std::string number;
        //     std::string name = textures[i].type;
        //     if(name == "texture_diffuse")
        //         number = std::to_string(diffuseNr++);
        //     else if(name == "texture_specular")
        //         number = std::to_string(specularNr++); // transfer unsigned int to stream
        //     else if(name == "texture_normal")
        //         number = std::to_string(normalNr++); // transfer unsigned int to stream
        //      else if(name == "texture_height")
        //         number = std::to_string(heightNr++); // transfer unsigned int to stream

        //    // shader_program_.setUniformValue()
        //    gl_funcs->glUniform1i(gl_funcs->glGetUniformLocation(shader_program_.programId(), (name+number).c_str()), i);
        //    gl_funcs->glBindTexture(GL_TEXTURE_2D, textures[i].id);
        // }

        gl_funcs->glDrawElements(GL_TRIANGLES, (GLsizei)num_geos, GL_UNSIGNED_INT, nullptr);
        //gl_funcs->glDrawArrays(GL_POINTS, 0, (GLsizei)num_geos);
        //std::cout<<"here2:"<<num_geos<<std::endl;

        vao_.release();
        //texture_.release(); 
        //gl_funcs->glActiveTexture(GL_TEXTURE0);
        #if DEBUG
        glDebugLog();
        #endif
    }
    
// int TextureFromFile(const char *path, const std::string &directory, bool gamma){
//     std::string filename = std::string(path);
//     filename = directory + '/' + filename;
//     std::cout<<"filename "<<filename<<std::endl;
//     GLuint textureID;
//     //makecurrent();
//    // QOpenGLFunctions* gl_funcs = QOpenGLContext::currentContext()->functions();
//    // std::cout<<"load"<<std::endl;std::cout<<"load"<<std::endl;
//     glGenTextures(1, &textureID);
//     //std::cout<<"load"<<std::endl;
//     int width, height, nrComponents;
//     unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//     //std::cout<<"load"<<std::endl;
//     if(data){
//         std::cout<<"data true"<<std::endl;
//         GLenum format;
//         if(nrComponents == 1)
//             format = GL_RED;
//         else if(nrComponents == 3)
//             format =GL_RGB;
//         else if(nrComponents == 4)
//             format = GL_RGBA;

//         glBindTexture(GL_TEXTURE_2D, textureID);
//         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);

//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//         stbi_image_free(data);
//     } 
//     else{
//         std::cout<<"Texture failed to load"<<path<<std::endl;
//         stbi_image_free(data);
//     }
//     return textureID;
//     }


    Model::Model(std::string const &path, bool gamma):gammaCorrection(gamma){
        loadModel(path);
    }

    // void Model::Draw(const QMatrix4x4& pmv_matrix){
    //     for(size_t i = 0; i < meshes.size(); i++){
    //         meshes[i].Render(pmv_matrix);
    //     }
    // }

    void Model::loadModel(std::string const& path){
        time_t first, second;
        Assimp::Importer importer;
        first = time(nullptr);
       // const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs| aiProcess_CalcTangentSpace);
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate|aiProcess_FlipUVs);
        second = time(nullptr);
        std::cout<<difftime(second, first)<<std::endl;
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            std::cout<<"Error::ASSIMP: "<<importer.GetErrorString()<<std::endl;
        }
        directory = path.substr(0, path.find_last_of('/'));
        //std::cout<<"directory: "<<directory<<std::endl;

        processNode(scene->mRootNode, scene);
        std::cout<<"processNode Over"<<std::endl;
    }

    void Model::processNode(aiNode* node, const aiScene *scene){
        for(size_t i = 0; i < node->mNumMeshes; i++){
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            //std::cout<<i<<' '<<"processMesh"<<std::endl;
            meshes.push_back(processMesh(mesh, scene));
        }

        for(size_t i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], scene);
        }

    }
    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene){
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        for(size_t i = 0; i < mesh->mNumVertices; i++){
            Vertex vertex;
            vertex.x = mesh->mVertices[i].x;
            vertex.y = mesh->mVertices[i].y;
            vertex.z = mesh->mVertices[i].z;

            if(mesh->HasNormals()){
                vertex.nx = mesh->mNormals[i].x;
                vertex.ny = mesh->mNormals[i].y;
                vertex.nz = mesh->mNormals[i].z;
            }

            if(mesh->mTextureCoords[0]){
                vertex.tx = mesh->mTextureCoords[0][i].x;
                vertex.ty = mesh->mTextureCoords[0][i].y;
            }
            else{
                vertex.tx = 0.0f;
                vertex.ty = 0.0f;
            }

            if(mesh->mColors[0]){
                vertex.r = mesh->mColors[0][i].r;
                vertex.g = mesh->mColors[0][i].g;
                vertex.b = mesh->mColors[0][i].b;
                vertex.a = mesh->mColors[0][i].a;
                vertex.a = 1.0f; //不透明
            }
            vertices.push_back(vertex);

        }
        for(size_t i=0; i<mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for(size_t j=0; j<face.mNumIndices; j++){
                indices.push_back(face.mIndices[j]);
            }
        }
       // std::cout<<"Here"<<std::endl;
        Texture diffuseMaps;
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        loadMaterialTextures(material, aiTextureType_DIFFUSE, diffuseMaps);
        textures.push_back(diffuseMaps);
        // aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // //material->mProperties[0].
        // std::vector<Texture> diffuseMaps; 
        // loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", diffuseMaps);
        // textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // std::cout<<"Here1"<<std::endl;
        // // 2. specular maps
        // std::vector<Texture> specularMaps;
        // loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", specularMaps);
        // textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // std::cout<<"Here2"<<std::endl;
        // // 3. normal maps
        // std::vector<Texture> normalMaps;
        // loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", normalMaps);
        // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // std::cout<<"Here3"<<std::endl;
        // // 4. height maps
        // std::vector<Texture> heightMaps;
        // loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", heightMaps);
        // textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        // std::cout<<"Here4"<<std::endl;
        //MeshPainter mesh;
        //mesh.Upload(vertices, indices, textures);
        //return mesh;
        return Mesh(vertices, textures, indices);

    }
        void Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, Texture& texture){
            aiString str;
            mat->GetTexture(type, 0, &str);
            texture.path = this->directory +"/"+str.C_Str();

        }
    //     void Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string type_Name, std::vector<Texture>& textures) {
    //         //vector<Texture> textures;
    //         for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    //         {
    //             std::cout<<"material "<<i<<std::endl;
    //             aiString str;
    //             mat->GetTexture(type, i, &str);
    //    //        std::cerr<<str.C_Str()<<std::endl;
    //          // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    //             bool skip = false;
    //             std::cout<<"material here"<<i<<std::endl;
    //             for(unsigned int j = 0; j < textures_loaded.size(); j++)
    //             { 
    //                 std::cout<<"materialj "<<j<<std::endl;
    //                 std::cerr<<textures_loaded[j].path.data()<<std::endl;
    //                 if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
    //                 {
    //                     textures.push_back(textures_loaded[j]);
    //                     skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
    //                     break;
    //                 }
    //             }
    //             std::cout<<"material her2"<<i<<std::endl;
    //             if(!skip)
    //             {   // if texture hasn't been loaded already, load it
    //                 Texture texture;
    //                 std::cout<<"MAT here"<<std::endl;
    //                 texture.id = TextureFromFile(str.C_Str(), this->directory);
    //                 std::cout<<"Mat here2"<<std::endl;
    //                 texture.type = type_Name;
    //                 texture.path = str.C_Str();
    //                 textures.push_back(texture);
    //                  textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
    //             }
    //         }
       // return textures;
   // }
    

} // namespace infrared