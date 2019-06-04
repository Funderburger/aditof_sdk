#if 1

#include "aditofdemocontroller.h"
#include "aditofdemoview.h"
#include <glog/logging.h>
#include <iostream>

int main(int argc, char **argv) {

    // Init google logging system
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = 1;

    auto controller = std::make_shared<AdiTofDemoController>();
    auto view =
        std::make_shared<AdiTofDemoView>(controller, "aditof-demo v0.1");
    view->render();

    return 0;
}

#else

#include <camera.h>
#include <frame.h>
#include <glog/logging.h>
#include <iostream>
#include <system.h>

using namespace aditof;

int main(int argc, char *argv[]) {

    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = 1;

    Status status = Status::OK;

    System system;
    status = system.initialize();
    if (status != Status::OK) {
        LOG(ERROR) << "Could not initialize system!";
        return 0;
    }

    std::vector<std::shared_ptr<Camera>> cameras;
    system.getCameraList(cameras);
    if (cameras.empty()) {
        LOG(WARNING) << "No cameras found";
        return 0;
    }

    auto camera = cameras.front();
    status = camera->initialize();
    if (status != Status::OK) {
        LOG(ERROR) << "Could not initialize camera!";
        return 0;
    }

    std::vector<std::string> frameTypes;
    camera->getAvailableFrameTypes(frameTypes);
    if (frameTypes.empty()) {
        std::cout << "no frame type avaialble!";
        return 0;
    }
    status = camera->setFrameType(frameTypes.front());
    if (status != Status::OK) {
        LOG(ERROR) << "Could not set camera frame type!";
        return 0;
    }

    std::vector<std::string> modes;
    camera->getAvailableModes(modes);
    if (modes.empty()) {
        LOG(ERROR) << "no camera modes available!";
        return 0;
    }
    status = camera->setMode(modes.front());
    if (status != Status::OK) {
        LOG(ERROR) << "Could not set camera mode!";
        return 0;
    }

    auto frame = std::make_shared<aditof::Frame>();

    status = camera->requestFrame(frame);
    if (status != Status::OK) {
        LOG(ERROR) << "Could not request frame!";
        return 0;
    } else {
        LOG(INFO) << "succesfully requested frame!";
    }

    uint16_t *data1;
    status = frame->getData(FrameDataType::RAW, &data1);

    if (status != Status::OK) {
        LOG(ERROR) << "Could not get frame data!";
        return 0;
    }

    if (!data1) {
        LOG(ERROR) << "no memory allocated in frame";
        return 0;
    }

    FrameDetails fDetails;
    frame->getDetails(fDetails);
    for (unsigned int i = 0; i < fDetails.width * fDetails.height; ++i) {
        std::cout << data1[i] << " ";
    }

    return 0;
}

#endif