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



    highp float y = texture(SamplerY, coordinate).r;  
    highp float u = texture(SamplerU, coordinate).r - 0.5;  
    highp float v = texture(SamplerV, coordinate).r - 0.5;  
    highp float r = y +             1.402 * v;  
    highp float g = y - 0.344 * u - 0.714 * v;  
    highp float b = y + 1.772 * u;  
    FragColor = vec4(r, g, b, 1.0f);
} 