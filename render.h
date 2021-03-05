#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avassert.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <string.h>
#ifdef __cplusplus
}
#endif
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
int GetStringFromPath(char *strings, const char *path);
void feedTextureWithImageData(GLuint handlers[3], AVFrame *avframe);
void processInput(GLFWwindow *window);
void dfBindTexture(uint8_t *data, int width, int height, GLuint handler);
void RenderFrame(AVFrame *frame);
void dispose_render();