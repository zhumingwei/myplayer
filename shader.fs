#version 330 core
out vec4 FragColor;
uniform sampler2D SamplerY;//sample2D的常量，用来获取I420数据的Y平面数据
uniform sampler2D SamplerU;//U平面
uniform sampler2D SamplerV;//V平面

in vec2 coordinate;//纹理坐标

const vec3 delyuv = vec3(-0.0/255.0,-128.0/255.0,-128.0/255.0);
const vec3 matYUVRGB1 = vec3(1.0,0.0,1.402);
const vec3 matYUVRGB2 = vec3(1.0,-0.344,-0.714);
const vec3 matYUVRGB3 = vec3(1.0,1.772,0.0);

void main()
{
    // vec3 CurResult;
    // vec3 yuv;
    // yuv.x = texture(SamplerY, coordinate).r;//因为是YUV的一个平面，所以采样后的r,g,b,a这四个参数的数值是一样的
    // yuv.y = texture(SamplerU, coordinate).r;
    // yuv.z = texture(SamplerV, coordinate).r;

    // yuv += delyuv;//读取值得范围是0-255，读取时要-128回归原值
    // CurResult.x = dot(yuv,matYUVRGB1);
    // CurResult.y = dot(yuv,matYUVRGB2);
    // CurResult.z = dot(yuv,matYUVRGB3);
    // 输出像素值给光栅器
    // FragColor = vec4(CurResult.rgb, 1.0f);
    // FragColor = vec4(CurResult.r, CurResult.g, CurResult.b, 1.0f);
    // FragColor = vec4(coordinate.xy,0.1f,1.0f);
    // FragColor = vec4(CurResult.r,0.0f,0.0f,1.0f);
    // FragColor = vec4(0.0f,CurResult.g,0.0f,1.0f);
    // FragColor = vec4(0.0f,0.0f,CurResult.b,1.0f);
    // FragColor = vec4(CurResult.r,1.0f,1.0f,1.0f);



    // float y = texture(SamplerY, coordinate).r;  
    // float u = texture(SamplerU, coordinate).r - 0.5f;  
    // float v = texture(SamplerV, coordinate).r - 0.5f;  
    // float r = y + 1.402f * v;  
    // float g = y - 0.344f * u - 0.714f * v;  
    // float b = y + 1.772f * u;  
    // FragColor = vec4(r, g, b, 1.0f);
    // FragColor = vec4(texture(SamplerV, coordinate).r, 1.0f, 1.0f, 1.0f);


            vec3 yuv;
            vec3 rgb;
            yuv.r = texture(SamplerY, coordinate).r - (16.0 / 255.0);
            yuv.g = texture(SamplerU, coordinate).r - 0.5;
            yuv.b = texture(SamplerV, coordinate).r - 0.5;
            rgb = mat3(1.164,  1.164,  1.164,
                       0, -0.392, 2.017,
                       1.596, -0.813,    0.0) * yuv;
            // FragColor = vec4(0.0f,0.0f,rgb.b, 1.0f);
            // FragColor = vec4(rgb.r, 0.0f,0.0f, 1.0f); // 红色
            // FragColor = vec4(rgb.r, rgb.g,0.0f, 1.0f); // 绿色
            // FragColor = vec4(0.0f, 0.0, rgb.b, 1.0f); // 绿色
            // FragColor = vec4(rgb.bbb, 1.0f); 
            FragColor = vec4(rgb.r, rgb.g,rgb.b , 0.0f); 
    
} 