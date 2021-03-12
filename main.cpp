#include <iostream>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avassert.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
#endif
#include "const.h"
#include "render.h"
#include "saveframe/saveframe.h"
using namespace std;

static char *src_filename;
static AVFormatContext *fmt_ctx = NULL;
static int video_stream_idx;
static AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;

static AVFrame *frame = NULL;
static AVStream *video_stream = NULL, *audio_stream = NULL;
static enum AVPixelFormat pix_fmt;
int width, height;

static uint8_t *video_dst_data[4] = {NULL};
static int video_dst_linesize[4];
static int video_dst_bufsize;

int open_codec_context(int *stream_idx,
                       AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0)
    {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), src_filename);
        return ret;
    }
    else
    {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec)
        {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx)
        {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0)
        {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders */
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0)
        {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }

        *stream_idx = stream_index;
    }

    return 0;
}

static int output_video_frame(AVFrame *frame)
{
    if (DAFU_DEBUG)
    {
        //TODO 出发处理帧细节
        static int framecount = 0;
        framecount++;
        bool iskey = frame->key_frame == 1;
        AVPictureType type = frame->pict_type;

        cout << framecount << " ";
        if (AV_PICTURE_TYPE_I == type)
        {
            cout << "I";
        }
        else if (AV_PICTURE_TYPE_P == type)
        {
            cout << "P";
        }
        else
        {
            cout << "B";
        }

        // cout << "   " << frame->quality;

        cout << "   dts:    " << frame->pkt_dts;

        cout << "   pts " << frame->pts;

        cout << "   pkt_size    " << frame->pkt_size;

        cout << "   size    " << frame->width << ":" << frame->height;

        cout << "   framenum:   " << frame->coded_picture_number;

        int size = 0;
        for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)
        {
            size += frame->linesize[i];
        }

        cout << "   size:   " << size;

        SaveFrame(framecount, frame);

        cout << endl;
    }

    RenderFrame(frame);
}

static int decode_packet(AVCodecContext *dec, const AVPacket *pkt)
{
    int ret = 0;

    // usbmit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);

    if (ret < 0)
    {
        cout << "Errot submitting a packet fordecoding" << av_err2str(ret);
        return ret;
    }

    // get all the available frames from the decoder
    while (ret >= 0)
    {
        ret = avcodec_receive_frame(dec, frame);
        if (ret < 0)
        {
            // those two return values are special and mean there is not output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;
            cout << "Error during decoding" << av_err2str(ret) << endl;
            return ret;
        }

        // write the frame data to output file
        if (dec->codec->type == AVMEDIA_TYPE_VIDEO)
        {
            ret = output_video_frame(frame);
        }
        else
        {
            // ret = output_audio_frame(frame);
        }
        av_frame_unref(frame);
        if (ret < 0)
            return ret;
    }
    return 0;
}

int main(int, char **args)
{
    std::cout << "Hello, world!\n";
    char *input_filename = args[1];
    src_filename = args[1];
    av_register_all();
    int ret = -1;
    // 打开视频
    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0)
    {
        cout << "Could not open source file" << src_filename << endl;
    }

    // 查找音视频流信息
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0)
    {
        cout << "could not find stream information" << endl;
    }

    // 解码器
    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0)
    {

        video_stream = fmt_ctx->streams[video_stream_idx];
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;

        ret = av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);

        if (ret < 0)
        {
            cout << "Could not allocate raw video buffer\n"
                 << endl;
        }
        video_dst_bufsize = ret;
    }

    //初始化帧数据结构
    frame = av_frame_alloc();
    if (!frame)
    {
        cout << "Could not allocate frame\n";
        ret = 123;
        goto end;
    }

    // 读取音视频流
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    static int aaa = 0;
    /* read frames form the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0)
    {

        if (pkt.stream_index == video_stream_idx)
        {
            ret = decode_packet(video_dec_ctx, &pkt);
        }
        else
        {
            // ret = decode_packet(audio_dec_ctx, &pkt);
        }
        av_packet_unref(&pkt);
        if (ret < 0)
        {
            break;
        }
    }

end:
    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    dispose_render();
}