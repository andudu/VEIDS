#include "openpose.h"

// Debugging/Other
/*--------------------------------------------------------------------------------------------
 * "The logging level. Integer in the range [0, 255]. 0 will output any log() message, while"
 * " 255 will not output any. Current OpenPose library messages are in the range 0-4: 1 for"
 * " low priority messages and 4 for important ones."
--------------------------------------------------------------------------------------------*/
#define FLAGS_logging_level 0
/*--------------------------------------------------------------------------------------------
 * "It would slightly reduce the frame rate in order to highly reduce the lag. Mainly useful"
 * " for 1) Cases where it is needed a low latency (e.g. webcam in real-time scenarios with"
 * " low-range GPU devices); and 2) Debugging OpenPose when it is crashing to locate the"
 * " error."
--------------------------------------------------------------------------------------------*/
#define FLAGS_disable_multi_thread      false
// Producer
#define FLAGS_camera                    -1
#define FLAGS_camera_resolution         "640x480"
#define FLAGS_camera_fps                30.0
#define FLAGS_process_real_time         false
#define FLAGS_frame_flip                false
#define FLAGS_frame_rotate              0
// OpenPose
#define FLAGS_model_folder              "/home/wiki/Tools/openpose/models/"
#define FLAGS_keypoint_scale            0
// OpenPose Body Pose
#define FLAGS_model_pose                "COCO"
#define FLAGS_scale_number              1
#define FLAGS_scale_gap                 0.3
// OpenPose Body Pose Heatmaps
#define FLAGS_heatmaps_add_parts        false
#define FLAGS_heatmaps_add_bkg          false
#define FLAGS_heatmaps_add_PAFs         false
#define FLAGS_heatmaps_scale            2
// OpenPose Rendering
#define FLAGS_part_to_show              0
#define FLAGS_disable_blending          false
// OpenPose Rendering Pose
#define FLAGS_render_threshold          0.05
#define FLAGS_render_pose               2
#define FLAGS_alpha_pose                0.6
#define FLAGS_alpha_heatmap             0.7

void OpenPose::run()
{
    while (!want2exit)
    {
        std::shared_ptr<std::vector<UserDatum>> datumProcessed;
        if (opWrapper.waitAndPop(datumProcessed))
        {
            outputImage = datumProcessed->at(0).cvOutputData;
            emit outputDone();
            detector->poseKeypoints = datumProcessed->at(0).poseKeypoints;
            detector->start();
        }
    }
}

OpenPose::OpenPose(std::string path)
{
    detector = new MotionDetection(FLAGS_camera_resolution);
    std::string videoPath = "";
    std::string rtspPath = "";
    int cameraPath = -1;
    if(path.length() == 0)
        std::cout << "The path is wrong." << std::endl;
    else if(path.length() <= 2)
        cameraPath = atoi(path.c_str());
    else if((int)path.find("rtsp") != -1)
        rtspPath = path;
    else
        videoPath = path;
    // logging_level
    op::ConfigureLog::setPriorityThreshold((op::Priority)255);
    // Applying user defined configuration - Google flags to program variables
    // outputSize
    op::Point<int> outputSize,netInputSize;
    outputSize.x = -1; outputSize.y = -1;
    // netInputSize
    netInputSize.x = -1; netInputSize.y = 368;
    // producerType
    const auto producerSharedPtr = op::flagsToProducer("", videoPath, rtspPath, cameraPath, FLAGS_camera_resolution, FLAGS_camera_fps);
    // poseModel
    const auto poseModel = op::flagsToPoseModel(FLAGS_model_pose);
    // keypointScale
    const auto keypointScale = op::flagsToScaleMode(FLAGS_keypoint_scale);
    // heatmaps to add
    const auto heatMapTypes = op::flagsToHeatMaps(FLAGS_heatmaps_add_parts, FLAGS_heatmaps_add_bkg, FLAGS_heatmaps_add_PAFs);
    const auto heatMapScale = op::flagsToHeatMapScaleMode(FLAGS_heatmaps_scale);
    // Logging
    op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
    // Configure OpenPose
    op::log("Configuring OpenPose wrapper.", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
    // Pose configuration (use WrapperStructPose{} for default and recommended configuration)
    const op::WrapperStructPose wrapperStructPose{true, netInputSize, outputSize, keypointScale,-1, 0, FLAGS_scale_number,
                                                  (float)FLAGS_scale_gap, op::flagsToRenderMode(FLAGS_render_pose),
                                                  poseModel, !FLAGS_disable_blending, (float)FLAGS_alpha_pose,
                                                  (float)FLAGS_alpha_heatmap, FLAGS_part_to_show, FLAGS_model_folder,
                                                  heatMapTypes, heatMapScale, (float)FLAGS_render_threshold,true};
    // Producer (use default to disable any input)
    const op::WrapperStructInput wrapperStructInput{producerSharedPtr, 0, (unsigned long long)-1,
                                                    FLAGS_process_real_time, FLAGS_frame_flip, FLAGS_frame_rotate,
                                                    cameraPath==-1?true:false};
    // Configure wrapper
    opWrapper.configure(wrapperStructPose, wrapperStructInput);
    // Set to single-thread running (to debug and/or reduce latency)
    if (FLAGS_disable_multi_thread)
       opWrapper.disableMultiThreading();
    op::log("Starting thread(s)", op::Priority::High);
    opWrapper.start();
}

OpenPose::~OpenPose()
{
    op::log("Stopping thread(s)", op::Priority::High);
    opWrapper.stop();
    detector->terminate();
    detector->wait();
    delete detector;
}
