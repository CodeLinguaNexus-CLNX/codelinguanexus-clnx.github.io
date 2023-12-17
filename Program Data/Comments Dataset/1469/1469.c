/*
This function is responsible for probing a file and performing various operations based on specified options.

Here's the detail:
- The function first determines whether to read frames or packets based on the `do_show_frames` and `do_count_frames` options, and determines whether to read packets based on `do_show_packets` and `do_count_packets`.
- It then attempts to open the input file using the `open_input_file` function, and if unsuccessful, returns the error code.
- Memory is allocated for handling frame and packet counts and selected streams based on the number of streams in the format context.
- For each stream in the format context, stream specifier is checked, and the index of the matched stream is stored in `selected_streams` array. If no specifier is present, all streams are selected by default.
- Depending on the options for showing frames and packets, appropriate sections are identified, and the corresponding packets are read using the `read_packets` function. Section headers and footers are printed when necessary.
- Additionally, if specified, the function shows programs, streams, chapters, and format information using helper functions.
- Finally, allocated resources are freed, and the function returns the result of the operations.

Needs to notice:
- This function heavily relies on various options and operations defined in other parts of the program, such as opening and closing input files, reading packets, and showing information.
- All error conditions are handled with the `CHECK_END` macro, which simplifies the error handling logic.
- Changes to the behavior or flow of this function should consider the impact on the overall probing and information display process within the application.
*/
static int probe_file(WriterContext *wctx, const char *filename)

{

    AVFormatContext *fmt_ctx;

    int ret, i;

    int section_id;



    do_read_frames = do_show_frames || do_count_frames;

    do_read_packets = do_show_packets || do_count_packets;



    ret = open_input_file(&fmt_ctx, filename);

    if (ret < 0)

        return ret;



#define CHECK_END if (ret < 0) goto end



    nb_streams_frames  = av_calloc(fmt_ctx->nb_streams, sizeof(*nb_streams_frames));

    nb_streams_packets = av_calloc(fmt_ctx->nb_streams, sizeof(*nb_streams_packets));

    selected_streams   = av_calloc(fmt_ctx->nb_streams, sizeof(*selected_streams));



    for (i = 0; i < fmt_ctx->nb_streams; i++) {

        if (stream_specifier) {

            ret = avformat_match_stream_specifier(fmt_ctx,

                                                  fmt_ctx->streams[i],

                                                  stream_specifier);

            CHECK_END;

            else

                selected_streams[i] = ret;

            ret = 0;

        } else {

            selected_streams[i] = 1;

        }

    }



    if (do_read_frames || do_read_packets) {

        if (do_show_frames && do_show_packets &&

            wctx->writer->flags & WRITER_FLAG_PUT_PACKETS_AND_FRAMES_IN_SAME_CHAPTER)

            section_id = SECTION_ID_PACKETS_AND_FRAMES;

        else if (do_show_packets && !do_show_frames)

            section_id = SECTION_ID_PACKETS;

        else // (!do_show_packets && do_show_frames)

            section_id = SECTION_ID_FRAMES;

        if (do_show_frames || do_show_packets)

            writer_print_section_header(wctx, section_id);

        ret = read_packets(wctx, fmt_ctx);

        if (do_show_frames || do_show_packets)

            writer_print_section_footer(wctx);

        CHECK_END;

    }



    if (do_show_programs) {

        ret = show_programs(wctx, fmt_ctx);

        CHECK_END;

    }



    if (do_show_streams) {

        ret = show_streams(wctx, fmt_ctx);

        CHECK_END;

    }

    if (do_show_chapters) {

        ret = show_chapters(wctx, fmt_ctx);

        CHECK_END;

    }

    if (do_show_format) {

        ret = show_format(wctx, fmt_ctx);

        CHECK_END;

    }



end:

    close_input_file(&fmt_ctx);

    av_freep(&nb_streams_frames);

    av_freep(&nb_streams_packets);

    av_freep(&selected_streams);



    return ret;

}
