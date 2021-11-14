//#version 150
//out vec4 FragColor;

//in vec2 TexCoords;
//in vec4 v_color;

//uniform sampler2D texture_map;

//void main(void)
//{    
    // FragColor = v_color;
    //FragColor = vec4(1, 0, 0, 0);
   // FragColor = texture2D(texture_map, TexCoords);
   // if(FragColor.r == 255 && FragColor.g == 0 && FragColor.b == 0)
    //FragColor = vec4(0,0,0,1);
   // FragColor = vec4(0,0,0,1);
//}


#version 330 core

in vec4 v_color;
out vec4 f_color;
in vec2 TexCoords;
uniform sampler2D texture_map;
void main(void) {
  //f_color = vec4(1.0, 0.0, 0.0, 1.0);
  //f_color = v_color;
   f_color = texture2D(texture_map, TexCoords);
   f_color = 0.5*f_color + 0.5*v_color;
   f_color.a = 1.0f;
}
