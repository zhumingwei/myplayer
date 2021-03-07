#ifdef __cplusplus
extern "C"
{
#endif
#include <libavutil/avassert.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
#endif

int SaveFrame(long long count, AVFrame *frame);