/*
This function is responsible for parsing and processing the extent descriptions of a VMDK (Virtual Machine Disk) image file. It iterates through the extent descriptions provided in the input 'desc' string, extracts relevant information, and performs operations to create or open corresponding extent files within the block layer of the virtual disk.

Here's the detail:
- The function takes the VMDK extent description string 'desc', a BlockDriverState pointer 'bs' representing the block device state, and the path to the descriptor file 'desc_file_path'.
- It parses the extent descriptions from the 'desc' string, extracting information such as access mode, size in sectors, type of extent (flat or sparse), file name, and flat offset if applicable.
- It validates and processes each extent description, opening the associated extent file and performing operations based on the type of extent (flat or sparse).
- For flat extents, it adds the extent to the block layer and sets relevant properties such as start offset, and for sparse extents, it opens them as "COWD" (Copy-On-Write Differential) sparse files within the block layer.
- The function handles errors such as invalid input parameters, unsuccessful file openings, and unsupported extent types.
- It iterates through the extent descriptions by moving to the next line in the 'desc' string after processing each extent.

Needs to notice:
- This function is specifically tailored for handling VMDK extent descriptions, which are crucial for understanding the layout and structure of VMDK virtual disk files.
- It relies on sscanf to parse the extent descriptions, assuming a specific format for the input 'desc' string. Any changes to the format or unexpected input may lead to parsing errors.
- The function interacts with the block layer through operations such as opening extent files and adding them to the block layer, indicating its close integration with the virtual disk block I/O operations.
- Error handling and reporting are essential aspects of this function, ensuring that any errors during extent parsing or file opening are properly handled and reported.
- The function's behavior is intimately tied to the internals of VMDK file format and the block layer of the virtual disk subsystem, making the specifics of VMDK extent handling an important consideration for using this function within a larger virtualization or storage system context.
*/
static int vmdk_parse_extents(const char *desc, BlockDriverState *bs,

        const char *desc_file_path)

{

    int ret;

    char access[11];

    char type[11];

    char fname[512];

    const char *p = desc;

    int64_t sectors = 0;

    int64_t flat_offset;

    char extent_path[PATH_MAX];

    BlockDriverState *extent_file;

    Error *local_err = NULL;



    while (*p) {

        /* parse extent line:

         * RW [size in sectors] FLAT "file-name.vmdk" OFFSET

         * or

         * RW [size in sectors] SPARSE "file-name.vmdk"

         */

        flat_offset = -1;

        ret = sscanf(p, "%10s %" SCNd64 " %10s \"%511[^\n\r\"]\" %" SCNd64,

                access, &sectors, type, fname, &flat_offset);

        if (ret < 4 || strcmp(access, "RW")) {

            goto next_line;

        } else if (!strcmp(type, "FLAT")) {

            if (ret != 5 || flat_offset < 0) {

                return -EINVAL;

            }

        } else if (ret != 4) {

            return -EINVAL;

        }



        if (sectors <= 0 ||

            (strcmp(type, "FLAT") && strcmp(type, "SPARSE") &&

             strcmp(type, "VMFS") && strcmp(type, "VMFSSPARSE")) ||

            (strcmp(access, "RW"))) {

            goto next_line;

        }



        path_combine(extent_path, sizeof(extent_path),

                desc_file_path, fname);

        ret = bdrv_file_open(&extent_file, extent_path, NULL, bs->open_flags,

                             &local_err);

        if (ret) {

            qerror_report_err(local_err);

            error_free(local_err);

            return ret;

        }



        /* save to extents array */

        if (!strcmp(type, "FLAT") || !strcmp(type, "VMFS")) {

            /* FLAT extent */

            VmdkExtent *extent;



            ret = vmdk_add_extent(bs, extent_file, true, sectors,

                            0, 0, 0, 0, sectors, &extent);

            if (ret < 0) {

                return ret;

            }

            extent->flat_start_offset = flat_offset << 9;

        } else if (!strcmp(type, "SPARSE") || !strcmp(type, "VMFSSPARSE")) {

            /* SPARSE extent and VMFSSPARSE extent are both "COWD" sparse file*/

            ret = vmdk_open_sparse(bs, extent_file, bs->open_flags);

            if (ret) {

                bdrv_unref(extent_file);

                return ret;

            }

        } else {

            fprintf(stderr,

                "VMDK: Not supported extent type \"%s\""".\n", type);

            return -ENOTSUP;

        }

next_line:

        /* move to next line */

        while (*p && *p != '\n') {

            p++;

        }

        p++;

    }

    return 0;

}
