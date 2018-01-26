#include "openpose.h"

// OpenPose dependencies

#define FLAGS_logging_level            3              // "The logging level. Integer in the range [0, 255]. 0 will output any log() message, while"
                                                      // " 255 will not output any. Current OpenPose library messages are in the range 0-4: 1 for"
                                                      // " low priority messages and 4 for important ones."
                                                      // "Process the desired image."
// OpenPose
#define FLAGS_model_pose               "COCO"         // "Model to be used. E.g. `COCO` (18 keypoints), `MPI` (15 keypoints, ~10% faster), "
                                                      // "`MPI_4_layers` (15 keypoints, even faster but less accurate)."
#define FLAGS_model_folder             "/home/wiki/Tools/openpose/models/"
                                                      // "Folder path (absolute or relative) where the models (pose, face, ...) are located."
#define FLAGS_net_resolution           "-1x368"       // "Multiples of 16. If it is increased, the accuracy potentially increases. If it is"
                                                      // " decreased, the speed increases. For maximum speed-accuracy balance, it should keep the"
                                                      // " closest aspect ratio possible to the images or videos to be processed. Using `-1` in"
                                                      // " any of the dimensions, OP will choose the optimal aspect ratio depending on the user's"
                                                      // " input value. E.g. the default `-1x368` is equivalent to `656x368` in 16:9 resolutions,"
                                                      // " e.g. full HD (1980x1080) and HD (1280x720) resolutions."
#define FLAGS_output_resolution        "-1x-1"       // "The image resolution (display and output). Use \"-1x-1\" to force the program to use the"
                                                      // " input image resolution."
#define FLAGS_num_gpu_start             0             // "GPU device start number."
#define FLAGS_scale_gap                 0.3           // "Scale gap between scales. No effect unless scale_number > 1. Initial scale is always 1."
                                                      // " If you want to change the initial scale, you actually want to multiply the"
                                                      // " `net_resolution` by your desired initial scale."
#define FLAGS_scale_number              1             // "Number of scales to average."
// OpenPose Rendering
#define FLAGS_disable_blending          false         // "If enabled, it will render the results (keypoint skeletons or heatmaps) on a black"
                                                      // " background, instead of being rendered into the original image. Related: `part_to_show`,"
                                                      // " `alpha_pose`, and `alpha_pose`."
#define FLAGS_render_threshold          0.05          // "Only estimated keypoints whose score confidences are higher than this threshold will be"
                                                      // " rendered. Generally, a high threshold (> 0.5) will only render very clear body parts;"
                                                      // " while small thresholds (~0.1) will also output guessed and occluded keypoints, but also"
                                                      // " more false positives (i.e. wrong detections)."
#define FLAGS_alpha_pose                0.6           // "Blending factor (range 0-1) for the body part rendering. 1 will show it completely, 0 will"
                                                      // " hide it. Only valid for GPU rendering."

void OpenPose::run()
{
    // Step 2 - Read Google flags (user defined configuration)
    // poseModel
    const auto poseModel = op::flagsToPoseModel(FLAGS_model_pose);
    // Logging
    op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
    // Step 3 - Initialize all required classes
    op::CvMatToOpInput cvMatToOpInput;
    op::CvMatToOpOutput cvMatToOpOutput;
    op::PoseExtractorCaffe poseExtractorCaffe{poseModel, FLAGS_model_folder,
                                              FLAGS_num_gpu_start, {}, op::ScaleMode::ZeroToOne, true};
    op::PoseCpuRenderer poseRenderer{poseModel, (float)FLAGS_render_threshold, !FLAGS_disable_blending,
                                     (float)FLAGS_alpha_pose};
    // Step 4 - Initialize resources on desired thread (in this case single thread, i.e. we init resources here)
    poseExtractorCaffe.initializationOnThread();
    poseRenderer.initializationOnThread();
    op::OpOutputToCvMat opOutputToCvMat;

    // ------------------------- POSE ESTIMATION AND RENDERING -------------------------
    // Step 1 - Read and load image, error if empty (possibly wrong path)
    // Alternative: cv::imread(FLAGS_image_path, CV_LOAD_IMAGE_COLOR);
    if(inputImage.empty())
        op::error("Could not open or find the image: ", __LINE__, __FUNCTION__, __FILE__);
    const op::Point<int> imageSize{inputImage.cols, inputImage.rows};
    // Step 2 - Get desired scale sizes
    std::vector<double> scaleInputToNetInputs;
    std::vector<op::Point<int>> netInputSizes;
    double scaleInputToOutput;
    op::Point<int> outputResolution;
    std::tie(scaleInputToNetInputs, netInputSizes, scaleInputToOutput, outputResolution)
        = scaleAndSizeExtractor->extract(imageSize);
    // Step 3 - Format input image to OpenPose input and output formats
    const auto netInputArray = cvMatToOpInput.createArray(inputImage, scaleInputToNetInputs, netInputSizes);
    auto outputArray = cvMatToOpOutput.createArray(inputImage, scaleInputToOutput, outputResolution);
    // Step 4 - Estimate poseKeypoints
    poseExtractorCaffe.forwardPass(netInputArray, imageSize, scaleInputToNetInputs);
    poseKeypoints = poseExtractorCaffe.getPoseKeypoints();
    // Step 5 - Render poseKeypoints
    poseRenderer.renderPose(outputArray, poseKeypoints, scaleInputToOutput);
    // Step 6 - OpenPose output format to cv::Mat
    outputImage = opOutputToCvMat.formatToCvMat(outputArray);
//    op::log("Pose successfully finished.", op::Priority::High);
}

OpenPose::OpenPose()
{
    // ------------------------- INITIALIZATION -------------------------
    // Step 1 - Set logging level
        // - 0 will output all the logging messages
        // - 255 will output nothing
    op::check(0 <= FLAGS_logging_level && FLAGS_logging_level <= 255, "Wrong logging_level value.",
              __LINE__, __FUNCTION__, __FILE__);
    op::ConfigureLog::setPriorityThreshold((op::Priority)FLAGS_logging_level);
    op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
    // Check no contradictory flags enabled
    if (FLAGS_alpha_pose < 0. || FLAGS_alpha_pose > 1.)
        op::error("Alpha value for blending must be in the range [0,1].", __LINE__, __FUNCTION__, __FILE__);
    if (FLAGS_scale_gap <= 0. && FLAGS_scale_number > 1)
        op::error("Incompatible flag configuration: scale_gap must be greater than 0 or scale_number = 1.",
                  __LINE__, __FUNCTION__, __FILE__);
    // outputSize
    const auto outputSize = op::flagsToPoint(FLAGS_output_resolution, "-1x-1");
    // netInputSize
    const auto netInputSize = op::flagsToPoint(FLAGS_net_resolution, "-1x368");
    scaleAndSizeExtractor = new op::ScaleAndSizeExtractor(netInputSize, outputSize, FLAGS_scale_number, FLAGS_scale_gap);
    op::log("OpenPose Initial Successful.", op::Priority::High);
}

OpenPose::~OpenPose()
{
    op::log("OpenPose delete successful.", op::Priority::High);
}
