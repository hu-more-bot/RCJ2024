#include <sd.hpp>

#include <string>

enum SDMode
{
    TXT2IMG,
    IMG2IMG,
    CONVERT,
    MODE_COUNT
};

struct SDParams
{
    int n_threads = -1;
    SDMode mode = TXT2IMG;

    std::string model_path;
    std::string vae_path;
    std::string taesd_path;
    std::string esrgan_path;
    std::string controlnet_path;
    std::string embeddings_path;
    sd_type_t wtype = SD_TYPE_COUNT;
    std::string lora_model_dir;
    std::string output_path = "output.png";
    std::string input_path;
    std::string control_image_path;

    std::string prompt;
    std::string negative_prompt;
    float cfg_scale = 7.0f;
    int clip_skip = -1; // <= 0 represents unspecified
    int width = 512;
    int height = 512;
    int batch_count = 1;

    sample_method_t sample_method = EULER_A;
    schedule_t schedule = DEFAULT;
    int sample_steps = 20;
    float strength = 0.75f;
    float control_strength = 0.9f;
    rng_type_t rng_type = CUDA_RNG;
    int64_t seed = 42;
    bool verbose = false;
    bool vae_tiling = false;
    bool control_net_cpu = false;
    bool canny_preprocess = false;
};

SD::SD(const char *model, sd_type_t type)
{
    rng_type_t rng_type = CUDA_RNG;
    schedule_t schedule = DEFAULT;

    sd.ctx = new_sd_ctx(model,
                        "",                       // vae path
                        "",                       // taesd path
                        "",                       // controlnet path
                        "",                       // lora model dir
                        "",                       // embeddings path
                        true,                     // TODO false if img2img
                        false,                    // vae tiling
                        true,                     // free_params_immediately
                        get_num_physical_cores(), // num of threads
                        type,
                        rng_type,
                        schedule,
                        false // controlnet cpu
    );
}

SD::~SD()
{
    free_sd_ctx(sd.ctx);
}