// Copyright (c) 2018, ETH Zurich and UNC Chapel Hill.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of ETH Zurich and UNC Chapel Hill nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: Johannes L. Schoenberger (jsch-at-demuc-dot-de)



#include "util/ply.h"

#include <fstream>

#include <Eigen/Core>

#include "util/logging.h"
#include "util/misc.h"

namespace infrared {
PlyMesh ReadPlyMesh(const std::string& path) {

  std::cerr<<"ReadPlyMesh"<<std::endl;
  std::ifstream file(path, std::ios::binary);
  std::string line;
  std::vector<PlyMeshVertex> points;
  //std::vector<PlyTexCoord>
  std::vector<PlyMeshFace> faces;
  PlyMesh Mesh;

  // The index of the property for ASCII PLY files.
  int X_index = -1;
  int Y_index = -1;
  int Z_index = -1;
  int NX_index = -1;
  int NY_index = -1;
  int NZ_index = -1;
  int R_index = -1;
  int G_index = -1;
  int B_index = -1;
  unsigned char face_num = -1;
  unsigned char tex_num = -1;
  int FV_index1 = -1;
  int FV_index2 = -1;
  int FV_index3 = -1;
  float texCoordX1 = -1;
  float texCoordX2 = -1;
  float texCoordY1 = -1;
  float texCoordY2 = -1;
  float texCoordZ1 = -1;
  float texCoordZ2 = -1;
  // Flag to use double precision in binary PLY files
  bool X_double = false;
  bool Y_double = false;
  bool Z_double = false;
  bool NX_double = false;
  bool NY_double = false;
  bool NZ_double = false;


  // The position in number of bytes of the property for binary PLY files.
  int X_byte_pos = -1;
  int Y_byte_pos = -1;
  int Z_byte_pos = -1;
  int NX_byte_pos = -1;
  int NY_byte_pos = -1;
  int NZ_byte_pos = -1;
  int R_byte_pos = -1;
  int G_byte_pos = -1;
  int B_byte_pos = -1;
  unsigned char face_num_byte_pos = -1;
  unsigned char tex_num_byte_pos = -1;
  int FV_ind1_byte_pos = -1;
  int FV_ind2_byte_pos = -1;
  int FV_ind3_byte_pos = -1;
  float texCoordX1_byte_pos = -1;
  float texCoordX2_byte_pos = -1;
  float texCoordY1_byte_pos = -1;
  float texCoordY2_byte_pos = -1;
  float texCoordZ1_byte_pos = -1;
  float texCoordZ2_byte_pos = -1;

  bool in_vertex_section = false;
  bool in_face_section = false;
  bool is_binary = false;
  bool is_little_endian = false;
  size_t ver_num_bytes_per_line = 0;
  size_t face_num_bytes_per_line = 0;
  size_t num_vertices = 0;
  size_t num_faces = 0;

  int ver_index = 0;
  int face_index = 0;
  while(std::getline(file, line)) {
    StringTrim(&line);
    std::cerr<<"HERE："<<line<<std::endl;

    if(line.empty()){
      continue;
    }
    if(line == "end_header"){
      break;
    }

    if(line.size() >= 6 && line.substr(0, 6) == "format"){
      if (line == "format ascii 1.0") {
        is_binary = false;
      } else if (line == "format binary_little_endian 1.0") {
        is_binary = true;
        is_little_endian = true;
      } else if (line == "format binary_big_endian 1.0") {
        is_binary = true;
        is_little_endian = false;
      }

    }

    const std::vector<std::string> line_elems = StringSplit(line, " ");

    if (line_elems.size() >= 3 && line_elems[0] == "element") {
      in_vertex_section = false;
      in_face_section = false;
      if (line_elems[1] == "vertex") {
        num_vertices = std::stoll(line_elems[2]);
        in_vertex_section = true;
        in_face_section = false;
      }
      else if(line_elems[1] == "face"){
        num_faces = std::stoll(line_elems[2]);
        in_face_section = true;
        in_vertex_section = false;
      }
      else if (std::stoll(line_elems[2]) > 0) {
        std::cout << "WARN: Only vertex and faces elements supported; ignoring "
                  << line_elems[1] << std::endl;
      }
      std::cerr<<num_vertices<<' '<<num_faces<<std::endl;
    }

    if(in_vertex_section){
      if (line_elems.size() >= 3 && line_elems[0] == "property") {
      CHECK(line_elems[1] == "float" || line_elems[1] == "float32" ||
            line_elems[1] == "double" || line_elems[1] == "float64" ||
            line_elems[1] == "uchar")
          << "PLY import only supports float, double, and uchar data types";

      if (line == "property float x" || line == "property float32 x" ||
          line == "property double x" || line == "property float64 x") {
        X_index = ver_index;
        X_byte_pos = ver_num_bytes_per_line;
        X_double = (line_elems[1] == "double" || line_elems[1] == "float64");
      } else if (line == "property float y" || line == "property float32 y" ||
                 line == "property double y" || line == "property float64 y") {
        Y_index = ver_index;
        Y_byte_pos = ver_num_bytes_per_line;
        Y_double = (line_elems[1] == "double" || line_elems[1] == "float64");
      } else if (line == "property float z" || line == "property float32 z" ||
                 line == "property double z" || line == "property float64 z") {
        Z_index = ver_index;
        Z_byte_pos = ver_num_bytes_per_line;
        Z_double = (line_elems[1] == "double" || line_elems[1] == "float64");
      } else if (line == "property float nx" || line == "property float32 nx" ||
                 line == "property double nx" ||
                 line == "property float64 nx") {
        NX_index = ver_index;
        NX_byte_pos = ver_num_bytes_per_line;
        NX_double = (line_elems[1] == "double" || line_elems[1] == "float64");
      } else if (line == "property float ny" || line == "property float32 ny" ||
                 line == "property double ny" ||
                 line == "property float64 ny") {
        NY_index = ver_index;
        NY_byte_pos = ver_num_bytes_per_line;
        NY_double = (line_elems[1] == "double" || line_elems[1] == "float64");
      } else if (line == "property float nz" || line == "property float32 nz" ||
                 line == "property double nz" ||
                 line == "property float64 nz") {
        NZ_index = ver_index;
        NZ_byte_pos = ver_num_bytes_per_line;
        NZ_double = (line_elems[1] == "double" || line_elems[1] == "float64");
      } else if (line == "property uchar r" || line == "property uchar red" ||
                 line == "property uchar diffuse_red" ||
                 line == "property uchar ambient_red" ||
                 line == "property uchar specular_red") {
        R_index = ver_index;
        R_byte_pos = ver_num_bytes_per_line;
      } else if (line == "property uchar g" || line == "property uchar green" ||
                 line == "property uchar diffuse_green" ||
                 line == "property uchar ambient_green" ||
                 line == "property uchar specular_green") {
        G_index = ver_index;
        G_byte_pos = ver_num_bytes_per_line;
      } else if (line == "property uchar b" || line == "property uchar blue" ||
                 line == "property uchar diffuse_blue" ||
                 line == "property uchar ambient_blue" ||
                 line == "property uchar specular_blue") {
        B_index = ver_index;
        B_byte_pos = ver_num_bytes_per_line;
      }

      ver_index += 1;
      if (line_elems[1] == "float" || line_elems[1] == "float32") {
        ver_num_bytes_per_line += 4;
      } else if (line_elems[1] == "double" || line_elems[1] == "float64") {
        ver_num_bytes_per_line += 8;
      } else if (line_elems[1] == "uchar") {
        ver_num_bytes_per_line += 1;
      } else {
        LOG(FATAL) << "Invalid data type: " << line_elems[1];
      }
    }
  }
  else if(in_face_section){
    if(line_elems.size() >= 3 && line_elems[0] == "property"){
      CHECK(line_elems[1] == "list") <<"Face property should be list";
    }
    // face index
    if(line == "property list uchar int vertex_indices"){
      face_num = face_index;
      face_num_byte_pos = face_num_bytes_per_line;
      FV_index1 = ++face_index;
      FV_index2 = ++face_index;
      FV_index3 = ++face_index;
      face_num_bytes_per_line+=4;
      FV_ind1_byte_pos = face_num_bytes_per_line;
      face_num_bytes_per_line+=4;
      FV_ind2_byte_pos = face_num_bytes_per_line;
      face_num_bytes_per_line+=4;
      FV_ind3_byte_pos = face_num_bytes_per_line;
      face_index+=1;
      face_num_bytes_per_line += 4;

    }
    else if(line == "property list uchar float texcoord"){
      tex_num = face_index;
      tex_num_byte_pos = face_num_bytes_per_line;
      texCoordX1 = ++face_index;
      texCoordX2 = ++face_index;
      texCoordY1 = ++face_index;
      texCoordY2 = ++face_index;
      texCoordZ1 = ++face_index;
      texCoordZ2 = ++face_index;
      face_num_bytes_per_line+=4;
      texCoordX1_byte_pos = face_num_bytes_per_line;
      face_num_bytes_per_line+=4;
      texCoordX2_byte_pos = face_num_bytes_per_line;
      face_num_bytes_per_line+=4;
      texCoordY1_byte_pos = face_num_bytes_per_line;
      face_num_bytes_per_line+=4;
      texCoordY2_byte_pos = face_num_bytes_per_line;
      face_num_bytes_per_line+=4;
      texCoordZ1_byte_pos = face_num_bytes_per_line;
      face_num_bytes_per_line+=4;
      texCoordZ2_byte_pos = face_num_bytes_per_line;
      face_index+=1;
      face_num_bytes_per_line += 4;
    }
    //cout<<
  ///  face_index+=1;
    std::cout<<"face_index"<<face_index<<std::endl;
   // face_num_bytes_per_line += 4;
   }
  }

  std::cerr<<"texCoordz2"<<texCoordZ2<<std::endl;
  const bool is_normal_missing =
      (NX_index == -1) || (NY_index == -1) || (NZ_index == -1);
  const bool is_rgb_missing =
      (R_index == -1) || (G_index == -1) || (B_index == -1);

  CHECK(X_index != -1 && Y_index != -1 && Z_index)
      << "Invalid PLY file format: x, y, z properties missing";

  points.reserve(num_vertices);
  faces.reserve(num_faces);

  if (is_binary) {
    std::vector<char> buffer(ver_num_bytes_per_line);
    for (size_t i = 0; i < num_vertices; ++i) {
      file.read(buffer.data(), ver_num_bytes_per_line);

      PlyMeshVertex point;

      if (is_little_endian) {
        point.x = LittleEndianToNative(
            X_double ? *reinterpret_cast<double*>(&buffer[X_byte_pos])
                     : *reinterpret_cast<float*>(&buffer[X_byte_pos]));
        point.y = LittleEndianToNative(
            Y_double ? *reinterpret_cast<double*>(&buffer[Y_byte_pos])
                     : *reinterpret_cast<float*>(&buffer[Y_byte_pos]));
        point.z = LittleEndianToNative(
            Z_double ? *reinterpret_cast<double*>(&buffer[Z_byte_pos])
                     : *reinterpret_cast<float*>(&buffer[Z_byte_pos]));

        if (!is_normal_missing) {
          point.nx = LittleEndianToNative(
              NX_double ? *reinterpret_cast<double*>(&buffer[NX_byte_pos])
                        : *reinterpret_cast<float*>(&buffer[NX_byte_pos]));
          point.ny = LittleEndianToNative(
              NY_double ? *reinterpret_cast<double*>(&buffer[NY_byte_pos])
                        : *reinterpret_cast<float*>(&buffer[NY_byte_pos]));
          point.nz = LittleEndianToNative(
              NZ_double ? *reinterpret_cast<double*>(&buffer[NZ_byte_pos])
                        : *reinterpret_cast<float*>(&buffer[NZ_byte_pos]));
        }

        if (!is_rgb_missing) {
          point.r = LittleEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[R_byte_pos]));
          point.g = LittleEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[G_byte_pos]));
          point.b = LittleEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[B_byte_pos]));
        }
      } else {
        point.x = BigEndianToNative(
            X_double ? *reinterpret_cast<double*>(&buffer[X_byte_pos])
                     : *reinterpret_cast<float*>(&buffer[X_byte_pos]));
        point.y = BigEndianToNative(
            Y_double ? *reinterpret_cast<double*>(&buffer[Y_byte_pos])
                     : *reinterpret_cast<float*>(&buffer[Y_byte_pos]));
        point.z = BigEndianToNative(
            Z_double ? *reinterpret_cast<double*>(&buffer[Z_byte_pos])
                     : *reinterpret_cast<float*>(&buffer[Z_byte_pos]));

        if (!is_normal_missing) {
          point.nx = BigEndianToNative(
              NX_double ? *reinterpret_cast<double*>(&buffer[NX_byte_pos])
                        : *reinterpret_cast<float*>(&buffer[NX_byte_pos]));
          point.ny = BigEndianToNative(
              NY_double ? *reinterpret_cast<double*>(&buffer[NY_byte_pos])
                        : *reinterpret_cast<float*>(&buffer[NY_byte_pos]));
          point.nz = BigEndianToNative(
              NZ_double ? *reinterpret_cast<double*>(&buffer[NZ_byte_pos])
                        : *reinterpret_cast<float*>(&buffer[NZ_byte_pos]));
           } 

        if (!is_rgb_missing) {
          point.r = BigEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[R_byte_pos]));
          point.g = BigEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[G_byte_pos]));
          point.b = BigEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[B_byte_pos]));
        }
      }

      points.push_back(point);
    }
    for(size_t i=0; i<num_faces; i++){
      file.read(buffer.data(), face_num_bytes_per_line);
      PlyMeshFace face;
      
      if(is_little_endian){
        // int tmp_face_num = LittleEndianToNative(
        //   *reinterpret_cast<unsigned char*>(&buffer[face_num_byte_pos])
        // );
        face.vertex_idx1 = LittleEndianToNative(
          *reinterpret_cast<int*>(&buffer[FV_ind1_byte_pos])
        );  
        face.vertex_idx2 = LittleEndianToNative(
          *reinterpret_cast<int*>(&buffer[FV_ind2_byte_pos])
        );
        face.vertex_idx3 = LittleEndianToNative(
          *reinterpret_cast<int*>(&buffer[FV_ind3_byte_pos])
        );
        // int tmp_tex_num = LittleEndianToNative(
        //   *reinterpret_cast<unsigned char*>(&buffer[tex_num_byte_pos])
        // );
        face.texture_idx1.u = LittleEndianToNative(
          *reinterpret_cast<float*>(&buffer[texCoordX1_byte_pos])
        );
        face.texture_idx1.v = LittleEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordX2_byte_pos])
        );
        face.texture_idx2.u = LittleEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordY1_byte_pos])
        );
        face.texture_idx2.v = LittleEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordY2_byte_pos])
        );
        face.texture_idx3.u = LittleEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordZ1_byte_pos])
        );
         face.texture_idx3.v = LittleEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordZ2_byte_pos])
        );
        faces.push_back(face);

      }
      else{

        // int tmp_face_num = BigEndianToNative(
        //   *reinterpret_cast<unsigned char*>(&buffer[face_num_byte_pos])
        // );
        face.vertex_idx1 = BigEndianToNative(
          *reinterpret_cast<int*>(&buffer[FV_ind1_byte_pos])
        );
        face.vertex_idx2 = BigEndianToNative(
          *reinterpret_cast<int*>(&buffer[FV_ind2_byte_pos])
        );
        face.vertex_idx3 = BigEndianToNative(
          *reinterpret_cast<int*>(&buffer[FV_ind3_byte_pos])
        );
        // int tmp_tex_num = BigEndianToNative(
        //   *reinterpret_cast<unsigned char*>(&buffer[tex_num_byte_pos])
        // );
        face.texture_idx1.u = BigEndianToNative(
          *reinterpret_cast<float*>(&buffer[texCoordX1_byte_pos])
        );
        face.texture_idx1.v = BigEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordX2_byte_pos])
        );
        face.texture_idx2.u = BigEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordY1_byte_pos])
        );
        face.texture_idx2.v = BigEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordY2_byte_pos])
        );
        face.texture_idx3.u = BigEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordZ1_byte_pos])
        );
        face.texture_idx3.v = BigEndianToNative(
           *reinterpret_cast<float*>(&buffer[texCoordZ2_byte_pos])
        );
        faces.push_back(face);

      }

    }
  } else {
    for(size_t i=0; i<num_vertices; i++){
      std::getline(file, line);
      //std::cerr<<line<<std::endl;
      StringTrim(&line);
      std::stringstream line_stream(line);

      std::string item;
      std::vector<std::string> items;
      while (!line_stream.eof()) {
        std::getline(line_stream, item, ' ');
        StringTrim(&item);
        items.push_back(item);
      }

      PlyMeshVertex point;

      point.x = std::stold(items.at(X_index));
      point.y = std::stold(items.at(Y_index));
      point.z = std::stold(items.at(Z_index));

      if (!is_normal_missing) {
        point.nx = std::stold(items.at(NX_index));
        point.ny = std::stold(items.at(NY_index));
        point.nz = std::stold(items.at(NZ_index));
      }

      if (!is_rgb_missing) {
        point.r = std::stoi(items.at(R_index));
        point.g = std::stoi(items.at(G_index));
        point.b = std::stoi(items.at(B_index));
      }

      points.push_back(point);
    
    }
    for(size_t i=0; i<num_faces; i++){

     // PlyMeshFace face;
      std::getline(file, line);
      StringTrim(&line);
      std::stringstream line_stream(line);

      std::string item;
      std::vector<std::string> items;
      while(!line_stream.eof()){
        std::getline(line_stream, item, ' ');
        StringTrim(&item);
        items.push_back(item);
      }
      PlyMeshFace face;
      face.vertex_idx1 = std::stoi(items.at(FV_index1));
      face.vertex_idx2 = std::stoi(items.at(FV_index2));
      face.vertex_idx3 = std::stoi(items.at(FV_index3));
      face.texture_idx1.u = std::stof(items.at(texCoordX1));
      face.texture_idx1.v = std::stof(items.at(texCoordX2));
      face.texture_idx2.u = std::stof(items.at(texCoordY1));
      face.texture_idx2.v = std::stof(items.at(texCoordY2));
      face.texture_idx3.u = std::stof(items.at(texCoordZ1));
      face.texture_idx3.v = std::stof(items.at(texCoordZ2));

      faces.push_back(face);
    }

   }

  Mesh.vertices = points;
  Mesh.faces = faces;
  return Mesh;

}
std::vector<PlyPoint> ReadPly(const std::string& path) {
  std::ifstream file(path, std::ios::binary);
  CHECK(file.is_open()) << path;

  std::vector<PlyPoint> points;

  std::string line;

  // The index of the property for ASCII PLY files.
  int X_index = -1;
  int Y_index = -1;
  int Z_index = -1;
  int NX_index = -1;
  int NY_index = -1;
  int NZ_index = -1;
  int R_index = -1;
  int G_index = -1;
  int B_index = -1;
  // Flag to use double precision in binary PLY files
  // bool X_double = false;
  // bool Y_double = false;
  // bool Z_double = false;
  // bool NX_double = false;
  // bool NY_double = false;
  // bool NZ_double = false;

  // The position in number of bytes of the property for binary PLY files.
  int X_byte_pos = -1;
  int Y_byte_pos = -1;
  int Z_byte_pos = -1;
  int NX_byte_pos = -1;
  int NY_byte_pos = -1;
  int NZ_byte_pos = -1;
  int R_byte_pos = -1;
  int G_byte_pos = -1;
  int B_byte_pos = -1;

  bool in_vertex_section = false;
  bool is_binary = false;
  bool is_little_endian = false;
  size_t num_bytes_per_line = 0;
  size_t num_vertices = 0;

  int index = 0;
  while (std::getline(file, line)) {
    StringTrim(&line);

    if (line.empty()) {
      continue;
    }

    if (line == "end_header") {
      break;
    }

    if (line.size() >= 6 && line.substr(0, 6) == "format") {
      if (line == "format ascii 1.0") {
        is_binary = false;
      } else if (line == "format binary_little_endian 1.0") {
        is_binary = true;
        is_little_endian = true;
      } else if (line == "format binary_big_endian 1.0") {
        is_binary = true;
        is_little_endian = false;
      }
    }

    const std::vector<std::string> line_elems = StringSplit(line, " ");

    if (line_elems.size() >= 3 && line_elems[0] == "element") {
      in_vertex_section = false;
      if (line_elems[1] == "vertex") {
        num_vertices = std::stoll(line_elems[2]);
        in_vertex_section = true;
      } else if (std::stoll(line_elems[2]) > 0) {
        LOG(FATAL) << "Only vertex elements supported";
      }
    }

    if (!in_vertex_section) {
      continue;
    }

    // Show diffuse, ambient, specular colors as regular colors.

    if (line_elems.size() >= 3 && line_elems[0] == "property") {
      CHECK(line_elems[1] == "float" || line_elems[1] == "float32" ||
            line_elems[1] == "uchar")
          << "PLY import only supports the float and uchar data types";

      if (line == "property float x" || line == "property float32 x") {
        X_index = index;
        X_byte_pos = num_bytes_per_line;
      } else if (line == "property float y" || line == "property float32 y") {
        Y_index = index;
        Y_byte_pos = num_bytes_per_line;
      } else if (line == "property float z" || line == "property float32 z") {
        Z_index = index;
        Z_byte_pos = num_bytes_per_line;
      } else if (line == "property float nx" || line == "property float32 nx") {
        NX_index = index;
        NX_byte_pos = num_bytes_per_line;
      } else if (line == "property float ny" || line == "property float32 ny") {
        NY_index = index;
        NY_byte_pos = num_bytes_per_line;
      } else if (line == "property float nz" || line == "property float32 nz") {
        NZ_index = index;
        NZ_byte_pos = num_bytes_per_line;
      } else if (line == "property uchar r" || line == "property uchar red" ||
                 line == "property uchar diffuse_red" ||
                 line == "property uchar ambient_red" ||
                 line == "property uchar specular_red") {
        R_index = index;
        R_byte_pos = num_bytes_per_line;
      } else if (line == "property uchar g" || line == "property uchar green" ||
                 line == "property uchar diffuse_green" ||
                 line == "property uchar ambient_green" ||
                 line == "property uchar specular_green") {
        G_index = index;
        G_byte_pos = num_bytes_per_line;
      } else if (line == "property uchar b" || line == "property uchar blue" ||
                 line == "property uchar diffuse_blue" ||
                 line == "property uchar ambient_blue" ||
                 line == "property uchar specular_blue") {
        B_index = index;
        B_byte_pos = num_bytes_per_line;
      }

      index += 1;
      if (line_elems[1] == "float" || line_elems[1] == "float32") {
        num_bytes_per_line += 4;
      } else if (line_elems[1] == "uchar") {
        num_bytes_per_line += 1;
      } else {
        LOG(FATAL) << "Invalid data type: " << line_elems[1];
      }
    }
  }

  const bool is_normal_missing =
      (NX_index == -1) || (NY_index == -1) || (NZ_index == -1);
  const bool is_rgb_missing =
      (R_index == -1) || (G_index == -1) || (B_index == -1);

  CHECK(X_index != -1 && Y_index != -1 && Z_index)
      << "Invalid PLY file format: x, y, z properties missing";

  points.reserve(num_vertices);

  if (is_binary) {
    std::vector<char> buffer(num_bytes_per_line);
    for (size_t i = 0; i < num_vertices; ++i) {
      file.read(buffer.data(), num_bytes_per_line);

      PlyPoint point;

      if (is_little_endian) {
        point.x = LittleEndianToNative(
            *reinterpret_cast<float*>(&buffer[X_byte_pos]));
        point.y = LittleEndianToNative(
            *reinterpret_cast<float*>(&buffer[Y_byte_pos]));
        point.z = LittleEndianToNative(
            *reinterpret_cast<float*>(&buffer[Z_byte_pos]));

        if (!is_normal_missing) {
          point.nx = LittleEndianToNative(
              *reinterpret_cast<float*>(&buffer[NX_byte_pos]));
          point.ny = LittleEndianToNative(
              *reinterpret_cast<float*>(&buffer[NY_byte_pos]));
          point.nz = LittleEndianToNative(
              *reinterpret_cast<float*>(&buffer[NZ_byte_pos]));
        }

        if (!is_rgb_missing) {
          point.r = LittleEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[R_byte_pos]));
          point.g = LittleEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[G_byte_pos]));
          point.b = LittleEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[B_byte_pos]));
        }
      } else {
        point.x =
            BigEndianToNative(*reinterpret_cast<float*>(&buffer[X_byte_pos]));
        point.y =
            BigEndianToNative(*reinterpret_cast<float*>(&buffer[Y_byte_pos]));
        point.z =
            BigEndianToNative(*reinterpret_cast<float*>(&buffer[Z_byte_pos]));

        if (!is_normal_missing) {
          point.nx = BigEndianToNative(
              *reinterpret_cast<float*>(&buffer[NX_byte_pos]));
          point.ny = BigEndianToNative(
              *reinterpret_cast<float*>(&buffer[NY_byte_pos]));
          point.nz = BigEndianToNative(
              *reinterpret_cast<float*>(&buffer[NZ_byte_pos]));
        }

        if (!is_rgb_missing) {
          point.r = BigEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[R_byte_pos]));
          point.g = BigEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[G_byte_pos]));
          point.b = BigEndianToNative(
              *reinterpret_cast<uint8_t*>(&buffer[B_byte_pos]));
        }
      }

      points.push_back(point);
    }
  }  else {
    while (std::getline(file, line)) {
      StringTrim(&line);
      std::stringstream line_stream(line);

      std::string item;
      std::vector<std::string> items;
      while (!line_stream.eof()) {
        std::getline(line_stream, item, ' ');
        StringTrim(&item);
        items.push_back(item);
      }

      PlyPoint point;

      point.x = std::stold(items.at(X_index));
      point.y = std::stold(items.at(Y_index));
      point.z = std::stold(items.at(Z_index));

      if (!is_normal_missing) {
        point.nx = std::stold(items.at(NX_index));
        point.ny = std::stold(items.at(NY_index));
        point.nz = std::stold(items.at(NZ_index));
      }

      if (!is_rgb_missing) {
        point.r = std::stoi(items.at(R_index));
        point.g = std::stoi(items.at(G_index));
        point.b = std::stoi(items.at(B_index));
      }

      points.push_back(point);
    }
  }

  return points;
}

