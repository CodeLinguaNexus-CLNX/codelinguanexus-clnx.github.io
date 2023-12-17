/*
This function is responsible for opening a URL for encrypted input using the provided URI and initializing the cryptographic context for decryption.

Here's the detail:
- The function takes a URLContext *h, a string URI, and flags as input parameters, representing the URL context, the URI to open, and additional flags for the operation.
- It checks if the URI starts with "crypto+" or "crypto:", indicating a supported encrypted URL format, and extracts the nested URL if present. If the URI format is not supported, an error is logged and an appropriate error code is returned.
- It validates the key and initialization vector (IV) lengths in the cryptographic context and returns an error if they are insufficient.
- If the operation involves writing (i.e., flags include AVIO_FLAG_WRITE), an error indicating that only decryption is supported at present is logged.
- It then attempts to open the nested URL for reading using ffurl_open, and if unsuccessful, an error is logged, and the function returns.
- Memory is allocated for the AES key expansion using av_mallocz, and if allocation fails, an out-of-memory error is returned.
- The AES key expansion is initialized with the provided key using av_aes_init.
- The URLContext's is_streamed field is set to 1 to indicate that the URL is a stream.

Needs to notice:
- This function assumes that the cryptographic context has been properly initialized and associated with the URLContext before invocation, and that the context has been appropriately allocated and initialized with the necessary cryptographic parameters.
- It directly operates on memory allocation and deallocation for cryptographic data, necessitating careful management of resources and potential risks associated with memory leaks or use-after-free errors.
- The function currently only supports decryption and may need to be expanded to support encryption if required.
- The use of cryptographic functions and algorithms, such as AES, demands a solid understanding of cryptographic principles and best practices to ensure secure operation and avoid vulnerabilities.
*/
static int crypto_open(URLContext *h, const char *uri, int flags)

{

    const char *nested_url;

    int ret;

    CryptoContext *c = h->priv_data;



    if (!av_strstart(uri, "crypto+", &nested_url) &&

        !av_strstart(uri, "crypto:", &nested_url)) {

        av_log(h, AV_LOG_ERROR, "Unsupported url %s\n", uri);

        ret = AVERROR(EINVAL);

        goto err;

    }



    if (c->keylen < BLOCKSIZE || c->ivlen < BLOCKSIZE) {

        av_log(h, AV_LOG_ERROR, "Key or IV not set\n");

        ret = AVERROR(EINVAL);

        goto err;

    }

    if (flags & AVIO_FLAG_WRITE) {

        av_log(h, AV_LOG_ERROR, "Only decryption is supported currently\n");

        ret = AVERROR(ENOSYS);

        goto err;

    }

    if ((ret = ffurl_open(&c->hd, nested_url, AVIO_FLAG_READ)) < 0) {

        av_log(h, AV_LOG_ERROR, "Unable to open input\n");

        goto err;

    }

    c->aes = av_mallocz(av_aes_size);

    if (!c->aes) {

        ret = AVERROR(ENOMEM);

        goto err;

    }



    av_aes_init(c->aes, c->key, 128, 1);



    h->is_streamed = 1;



    return 0;

err:

    av_free(c->key);

    av_free(c->iv);

    return ret;

}