void WriteTextPlyPoints(const std::string& path,
                        const std::vector<PlyPoint>& points,
                        const bool write_normal, const bool write_rgb) {
  std::ofstream file(path);
  CHECK(file.is_open()) << path;

  file << "ply" << std::endl;
  file << "format ascii 1.0" << std::endl;
  file << "element vertex " << points.size() << std::endl;

  file << "property float x" << std::endl;
  file << "property float y" << std::endl;
  file << "property float z" << std::endl;

  if (write_normal) {
    file << "property float nx" << std::endl;
    file << "property float ny" << std::endl;
    file << "property float nz" << std::endl;
  }

  if (write_rgb) {
    file << "property uchar red" << std::endl;
    file << "property uchar green" << std::endl;
    file << "property uchar blue" << std::endl;
  }

  file << "end_header" << std::endl;

  for (const auto& point : points) {
    file << point.x << " " << point.y << " " << point.z;

    if (write_normal) {
      file << " " << point.nx << " " << point.ny << " " << point.nz;
    }

    if (write_rgb) {
      file << " " << static_cast<int>(point.r) << " "
           << static_cast<int>(point.g) << " " << static_cast<int>(point.b);
    }

    file << std::endl;
  }

  file.close();
}

void WriteBinaryPlyPoints(const std::string& path,
                          const std::vector<PlyPoint>& points,
                          const bool write_normal, const bool write_rgb) {
  std::fstream text_file(path, std::ios::out);
  CHECK(text_file.is_open()) << path;

  text_file << "ply" << std::endl;
  text_file << "format binary_little_endian 1.0" << std::endl;
  text_file << "element vertex " << points.size() << std::endl;

  text_file << "property float x" << std::endl;
  text_file << "property float y" << std::endl;
  text_file << "property float z" << std::endl;

  if (write_normal) {
    text_file << "property float nx" << std::endl;
    text_file << "property float ny" << std::endl;
    text_file << "property float nz" << std::endl;
  }

  if (write_rgb) {
    text_file << "property uchar red" << std::endl;
    text_file << "property uchar green" << std::endl;
    text_file << "property uchar blue" << std::endl;
  }

  text_file << "end_header" << std::endl;
  text_file.close();

  std::fstream binary_file(path,
                           std::ios::out | std::ios::binary | std::ios::app);
  CHECK(binary_file.is_open()) << path;

  for (const auto& point : points) {
    WriteBinaryLittleEndian<float>(&binary_file, point.x);
    WriteBinaryLittleEndian<float>(&binary_file, point.y);
    WriteBinaryLittleEndian<float>(&binary_file, point.z);

    if (write_normal) {
      WriteBinaryLittleEndian<float>(&binary_file, point.nx);
      WriteBinaryLittleEndian<float>(&binary_file, point.ny);
      WriteBinaryLittleEndian<float>(&binary_file, point.nz);
    }

    if (write_rgb) {
      WriteBinaryLittleEndian<uint8_t>(&binary_file, point.r);
      WriteBinaryLittleEndian<uint8_t>(&binary_file, point.g);
      WriteBinaryLittleEndian<uint8_t>(&binary_file, point.b);
    }
  }

  binary_file.close();
}

void WriteTextPlyMesh(const std::string& path, const PlyMesh& mesh) {
  std::fstream file(path, std::ios::out);
  CHECK(file.is_open());

  file << "ply" << std::endl;
  file << "format ascii 1.0" << std::endl;
  file << "element vertex " << mesh.vertices.size() << std::endl;
  file << "property float x" << std::endl;
  file << "property float y" << std::endl;
  file << "property float z" << std::endl;
  file << "element face " << mesh.faces.size() << std::endl;
  file << "property list uchar int vertex_index" << std::endl;
  file << "end_header" << std::endl;

  for (const auto& vertex : mesh.vertices) {
    file << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
  }

  for (const auto& face : mesh.faces) {
    file << StringPrintf("3 %d %d %d", face.vertex_idx1, face.vertex_idx2,
                         face.vertex_idx3)
         << std::endl;
  }
}

void WriteBinaryPlyMesh(const std::string& path, const PlyMesh& mesh) {
  std::fstream text_file(path, std::ios::out);
  CHECK(text_file.is_open());

  text_file << "ply" << std::endl;
  text_file << "format binary_little_endian 1.0" << std::endl;
  text_file << "element vertex " << mesh.vertices.size() << std::endl;
  text_file << "property float x" << std::endl;
  text_file << "property float y" << std::endl;
  text_file << "property float z" << std::endl;
  text_file << "element face " << mesh.faces.size() << std::endl;
  text_file << "property list uchar int vertex_index" << std::endl;
  text_file << "end_header" << std::endl;
  text_file.close();

  std::fstream binary_file(path,
                           std::ios::out | std::ios::binary | std::ios::app);
  CHECK(binary_file.is_open()) << path;

  for (const auto& vertex : mesh.vertices) {
    WriteBinaryLittleEndian<float>(&binary_file, vertex.x);
    WriteBinaryLittleEndian<float>(&binary_file, vertex.y);
    WriteBinaryLittleEndian<float>(&binary_file, vertex.z);
  }

  for (const auto& face : mesh.faces) {
    CHECK_LT(face.vertex_idx1, mesh.vertices.size());
    CHECK_LT(face.vertex_idx2, mesh.vertices.size());
    CHECK_LT(face.vertex_idx3, mesh.vertices.size());
    const uint8_t kNumVertices = 3;
    WriteBinaryLittleEndian<uint8_t>(&binary_file, kNumVertices);
    WriteBinaryLittleEndian<int>(&binary_file, face.vertex_idx1);
    WriteBinaryLittleEndian<int>(&binary_file, face.vertex_idx2);
    WriteBinaryLittleEndian<int>(&binary_file, face.vertex_idx3);
  }

  binary_file.close();
}

}  // namespace infrared
